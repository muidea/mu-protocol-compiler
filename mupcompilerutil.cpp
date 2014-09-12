#include "mupcompilerutil.h"
#include "mupimport.h"
#include "mupmodule.h"
#include "mupscript.h"
#include "mupcommon.h"
#include <list>
#include <stack>
#include <assert.h>
#include <sstream>

static std::string s_outputPath;
static std::list<std::string> s_includePaths;
static std::stack<CMUPModule*> s_moduleStack;
static std::stack<CMUPScript*> s_scriptStack;

extern std::list<CMUPScript*> g_scriptList;
extern std::string g_nameSpace;

void outputErrorInfo( 
    unsigned int lineNo, 
    std::string const& info )
{
    assert(!s_scriptStack.empty());
    CMUPScript* script = s_scriptStack.top();

    script->traceInfo("Semantic Error", lineNo, info);
}

void setOutputPath( 
    std::string const& outputPath )
{
    s_outputPath = outputPath;
}

std::string getScriptOutputPath( 
    CMUPScript* script )
{
    std::string workPath = script->getWorkPath();
    std::string scriptPath = CMUPCommon::spliteFilePath(script->getName());
    if (workPath.size() >= scriptPath.size())
    {
        return s_outputPath;
    }

    std::string subPath = scriptPath.substr(0, workPath.size());
    if (workPath != subPath)
    {
        return s_outputPath;
    }

    return CMUPCommon::verifyPath(s_outputPath + scriptPath.substr(workPath.size()));
}

void setIncludePaths( 
    std::list<std::string> const& includePaths )
{
    s_includePaths = includePaths;
}

// 查找真实的module类型
// bIncludeAssign = true 表示返回基本类型的assign/list/vector Module
CMUPModule* findReallyModule( 
    std::string const& moduleName, 
    bool bIncludeAssign )
{
    assert(!s_moduleStack.empty());
    assert(!s_scriptStack.empty());

    CMUPModule* module = NULL;
    std::list<CMUPModule*> const& modules = s_scriptStack.top()->getModules();
    std::list<CMUPModule*>::const_iterator mIter = modules.begin();
    for ( ; mIter != modules.end(); ++mIter )
    {
        CMUPModule* current = *mIter;
        if (current == s_moduleStack.top())
        {
            break;
        }
        if (current->getName() == moduleName)
        {
            module = current;
            break;
        }
    }

    // 如果已经找到对应的Module，则还需要判断该Module是否是assign类型的，如果是则要继续查找
    if (NULL != module)
    {
        if (MUASSIGEN == module->getType())
        {
            std::list<CMUPVar*> vars = module->getVars();
            assert(1 == vars.size());
            CMUPVar* var = vars.front();
            CMUPModule* tempModule = findReallyModule(var->getType()->getName(), bIncludeAssign);
            if (bIncludeAssign && (NULL == tempModule))
            {
                return module;
            }

            return tempModule;
        }

        return module;
    }

    CMUPImport* import = NULL;
    std::list<CMUPImport*> imports = s_scriptStack.top()->getImports();
    std::list<CMUPImport*>::const_iterator iIter = imports.begin();
    for ( ; iIter != imports.end(); ++iIter )
    {
        CMUPImport* current = *iIter;
        if (current->getName() == moduleName)
        {
            import = current;
            break;
        }
    }

    // 如果找不到对应的import，则说明该Module不存在
    if (NULL == import)
    {
        return NULL;
    }

    // 到这里我们需要查找对应的script文件，如果找不到，则说明该Module也不存在
    std::string importFileName;
    import->getImportFile(importFileName);
    CMUPScript* script = searchScript(importFileName);
    if (NULL == script)
    {
        return NULL;
    }

    // 这里递推的向import对应的script查找
    s_scriptStack.push(script);
    module = findReallyModule(moduleName, bIncludeAssign);
    s_scriptStack.pop();

    return module;
}

