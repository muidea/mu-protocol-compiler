#ifndef __CMUP_STRUCT_H_0016__
#define __CMUP_STRUCT_H_0016__
#include "mupobject.h"
#include "mupvar.h"
#include <list>

class CMUPBase
    : public CMUPType
{
public:
    CMUPBase( );

    CMUPBase( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPBase( );

    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

    std::string getCalcSizeString( ) const;

    std::string getDecodeString( ) const;

    std::string getEncodeString( ) const;

};

class CMUPModule 
    : public CMUPObject
{
public:
    CMUPModule( );

    CMUPModule( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPModule( );

    void setType( 
        unsigned int type );

    unsigned int getType( ) const;

    void setBase( 
        CMUPBase* base );

    CMUPBase* getBase( ) const;

    void linkVar( 
        CMUPVar* var );

    std::list<CMUPVar*> const& getVars( ) const;

    virtual void release( );

    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

    // 代码生成相关实现
public:
    std::string structDeclare( ) const;

    std::string classDeclare( ) const;

    std::string listDeclare( ) const;

    std::string vectorDeclare( ) const;

    std::string assignDeclare( ) const;

    std::string classDefine( ) const;

    std::string classConstructDefine( ) const;

    std::string classAssignConstructDefine( ) const;

    std::string classDestructDefine( ) const;

    std::string classEncodeDefine( ) const;

    std::string classDecodeDefine( ) const;

    std::string classPureDecodeDefine( ) const;

    std::string classCalcSizeDefine( ) const;

protected:
    CMUPModule& operator=( 
        CMUPModule const& right );

    unsigned int _type;
    CMUPBase* _base;
    std::list<CMUPVar*> _varList;
};

#endif // __CMUP_STRUCT_H_0016__

