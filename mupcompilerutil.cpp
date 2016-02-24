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

// ������ʵ��module����
// bIncludeAssign = true ��ʾ���ػ������͵�assign/list/vector Module
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

    // ����Ѿ��ҵ���Ӧ��Module������Ҫ�жϸ�Module�Ƿ���assign���͵ģ��������Ҫ��������
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

    // ����Ҳ�����Ӧ��import����˵����Module������
    if (NULL == import)
    {
        return NULL;
    }

    // ������������Ҫ���Ҷ�Ӧ��script�ļ�������Ҳ�������˵����ModuleҲ������
    std::string importFileName;
    import->getImportFile(importFileName);
    CMUPScript* script = searchScript(importFileName);
    if (NULL == script)
    {
        return NULL;
    }

    // ������Ƶ���import��Ӧ��script����
    s_scriptStack.push(script);
    module = findReallyModule(moduleName, bIncludeAssign);
    s_scriptStack.pop();

    return module;
}

// ��ȡ��ʵ��Typeֵ
// ������Ҫ��Ϊ�˻�ȡassignType��Ӧ����ʵ����
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
            // ������Զ������ͣ������ҵ������Ͷ�Ӧ����ʵModule��Ȼ��ȷ������ʵ������
            CMUPModule* pReallyMode = findReallyModule(type->getName(), true);
            assert(pReallyMode);
            if (MUASSIGEN == pReallyMode->getType())
            {
                // ������˵����Module��һ��assign���ͣ�����type�ǻ������ͣ�����Ӧ�÷��ػ������͵�typeValue
                std::list<CMUPVar*> const& vars = pReallyMode->getVars();
                assert(1 == vars.size());
                CMUPVar* var = vars.front();
                return var->getType()->getValue();
            }

            // ���ﷵ�ص�ֵ�����¼���struct��class��list��vector
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
    // 1�������Ƿ������curScript�ı���Ŀ¼��
    std::string localPath = CMUPCommon::spliteFilePath(s_scriptStack.top()->getName());
    script = findScript(localPath + importFileName);
    if (NULL != script)
    {
        return script;
    }

    // 2������importFile�Ƿ������curScript�Ĺ���Ŀ¼��
    std::string workPath = s_scriptStack.top()->getWorkPath();
    std::string scriptName = workPath + importFileName;
    script = findScript(scriptName);
    if (NULL != script)
    {
        return script;
    }

    // �ٿ����Ƿ������includeĿ¼��
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

/* 0���ж��Ƿ����ڲ�����
 * 1���ж϶�Ӧ��script�Ƿ����
 * 2���ж�import��module�Ƿ������script��
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

    // 3��ȷ�϶�Ӧ��Module�Ƿ���ڣ��������������Ϊ��import�Ƿ�
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

    // �����ϵͳ���ͣ���϶��ǺϷ���
    if (type->isSystemType())
    {
        return true;
    }

    // ������ڲ����ͣ���϶��ǺϷ���
    if (type->isInternalType())
    {
        return true;
    }

    assert(!s_scriptStack.empty());
    CMUPScript* currentScript = s_scriptStack.top();
    // ������ҵ���Ӧ��import�����ж���import�Ƿ�Ϸ�����
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

    // �����Ҫ�ӵ�ǰscript��module�б�������
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
        // ������˵��û�ҵ���Ӧ��Module��������ж�type�Ƿ�
        outputErrorInfo(type->getLineNo(),"Illegal type declare.");
        return false;
    }

    // ����������ж���module�Ƿ�Ϸ�
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

    // ֻҪȷ��type�Ϸ�����Ϊvar�Ϸ�
    CMUPType* type = var->getType();

    if (!type->verify())
    {
        outputErrorInfo(var->getLineNo(),"Illegal variable definition.");
        return false;
    }

    // �����assign/list/vector�ı���
    // �������������������ͬ�����������ͬ
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

    // ����Ҳ�����Ӧ��Module�����ʾ��base�ǷǷ���
    bool returnValue = true;
    CMUPModule* module = findReallyModule(base->getName(), false);
    if (NULL == module)
    {
        outputErrorInfo(base->getLineNo(),"Illegal base type declare.");
        returnValue = false;
    }

    // ֻ��class��������base type
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
    // ȷ��Base�ǺϷ���
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
    // ȷ��Var���ǺϷ���
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

    // ȷ��Varû�ж����ظ�
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
                // �������ֱ��������baseVar��������ظ������
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
    // 1��import�����ǺϷ���
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

    // import���ܳ����ظ���
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

    // module�����ǺϷ���
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

    // Module�����ظ�����
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

    // �����Գ���ѭ������
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
        // �������ͺ�ϵͳ������Զ������Ҫ�󷵻�defineString
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

    // ֻ��class��defineʵ��
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
    // ������Զ������ͣ���Ҫ������struct����class
    // �����struct����Ҫֱ�ӵ���calcSize
    // �����class����Ҫ���ñ�����Ӧ��calcSize
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