// 获取真实的Type值
// 这里主要是为了获取assignType对应的真实类型
unsigned int getAssignTypeReallyModuleValue( 
    CMUPType* type )
{
    switch(type->getValue())
    {
    case INT:
    case LONG:
    case FLOAT:
    case DOUBLE:
    case BOOL:
    case CHAR:
    case MUINT8:
    case MUINT16:
    case MUINT32:
    case MUINT64:
    case MUUINT:
    case MUUINT8:
    case MUUINT16:
    case MUUINT32:
    case MUUINT64:
    case MUFLOAT32:
    case MUFLOAT64:
    case MUULONG:
    case MUSTRING:
        return type->getValue();
        break;
    case MUIDENTIFIER:
        {
            // 如果是自定义类型，则先找到该类型对应的真实Module，然后确定其真实的类型
            CMUPModule* pReallyMode = findReallyModule(type->getName(), true);
            assert(pReallyMode);
            if (MUASSIGEN == pReallyMode->getType())
            {
                // 到这里说明该Module是一个assign类型，并且type是基本类型，这里应该返回基本类型的typeValue
                std::list<CMUPVar*> const& vars = pReallyMode->getVars();
                assert(1 == vars.size());
                CMUPVar* var = vars.front();
                return var->getType()->getValue();
            }

            // 这里返回的值有以下几种struct、class、list、vector
            return pReallyMode->getType();
        }
        break;
    default:
        assert(false);
    }

    assert(false);
    return 0;
}

CMUPScript* searchScript(
    std::string const& importFileName )
{
    assert(!s_scriptStack.empty());
    CMUPScript* script = NULL;
    // 1、看看是否存在于curScript的本地目录中
    std::string localPath = CMUPCommon::spliteFilePath(s_scriptStack.top()->getName());
    script = findScript(localPath + importFileName);
    if (NULL != script)
    {
        return script;
    }

    // 2、看看importFile是否存在于curScript的工作目录中
    std::string workPath = s_scriptStack.top()->getWorkPath();
    std::string scriptName = workPath + importFileName;
    script = findScript(scriptName);
    if (NULL != script)
    {
        return script;
    }

    // 再看看是否存在于include目录下
    std::list<std::string>::const_iterator iIter =s_includePaths.begin();
    for ( ; iIter != s_includePaths.end(); ++iIter )
    {
        scriptName = *iIter + importFileName;
        script = findScript(scriptName);
        if (NULL != script)
        {
            break;
        }
    }

    return script;
}

CMUPScript* findScript( 
    std::string const& scriptName )
{
    CMUPScript* script = NULL;
    std::list<CMUPScript*>::const_iterator iIter = g_scriptList.begin();
    for ( ; iIter != g_scriptList.end(); ++iIter )
    {
        script = *iIter;
        if (script->getName() == scriptName)
        {
            break;
        }

        script = NULL;
    }

    return script;
}

/* 0、判断是否是内部类型
 * 1、判断对应的script是否存在
 * 2、判断import的module是否存在于script中
 **/
bool verifyImport( 
    CMUPImport* import )
{
    assert(import);

    switch(import->getValue())
    {
    case MUSTRING:
    case MULIST:
    case MUVECTOR:
        return true;
        break;
    default:
        break;
    }

    CMUPScript* script = NULL;

    std::string importFileName;
    import->getImportFile(importFileName);
    script = searchScript(importFileName);
    if (NULL == script)
    {
        outputErrorInfo(import->getLineNo(),"Illegal import package declare.");
        return false;
    }

    // 3、确认对应的Module是否存在，如果不存在则认为该import非法
    CMUPModule* scriptModule = script->findModule(import->getName());
    if (NULL == scriptModule)
    {
        outputErrorInfo(import->getLineNo(),"Illegal import module declare.");
        return false;
    }

    return true;
}

bool verifyType( 
    CMUPType* type )
{
    assert(type);

    // 如果是系统类型，则肯定是合法的
    if (type->isSystemType())
    {
        return true;
    }

    // 如果是内部类型，则肯定是合法的
    if (type->isInternalType())
    {
        return true;
    }

    assert(!s_scriptStack.empty());
    CMUPScript* currentScript = s_scriptStack.top();
    // 如果能找到对应的import，则判定该import是否合法就行
    CMUPImport* import = currentScript->findImport(type->getName());
    if (NULL != import)
    {
        if (!import->verify())
        {
            outputErrorInfo(type->getLineNo(),"Illegal type declare.");
            return false;
        }

        return true;
    }

    // 否则就要从当前script的module列表中找了
    assert(!s_moduleStack.empty());
    CMUPModule* module = NULL;
    std::list<CMUPModule*> const& modules = currentScript->getModules();
    std::list<CMUPModule*>::const_iterator curIter = modules.begin();
    while(*curIter != s_moduleStack.top())
    {
        CMUPModule* current = *curIter;
        if (current->getName() == type->getName())
        {
            module = current;
            break;
        }

        curIter++;
    }

    if (NULL == module)
    {
        // 到这里说明没找到对应的Module，则可以判定type非法
        outputErrorInfo(type->getLineNo(),"Illegal type declare.");
        return false;
    }

    // 到这里继续判定该module是否合法
    if (!module->verify())
    {
        outputErrorInfo(type->getLineNo(),"Illegal type declare.");
        return false;
    }

    return true;
}

