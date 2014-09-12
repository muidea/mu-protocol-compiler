#include "mucmdlineparser.h"
#include "mupfile.h"
#include "mupscript.h"
#include "mupmodule.h"
#include "mupimport.h"
#include "mupvar.h"
#include "mupdeclarestream.h"
#include "mupdefinestream.h"
#include "mupprecompiler.h"
#include "mupcompilerutil.h"
#include "mupcommon.h"
#include "mupconfig.h"
#include <string>
#include <list>
#include <vector>
#include <assert.h>
#include <iostream>

std::string g_nameSpace;
CMUPImport* g_curImport;
CMUPModule* g_curModule;
CMUPScript* g_curScript;
std::list<CMUPScript*> g_scriptList;

static std::list<CMUPFile> s_mpsFileList;
static std::vector<CMUPObject*> s_objectStack;

extern "C"
{

void showHelpTips(std::string const& usageDesc)
{
    std::cout << usageDesc << std::endl;
}

void outputInvalidSyntaxInfoImpl( 
    int lineNo, 
    char* errorInfo )
{
    assert(g_curScript);

    std::string strInfo;
    if (!s_objectStack.empty())
    {
        strInfo.append(s_objectStack.back()->getName());
    }

    g_curScript->traceInfo("Syntax Error", lineNo, strInfo);

    g_curScript->release();
    g_curScript = NULL;

    if (g_curModule)
    {
        g_curModule->release();
        g_curModule = NULL;
    }

    if (g_curImport)
    {
        g_curImport->release();
        g_curImport = NULL;
    }

    while(!s_objectStack.empty())
    {
        CMUPObject* object = s_objectStack.back();
        object->release();
        s_objectStack.pop_back();
    }
}

int initParserImpl( 
    int argc, 
    char *argv[])
{
    int iRet = -1;
    std::cout << PRODUCT_DECLARE << std::endl;
    std::cout << VERSION_DECLARE << std::endl;
    std::cout << AUTHOR_DECLARE << std::endl;
    std::cout << BUILD_TIME_DECLARE << std::endl;
    std::cout << "--------------------------------" << std::endl;

    CMUCmdLineParser cmdLineParser;
    do 
    {
        cmdLineParser.defineOption("file", "mupparser script file. For example, test.mps", CMUCmdLineParser::OptionRequiresValue);
        cmdLineParser.defineOption("include", "add a directory to the file include path", CMUCmdLineParser::OptionOptional);
        cmdLineParser.defineOption("output", "name of output file", CMUCmdLineParser::OptionOptional);
        cmdLineParser.defineOption("namespace", "define output classes\'s namespace", CMUCmdLineParser::OptionOptional);

        if (!cmdLineParser.parser(argc, (const char**)argv))
        {
            break;
        }

        std::list<std::string> incPaths;
        std::list<std::string> includePaths;
        cmdLineParser.getStringOptions("include", incPaths);
        std::list<std::string>::const_iterator iIter = incPaths.begin();
        for ( ; iIter != incPaths.end(); ++iIter )
        {
            includePaths.push_back(CMUPCommon::verifyPath(*iIter));
        }
        setIncludePaths(includePaths);

        std::string currentPath = CMUPCommon::getCurrentWorkPath();

        std::list<std::string> fileList;
        cmdLineParser.getStringOptions("file",fileList);
        iIter =fileList.begin();
        for ( ; iIter != fileList.end(); ++iIter )
        {
            std::string workPath;
            std::string fileName;
            /*如果是绝对路径，则需要分离出文件名*/
            if (CMUPCommon::isAbsolutePath(*iIter))
            {
                workPath = CMUPCommon::spliteFilePath(*iIter);
                fileName = *iIter;
            }
            else
            {
                workPath = currentPath;
                fileName = CMUPCommon::relativePath2absolutePath(currentPath + *iIter);
            }

            s_mpsFileList.push_back(CMUPFile(workPath, fileName, 0));
        }

        std::string outputPath;
        if (!cmdLineParser.getStringOption("output", outputPath))
        {
            outputPath = currentPath;
        }
        setOutputPath(CMUPCommon::verifyPath(outputPath));

        if (!cmdLineParser.getStringOption("namespace", g_nameSpace))
        {
            g_nameSpace = "MUPProtocol";
        }
        else
        {
            if (!CMUPCommon::isIdentifier(g_nameSpace))
            {
                std::cout << "Illegal namespace declare" << std::endl;

                iRet = -1;
                break;
            }
        }

        iRet = 0;
    } while (0);

    if (-1 == iRet)
    {
        showHelpTips(cmdLineParser.usageDescription("mupparser"));
    }

    return iRet;
}

FILE* getNextFileImpl()
{
    assert(NULL == g_curScript);

    FILE* fileHandle = NULL;
    while(!s_mpsFileList.empty())
    {
        CMUPFile const& mpsFile = s_mpsFileList.front();
        std::string fileName = mpsFile.getFileName();
        CMUPScript* script = findScript(fileName);
        if (NULL == script)
        {
            std::cout  << "\nParse script:\n" << fileName << std::endl;
            // 到这里说明该文件没有被解析过
            CMUFileLineInfo fileLineInfo;
            fileHandle = ::fopen(fileName.c_str(), "rt");
            fileHandle = CMUPreUtil::preParse(fileHandle, fileLineInfo);
            assert(NULL != fileHandle);

            script = new CMUPScript(mpsFile.getFileName(), 0, 0, mpsFile.getWorkPath(), mpsFile.getDeep(), fileLineInfo);
            g_curScript = script;
        }

        s_mpsFileList.pop_front();
        if (NULL != fileHandle)
        {
            break;
        }
    }

    return fileHandle;
}


void finalParserImpl()
{
    assert(s_objectStack.empty());

    std::list<CMUPScript*>::const_iterator iter = g_scriptList.begin();
    for ( ; iter != g_scriptList.end(); ++iter )
    {
        CMUPScript* curScript = *iter;

        std::cout  << "\nVerify script:\n" << curScript->getName() << std::endl;
        if (curScript->verify())
        {
            std::cout  << "\nGenerate code:\n" << curScript->getName() << std::endl;

            std::string scriptName = CMUPCommon::spliteName(CMUPCommon::spliteFileName(curScript->getName()));
            std::string scriptFileOutputPath = getScriptOutputPath(curScript);

            CMUPDeclareStream declareStream(scriptFileOutputPath);
            if(declareStream.open(scriptName))
            {
                declareStream << *curScript;

                declareStream.close();
            }

            if (!curScript->needSerializeDefine())
            {
                continue;
            }

            CMUPDefineStream defineStream(scriptFileOutputPath);
            if (defineStream.open(scriptName))
            {
                defineStream << *curScript;

                defineStream.close();
            }
        }
    }

    for ( iter = g_scriptList.begin(); iter != g_scriptList.end(); )
    {
        CMUPScript* pScript = *iter;

        pScript->release();
        iter = g_scriptList.erase(iter);
    }
}

bool isAvailableInStack( 
    unsigned int tokenID)
{
    switch (tokenID)
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
    case MUIDENTIFIER:
        return true;
    default:
        return false;
    }

    return false;
}

