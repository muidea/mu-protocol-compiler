#include "mupscript.h"
#include "mupcommon.h"
#include "mupcompilerutil.h"
#include <sstream>
#include <assert.h>

CMUPScript::CMUPScript( )
{
}

CMUPScript::CMUPScript( 
    std::string const& name, 
    unsigned int value, 
    unsigned int lineNo, 
    std::string const& workPath, 
    int deep,
    std::map<unsigned int, unsigned int> const& fileLineInfo )
    : CMUPObject(name, value, lineNo)
    , _workPath(workPath)
    , _deep(deep)
    , _fileLineInfo(fileLineInfo)
{
}

CMUPScript::~CMUPScript( )
{
}

std::string const& CMUPScript::getWorkPath( ) const
{
    return _workPath;
}

int CMUPScript::getDeep( ) const
{
    return _deep;
}

std::map<unsigned int, unsigned int> const& CMUPScript::getFileLineInfo( ) const
{
    return _fileLineInfo;
}

void CMUPScript::linkImport( 
    CMUPImport* import)
{
    _importList.push_back(import);
}

std::list<CMUPImport*> const& CMUPScript::getImports( ) const
{
    return _importList;
}

CMUPImport* CMUPScript::findImport( 
    std::string const& name) const
{
    CMUPImport* pImport = NULL;
    std::list<CMUPImport*>::const_iterator iIter = _importList.begin();
    for ( ; iIter != _importList.end(); ++iIter)
    {
        CMUPImport* pCur = *iIter;
        if (pCur->getName() == name)
        {
            pImport = pCur;
            break;
        }
    }

    return pImport;
}

void CMUPScript::linkModule( 
    CMUPModule* module)
{
    _moduleList.push_back(module);
}

std::list<CMUPModule*> const& CMUPScript::getModules( ) const
{
    return _moduleList;
}

CMUPModule* CMUPScript::findModule( 
    std::string const& name) const
{
    CMUPModule* module = NULL;
    std::list<CMUPModule*>::const_iterator iIter = _moduleList.begin();
    for ( ; iIter != _moduleList.end(); ++iIter)
    {
        CMUPModule* cur = *iIter;
        if (cur->getName() == name)
        {
            module = cur;
            break;
        }
    }

    return module;
}

void CMUPScript::getIncludeFiles( 
    std::list<CMUPFile>& fileList )
{
    std::string path = CMUPCommon::spliteFilePath(_name);
    std::list<CMUPImport*>::const_iterator iIter = _importList.begin();
    for( ; iIter != _importList.end(); ++iIter )
    {
        CMUPImport* importNode = *iIter;
        std::string mpsFile;
        importNode->getImportFile(mpsFile);
        if (mpsFile.empty())
        {
            // 如果获取不到对应的mps文件，则说明import是内置的string,list,vector这几种
            continue;
        }

        // 1、看script所在目录下是否存在该文件
        if (CMUPCommon::isExist(path + mpsFile))
        {
            CMUPFile file(_workPath, path + mpsFile, _deep + 1);
            fileList.push_back(file);
            continue;
        }
        
        // 2、看当前工作目录下是否存在该文件
        if (CMUPCommon::isExist(_workPath + mpsFile))
        {
            CMUPFile file(_workPath, _workPath + mpsFile, _deep + 1);
            fileList.push_back(file);
            continue;
        }

        // 3、如果当前目录不存在，则看include目录下是否存在该文件
        std::string includePath;
        if (checkScriptFileInIncludePath(mpsFile, includePath))
        {
            fileList.push_back(CMUPFile(includePath, includePath + mpsFile, _deep + 1));
            continue;
        }
    }
}

void CMUPScript::traceInfo( 
    std::string const& type, 
    unsigned int lineNo, 
    std::string const& info)
{
    unsigned int rawLine = _fileLineInfo[lineNo];

    CMUPCommon::traceInfo(type, _name, rawLine, info);
}

bool CMUPScript::needSerializeDefine() const
{
    std::list<CMUPModule*>::const_iterator iIter = _moduleList.begin();
    for ( ; iIter != _moduleList.end(); ++iIter )
    {
        CMUPModule* module = *iIter;
        if (MUCLASS == module->getType())
        {
            return true;
        }
    }

    return false;
}

void CMUPScript::release( )
{
    std::list<CMUPImport*>::const_iterator iIter = _importList.begin();
    for( ; iIter != _importList.end(); ++iIter )
    {
        CMUPImport* import = *iIter;
        import->release();
    }

    std::list<CMUPModule*>::const_iterator mIter = _moduleList.begin();
    for ( ; mIter != _moduleList.end(); ++mIter )
    {
        CMUPModule* module = *mIter;
        module->release();
    }

    CMUPObject::release();
}

bool CMUPScript::verify()
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;
        _illegalFlag = verifyScript(this);
    }

    return _illegalFlag;
}

std::string CMUPScript::getDeclareString() const
{
    return declareScript(this);
}

std::string CMUPScript::getDefineString( ) const
{
    return defineScript(this);
}

CMUPDeclareStream& operator<<( 
    CMUPDeclareStream& os, 
    CMUPScript const& right)
{
    os << right.getDeclareString();

    return os;
}

CMUPDefineStream& operator<<( 
    CMUPDefineStream& os, 
    CMUPScript const& right)
{
    os << right.getDefineString();

    return os;
}