bool verifyVar( 
    CMUPVar* var, 
    bool assignFlag )
{
    assert(var);

    // 只要确认type合法就认为var合法
    CMUPType* type = var->getType();

    if (!type->verify())
    {
        outputErrorInfo(var->getLineNo(),"Illegal variable definition.");
        return false;
    }

    // 如果是assign/list/vector的变量
    // 则变量类型与名不能相同，否则必须相同
    bool returnValue = assignFlag ? (type->getName() == var->getName()) : (type->getName() != var->getName());
    if (!returnValue)
    {
        outputErrorInfo(var->getLineNo(),"Illegal variable definition.");
    }

    return returnValue;
}

bool verifyBase( 
    CMUPBase* base )
{
    assert(base);

    // 如果找不到对应的Module，则表示该base是非法的
    bool returnValue = true;
    CMUPModule* module = findReallyModule(base->getName(), false);
    if (NULL == module)
    {
        outputErrorInfo(base->getLineNo(),"Illegal base type declare.");
        returnValue = false;
    }

    // 只有class才允许是base type
    if (MUCLASS != module->getType())
    {
        outputErrorInfo(base->getLineNo(),"Illegal base type declare.");
        returnValue = false;
    }

    return returnValue;
}

bool verifyModule( 
    CMUPModule* module )
{
    assert(module);

    s_moduleStack.push(module);

    bool returnValue = true;

    std::list<CMUPVar*> baseVars;
    // 确认Base是合法的
    CMUPBase* base = module->getBase();
    if (base)
    {
        if (!base->verify())
        {
            returnValue = false;
        }

        CMUPModule* baseModule = findReallyModule(base->getName(), false);
        if (baseModule)
        {
            baseVars = baseModule->getVars();
        }
    }

    bool bAssignFlag = (MUASSIGEN == module->getType() || MULIST == module->getType() || MUVECTOR == module->getType());
    // 确认Var都是合法的
    std::list<CMUPVar*> const& vars = module->getVars();
    std::list<CMUPVar*>::const_iterator curIter = vars.begin();
    for ( ; curIter != vars.end(); ++curIter)
    {
        CMUPVar* var = *curIter;
        if (!var->verify(bAssignFlag))
        {
            returnValue = false;
        }
    }

    // 确认Var没有定义重复
    curIter = vars.begin();
    std::list<CMUPVar*>::const_iterator nextIter = curIter;
    std::list<CMUPVar*>::const_iterator baseIter;
    for ( nextIter++; nextIter != vars.end(); ++nextIter )
    {
        CMUPVar* currentVar = *curIter;
        CMUPVar* nextVar = *nextIter;
        if (currentVar->getName() == nextVar->getName())
        {
            returnValue = false;

            outputErrorInfo(currentVar->getLineNo(),"Conflict variable definition.");
        }

        for ( baseIter = baseVars.begin(); baseIter != baseVars.end(); ++baseIter )
        {
            CMUPVar* baseVar = *baseIter;
            if (currentVar->getName() == baseVar->getName())
            {
                returnValue = false;

                outputErrorInfo(currentVar->getLineNo(),"Conflict variable definition.");
                // 这里可以直接跳出，baseVar不会出现重复定义的
                break;
            }
        }

        curIter++;
    }

    if (!returnValue)
    {
        outputErrorInfo(module->getLineNo(),"Illegal module definition.");
    }

    s_moduleStack.pop();

    return returnValue;
}

