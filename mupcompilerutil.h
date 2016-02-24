#ifndef _MUPPARSER_UTIL_H_2048_
#define _MUPPARSER_UTIL_H_2048_
#include <string>
#include <list>

class CMUPImport;
class CMUPType;
class CMUPVar;
class CMUPBase;
class CMUPModule;
class CMUPScript;

void setOutputPath( 
    std::string const& outputPath );

std::string getScriptOutputPath( 
    CMUPScript* script );

void setIncludePaths( 
    std::list<std::string> const& includePaths );

// ������ʵ��module����
// bIncludeAssign = true ��ʾ���ػ������͵�assign/list/vector Module
CMUPModule* findReallyModule(
    std::string const& moduleName,
    bool bIncludeAssign );

// ��ȡ��ʵ��Typeֵ
// ������Ҫ��Ϊ�˻�ȡassignType��Ӧ����ʵ����
unsigned int getAssignTypeReallyModuleValue( 
    CMUPType* type );

CMUPScript* searchScript(
    std::string const& importFileName );

CMUPScript* findScript( 
    std::string const& scriptName );

//////////////////////////////////////////////////////////////////////////
// У����������Ƿ�Ϸ�
//////////////////////////////////////////////////////////////////////////
bool verifyImport( 
    CMUPImport* import );

bool verifyType( 
    CMUPType* type );

bool verifyVar( 
    CMUPVar* var, 
    bool assignFlag );

bool verifyBase( 
    CMUPBase* base );

bool verifyModule( 
    CMUPModule* module );

bool verifyScript( 
    CMUPScript* script );

bool verifyLoopBackScript( 
    CMUPScript* script );

//////////////////////////////////////////////////////////////////////////
// ������������
//////////////////////////////////////////////////////////////////////////
std::string declareImport( 
    const CMUPImport* import );

std::string declareType( 
    const CMUPType* type );

std::string declareModule( 
    const CMUPModule* module );

std::string declareScript( 
    const CMUPScript* script );

std::string defineImport( 
    const CMUPImport* import );

std::string defineType( 
    const CMUPType* type );

std::string defineModule( 
    const CMUPModule* module );

std::string defineScript( 
    const CMUPScript* script );

std::string getVarCalcSizeString( 
    const CMUPVar* var, 
    std::string preTag = "" );

std::string getVarDecodeString( 
    const CMUPVar* var, 
    std::string preTag = "" );

std::string getVarEncodeString( 
    const CMUPVar* var, 
    std::string preTag = "" );

bool checkScriptFileInIncludePath( 
    std::string const& mpsFile, 
    std::string& includePath );

#endif // _MUPPARSER_UTIL_H_2048_