void appendTagImpl( 
    int tagID, 
    char* tagValue, 
    int lineNo )
{
    if (isAvailableInStack(tagID))
    {
        CMUPObject* object = new CMUPObject(tagValue, tagID, lineNo);
        s_objectStack.push_back(object);
    }
}

void parsePackageImpl()
{
    assert(!s_objectStack.empty());

    CMUPObject* object = s_objectStack.back();
    s_objectStack.pop_back();
    if (!g_curImport)
    {
        g_curImport = new CMUPImport();
    }

    g_curImport->linkPackage(object);
}

void parseImportImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(g_curImport);

    CMUPObject* object = s_objectStack.back();
    s_objectStack.pop_back();

    g_curImport->setName(object->getName());
    g_curImport->setValue(object->getValue());
    g_curImport->setLineNo(object->getLineNo());

    g_curScript->linkImport(g_curImport);
    g_curImport = NULL;
    object->release();
}

void parseBaseImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);

    CMUPObject* object = s_objectStack.back();
    s_objectStack.pop_back();

    if (!g_curModule)
    {
        g_curModule = new CMUPModule();
    }

    CMUPBase* base = new CMUPBase(object->getName(), object->getValue(), object->getLineNo());

    g_curModule->setBase(base);

    object->release();
}

void parseVarImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);

    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    assert(!s_objectStack.empty());
    CMUPObject* typeObject = s_objectStack.back();
    s_objectStack.pop_back();

    if (!g_curModule)
    {
        g_curModule = new CMUPModule();
    }

    CMUPVar* var = new CMUPVar(nameObject->getName(), nameObject->getValue(), nameObject->getLineNo());
    CMUPType* type = new CMUPType(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    var->setType(type);

    g_curModule->linkVar(var);

    nameObject->release();
    typeObject->release();

    if (MUSTRING == type->getValue())
    {
        CMUPImport* import = g_curScript->findImport(type->getName());
        if (NULL == import)
        {
            import = new CMUPImport(type->getName(), MUSTRING,0);

            g_curScript->linkImport(import);
        }
    }
}

void parseStructImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(g_curModule);

    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    g_curModule->setName(nameObject->getName());
    g_curModule->setValue(nameObject->getValue());
    g_curModule->setLineNo(nameObject->getLineNo());
    g_curModule->setType(MUSTRUCT);

    g_curScript->linkModule(g_curModule);
    g_curModule = NULL;

    nameObject->release();
}

void parseClassImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(g_curModule);

    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    g_curModule->setName(nameObject->getName());
    g_curModule->setValue(nameObject->getValue());
    g_curModule->setLineNo(nameObject->getLineNo());
    g_curModule->setType(MUCLASS);

    g_curScript->linkModule(g_curModule);
    g_curModule = NULL;

    nameObject->release();
}

void parseListImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(!g_curModule);

    CMUPObject* typeObject = s_objectStack.back();
    s_objectStack.pop_back();

    assert(!s_objectStack.empty());
    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    CMUPModule* module = new CMUPModule(nameObject->getName(), nameObject->getValue(), nameObject->getLineNo());
    module->setType(MULIST);

    CMUPVar* var = new CMUPVar(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    CMUPType* type = new CMUPType(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    var->setType(type);
    module->linkVar(var);

    g_curScript->linkModule(module);

    nameObject->release();
    typeObject->release();

    CMUPImport* import = g_curScript->findImport("mulist");
    if (NULL == import)
    {
        import = new CMUPImport("mulist", MULIST, 0);
        g_curScript->linkImport(import);
    }
}

void parseVectorImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(!g_curModule);

    CMUPObject* typeObject = s_objectStack.back();
    s_objectStack.pop_back();

    assert(!s_objectStack.empty());
    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    CMUPModule* module = new CMUPModule(nameObject->getName(), nameObject->getValue(), nameObject->getLineNo());
    module->setType(MUVECTOR);

    CMUPVar* var = new CMUPVar(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    CMUPType* type = new CMUPType(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    var->setType(type);
    module->linkVar(var);

    g_curScript->linkModule(module);

    nameObject->release();
    typeObject->release();

    CMUPImport* import = g_curScript->findImport("muvector");
    if (NULL == import)
    {
        import = new CMUPImport("muvector", MUVECTOR, 0);
        g_curScript->linkImport(import);
    }
}

void parseAssignImpl()
{
    assert(!s_objectStack.empty());
    assert(g_curScript);
    assert(!g_curModule);

    CMUPObject* typeObject = s_objectStack.back();
    s_objectStack.pop_back();

    assert(!s_objectStack.empty());
    CMUPObject* nameObject = s_objectStack.back();
    s_objectStack.pop_back();

    CMUPModule* module = new CMUPModule(nameObject->getName(), nameObject->getValue(), nameObject->getLineNo());
    module->setType(MUASSIGEN);

    CMUPVar* var = new CMUPVar(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    CMUPType* type = new CMUPType(typeObject->getName(), typeObject->getValue(), typeObject->getLineNo());
    var->setType(type);
    module->linkVar(var);

    g_curScript->linkModule(module);

    nameObject->release();
    typeObject->release();
}

void parseScriptImpl()
{
    assert(s_objectStack.empty());

    std::list<CMUPFile> fileList;

    g_curScript->getIncludeFiles(fileList);

    std::list<CMUPFile>::const_reverse_iterator iIter = fileList.rbegin();
    for ( ; iIter != fileList.rend(); ++iIter )
    {
        s_mpsFileList.push_front(*iIter);
    }

    g_scriptList.push_back(g_curScript);

    g_curScript = NULL;
}

};