bool verifyScript( 
    CMUPScript* script )
{
    assert(script);

    s_scriptStack.push(script);

    bool returnValue = true;
    // 1、import必须是合法的
    std::list<CMUPImport*> const& imports = script->getImports();
    std::list<CMUPImport*>::const_iterator curImportIter = imports.begin();
    for ( ; curImportIter != imports.end(); ++curImportIter)
    {
        CMUPImport* currentImport = *curImportIter;
        if (!currentImport->verify())
        {
            returnValue = false;
        }
    }

    // import不能出现重复的
    curImportIter = imports.begin();
    for ( ; curImportIter != imports.end(); ++curImportIter)
    {
        std::list<CMUPImport*>::const_iterator nextImportIter = curImportIter;
        for ( ++nextImportIter; nextImportIter != imports.end(); ++nextImportIter )
        {
            CMUPImport* currentImport = *curImportIter;
            CMUPImport* nextImport = *nextImportIter;
            if ( currentImport->getName() == nextImport->getName())
            {
                returnValue = false;

                outputErrorInfo(currentImport->getLineNo(),"Conflict import definition.");
                break;
            }
        }
    }

    // module必须是合法的
    std::list<CMUPModule*> const& modules = script->getModules();
    std::list<CMUPModule*>::const_iterator curModuleIter = modules.begin();
    for ( ; curModuleIter != modules.end(); ++curModuleIter)
    {
        CMUPModule* currentModule = *curModuleIter;
        if (!currentModule->verify())
        {
            returnValue = false;
        }
    }

    // Module不能重复定义
    curModuleIter = modules.begin();
    for ( ; curModuleIter != modules.end(); ++curModuleIter)
    {
        CMUPModule* currentModule = *curModuleIter;
        curImportIter = imports.begin();
        for ( ; curImportIter != imports.end(); ++curImportIter )
        {
            CMUPImport* currentImport = *curImportIter;
            if ( currentModule->getName() == currentImport->getName())
            {
                returnValue = false;

                outputErrorInfo(currentModule->getLineNo(),"Conflict module definition.");
                break;
            }
        }

        std::list<CMUPModule*>::const_iterator nextModuleIter =curModuleIter;
        for ( ++nextModuleIter; nextModuleIter != modules.end(); ++nextModuleIter )
        {
            CMUPModule* nextModule = *nextModuleIter;
            if ( currentModule->getName() == nextModule->getName())
            {
                returnValue = false;

                outputErrorInfo(currentModule->getLineNo(),"Conflict module definition.");
                break;
            }

            curModuleIter++;
        }
    }

    // 不可以出现循环依赖
    if (verifyLoopBackScript(script))
    {
        outputErrorInfo(script->getLineNo(),"Cyclic dependency script.");

        returnValue = false;
    }

    if (!returnValue)
    {
        outputErrorInfo(script->getLineNo(),"Illegal script definition.");
    }

    s_scriptStack.pop();

    return returnValue;
}

bool verifyLoopBackScript( 
    CMUPScript* script )
{
    assert(script);

    bool loopBackFlag = false;
    std::list<CMUPFile> fileList;
    CMUPScript* curScript = script;
    do 
    {
        std::list<CMUPFile> currentFileList;
        curScript->getIncludeFiles(currentFileList);

        if (!currentFileList.empty())
        {
            std::list<CMUPFile>::const_reverse_iterator iIter = currentFileList.rbegin();
            for ( ; iIter != currentFileList.rend(); ++iIter )
            {
                fileList.push_front(*iIter);
            }
        }

        if (!fileList.empty())
        {
            curScript = findScript(fileList.front().getFileName());
            if (curScript == script)
            {
                loopBackFlag = true;
                break;
            }

            fileList.pop_front();

            continue;
        }

        break;
    } while (!fileList.empty());

    return loopBackFlag;
}

std::string declareImport( 
    const CMUPImport* import )
{
    if (MUSTRING == import->getValue())
    {
        return "#include <string>";
    }
    if (MULIST == import->getValue())
    {
        return "#include <list>";
    }
    if (MUVECTOR == import->getValue())
    {
        return "#include <vector>";
    }

    std::stringstream os;
    os << "#include \"";
    std::list<CMUPPackage*> const& packageList = import->getPackages();
    std::list<CMUPPackage*>::const_iterator pIter = packageList.begin();
    std::list<CMUPPackage*>::const_iterator nIter;
    for ( ; pIter != packageList.end(); ++pIter)
    {
        CMUPPackage* pPackage = *pIter;
        os << pPackage->getDeclareString();

        nIter = pIter;
        nIter++;
        if (nIter != packageList.end())
        {
            os << "/";
        }
    }

    os << ".h\"";

    return os.str();
}

