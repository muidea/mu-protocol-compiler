#ifndef __CMUP_IMPORT_H_0023__
#define __CMUP_IMPORT_H_0023__
#include "mupobject.h"
#include <list>

typedef CMUPObject CMUPPackage;

class CMUPImport
    : public CMUPObject
{
public:
    CMUPImport( );

    CMUPImport( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPImport( );

    void linkPackage( 
        CMUPPackage* package );

    std::list<CMUPPackage*> const& getPackages( ) const;

    void getImportFile( 
        std::string& fileName );

    virtual void release( );
    
    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

protected:
    CMUPImport& operator=( 
        CMUPImport const& right );

    std::list<CMUPPackage*> _packageList;
};

#endif // __CMUP_IMPORT_H_0023__

