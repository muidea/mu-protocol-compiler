#ifndef __CMUP_SCRIPT_0042__
#define __CMUP_SCRIPT_0042__
#include "mupimport.h"
#include "mupmodule.h"
#include "mupfile.h"
#include <list>
#include <map>

class CMUPScript
    : public CMUPObject
{
public:
    CMUPScript( );

    CMUPScript( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo, 
        std::string const& workPath, 
        int deep, 
        std::map<unsigned int, unsigned int> const& fileLineInfo);

    virtual ~CMUPScript( );

    std::string const& getWorkPath( ) const;

    int getDeep( ) const;

    std::map<unsigned int, unsigned int> const& getFileLineInfo() const;

    void linkImport( 
        CMUPImport* import);

    std::list<CMUPImport*> const& getImports() const;

    CMUPImport* findImport( 
        std::string const& name) const;

    void linkModule( 
        CMUPModule* module);

    std::list<CMUPModule*> const& getModules( ) const;

    CMUPModule* findModule( 
        std::string const& name) const;

    void getIncludeFiles( 
        std::list<CMUPFile>& fileList);

    void traceInfo( 
        std::string const& type, 
        unsigned int lineNo, 
        std::string const& info);

    bool needSerializeDefine() const;

    virtual void release( );

    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

protected:
    CMUPScript& operator=( 
        CMUPScript const& right);

    std::list<CMUPImport*> _importList;
    std::list<CMUPModule*> _moduleList;
    std::string _workPath;
    int _deep;
    std::map<unsigned int, unsigned int> _fileLineInfo;
};

CMUPDeclareStream& operator<<( 
    CMUPDeclareStream& os, 
    CMUPScript const& right);

CMUPDefineStream& operator<<( 
    CMUPDefineStream& os, 
    CMUPScript const& right);

#endif // __CMUP_SCRIPT_0042__