std::string declareType( 
    const CMUPType* type )
{
    if (type->isInternalType())
    {
        return "std::string";
    }

    if (type->isSystemType())
    {
        switch (type->getValue())
        {
        case INT:
        case LONG:
        case FLOAT:
        case DOUBLE:
        case BOOL:
        case CHAR:
            return type->getName();
        case MUINT8:
            return "INT8";
        case MUINT16:
            return "INT16";
        case MUINT32:
            return "INT32";
        case MUINT64:
            return "INT64";
        case MUUINT:
            return "UINT";
        case MUUINT8:
            return "UINT8";
        case MUUINT16:
            return "UINT16";
        case MUUINT32:
            return "UINT32";
        case MUUINT64:
            return "UINT64";
        case MUFLOAT32:
            return "FLOAT";
        case MUFLOAT64:
            return "DOUBLE";
        case MUULONG:
            return "ULONG";
        default:
            assert(false);
        }
    }

    return MUP_PREFLAG + type->getName();
}

std::string declareModule( 
    const CMUPModule* module )
{
    std::stringstream os;

    s_moduleStack.push((CMUPModule*)module);

    if (MUSTRUCT == module->getType())
    {
        os << module->structDeclare();
    }
    if (MUCLASS == module->getType())
    {
        os << module->classDeclare();
    }
    if (MULIST == module->getType())
    {
        os << module->listDeclare();
    }
    if (MUVECTOR == module->getType())
    {
        os << module->vectorDeclare();
    }
    if (MUASSIGEN == module->getType())
    {
        os << module->assignDeclare();
    }

    s_moduleStack.pop();

    return os.str();
}

std::string declareScript( 
    const CMUPScript* script )
{
    std::stringstream os;

    s_scriptStack.push((CMUPScript*)script);

    std::list<CMUPImport*> importList = script->getImports();
    std::list<CMUPImport*>::const_iterator iIter = importList.begin();
    for ( ; iIter != importList.end(); ++iIter )
    {
        CMUPImport* import = *iIter;
        std::string include = import->getDeclareString();
        std::list<CMUPImport*>::const_iterator nIter = iIter;
        bool conflictFlag = false;
        for ( ++nIter; nIter != importList.end(); ++nIter )
        {
            CMUPImport* nextImport = *nIter;
            std::string nextInclude = nextImport->getDeclareString();
            if (include == nextInclude)
            {
                conflictFlag = true;
                break;
            }
        }

        if (!conflictFlag)
        {
            os << include << "\n";
        }
    }

    os << "\n\n";
    os << "namespace " << g_nameSpace << " {\n\n";

    std::list<CMUPModule*> moduleList = script->getModules();
    std::list<CMUPModule*>::const_iterator sIter = moduleList.begin();
    for ( ; sIter != moduleList.end(); ++sIter )
    {
        CMUPModule* module = *sIter;
        os << module->getDeclareString();
    }

    os << "\n}\n\n";

    s_scriptStack.pop();

    return os.str();
}

std::string defineImport( 
    const CMUPImport* import )
{
    // noting todo
    return "";
}

std::string defineType( 
    const CMUPType* type )
{
    if (type->isInternalType() || type->isSystemType())
    {
        // 内置类型和系统类型永远都不会要求返回defineString
        assert(false);
        return "";
    }

    return MUP_PREFLAG + type->getName();
}

std::string defineModule( 
    const CMUPModule* module )
{
    std::stringstream os;

    s_moduleStack.push((CMUPModule*)module);

    if (MUCLASS == module->getType())
    {
        os << module->classDefine();
    }

    s_moduleStack.pop();

    // 只有class有define实现
    return os.str();
}

std::string defineScript( 
    const CMUPScript* script )
{
    std::stringstream os;

    s_scriptStack.push((CMUPScript*)script);

    os << "\n";
    os << "namespace " << g_nameSpace << " {\n\n";

    std::list<CMUPModule*> moduleList = script->getModules();
    std::list<CMUPModule*>::const_iterator sIter = moduleList.begin();
    for ( ; sIter != moduleList.end(); ++sIter )
    {
        CMUPModule* module = *sIter;
        os << module->getDefineString();
    }

    os << "\n}\n\n";

    s_scriptStack.pop();

    return os.str();
}

