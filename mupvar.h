#ifndef __CMUP_VAR_H_2347__
#define __CMUP_VAR_H_2347__
#include "mupobject.h"

class CMUPType 
    : public CMUPObject
{
public:
    CMUPType( );

    CMUPType( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPType( );

    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

    bool isSystemType() const;

    bool isInternalType() const;

    bool isCustomerType() const;
};

class CMUPVar
    : public CMUPObject
{
public:
    CMUPVar( );

    CMUPVar( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPVar( );

    void setType( 
        CMUPType* type );

    CMUPType* getType( ) const;

    virtual void release( );

    virtual bool verify( bool assignFlag = false );

    std::string getCalcSizeString( ) const;

    std::string getDecodeString( ) const;

    std::string getEncodeString( ) const;

protected:
    CMUPVar& operator=( 
        CMUPVar const& right );

    CMUPType* _type;
};

#endif // __CMUP_VAR_H_2347__