std::string getVarCalcSizeString(
    const CMUPVar* var, 
    std::string preTag /* = "" */ )
{
    CMUPType* type = var->getType();
    switch(getAssignTypeReallyModuleValue(type))
    {
    case INT:
    case LONG:
    case FLOAT:
    case DOUBLE:
    case BOOL:
    case CHAR:
    case MUINT8:
    case MUINT16:
    case MUINT32:
    case MUINT64:
    case MUUINT:
    case MUUINT8:
    case MUUINT16:
    case MUUINT32:
    case MUUINT64:
    case MUFLOAT32:
    case MUFLOAT64:
    case MUULONG:
    case MUSTRING:
    case MULIST:
    case MUVECTOR:
        return "MUPProtocol::getSize(" + preTag + var->getName() + ")";
        break;
    // 如果是自定义类型，则要区分是struct还是class
    // 如果是struct则需要直接调用calcSize
    // 如果是class则需要调用变量对应的calcSize
    case MUSTRUCT:
        return g_nameSpace + "::getSize(" + preTag + var->getName() + ")";
        break;
    case MUCLASS:
        return preTag + var->getName() + ".calcSize()";
        break;
    default:
        assert(false);
    }

    assert(false);
    return "";
}

std::string getVarDecodeString( 
    const CMUPVar* var, 
    std::string preTag /* = "" */ )
{
    CMUPType* type = var->getType();
    switch(getAssignTypeReallyModuleValue(type))
    {
    case INT:
    case LONG:
    case FLOAT:
    case DOUBLE:
    case BOOL:
    case CHAR:
    case MUINT8:
    case MUINT16:
    case MUINT32:
    case MUINT64:
    case MUUINT:
    case MUUINT8:
    case MUUINT16:
    case MUUINT32:
    case MUUINT64:
    case MUFLOAT32:
    case MUFLOAT64:
    case MUULONG:
    case MUSTRING:
    case MULIST:
    case MUVECTOR:
        return "MUPProtocol::decode((char*)pDataPtr + uDataSize - uRemainSize, uRemainSize, " + preTag + var->getName() + ", uRemainSize)";
        break;
    case MUSTRUCT:
        return g_nameSpace + "::decode((char*)pDataPtr + uDataSize - uRemainSize, uRemainSize, " + preTag + var->getName() + ", uRemainSize)";
        break;
    case MUCLASS:
        return preTag + var->getName() + ".decode((char*)pDataPtr + uDataSize - uRemainSize, uRemainSize, uRemainSize)";
        break;
    default:
        assert(false);
        break;
    }

    assert(false);
    return "";
}

std::string getVarEncodeString( 
    const CMUPVar* var, 
    std::string preTag /* = "" */ )
{
    CMUPType* type = var->getType();
    switch(getAssignTypeReallyModuleValue(type))
    {
    case INT:
    case LONG:
    case FLOAT:
    case DOUBLE:
    case BOOL:
    case CHAR:
    case MUINT8:
    case MUINT16:
    case MUINT32:
    case MUINT64:
    case MUUINT:
    case MUUINT8:
    case MUUINT16:
    case MUUINT32:
    case MUUINT64:
    case MUFLOAT32:
    case MUFLOAT64:
    case MUULONG:
    case MUSTRING:
    case MULIST:
    case MUVECTOR:
        return "MUPProtocol::encode(" + preTag + var->getName() + ", (char*)pBuffPtr + uBuffSize - uRemainSize, uRemainSize, uRemainSize)";
        break;
    case MUSTRUCT:
        return g_nameSpace +"::encode(" + preTag + var->getName() + ", (char*)pBuffPtr + uBuffSize - uRemainSize, uRemainSize, uRemainSize)";
        break;
    case MUCLASS:
        return preTag + var->getName() + ".encode((char*)pBuffPtr + uBuffSize - uRemainSize, uRemainSize, uRemainSize)";
        break;
    default:
        assert(false);
        break;
    }

    assert(false);
    return "";
}

bool checkScriptFileInIncludePath( 
    std::string const& mpsFile, 
    std::string& includePath )
{
    bool bRet = false;
    std::list<std::string>::const_iterator cIter = s_includePaths.begin();
    for ( ; cIter != s_includePaths.end(); ++cIter )
    {
        if (CMUPCommon::isExist(*cIter + mpsFile))
        {
            includePath =*cIter;
            bRet = true;
            break;
        }
    }

    return bRet;
}

