#include "mupvar.h"
#include "mupcommon.h"
#include "mupcompilerutil.h"
#include <list>
#include <assert.h>

CMUPType::CMUPType( )
{
}

CMUPType::CMUPType( 
    std::string const& name, 
    unsigned int value, 
    unsigned int lineNo )
    : CMUPObject(name, value, lineNo)
{
}

CMUPType::~CMUPType( )
{
}

bool CMUPType::verify( )
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;
        _illegalFlag = verifyType(this);
    }

    return _illegalFlag;
}

std::string CMUPType::getDeclareString( ) const
{
    return declareType(this);
}

std::string CMUPType::getDefineString( ) const
{
    return defineType(this);
}

bool CMUPType::isSystemType() const
{
    switch (getValue())
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
        return true;
    default:
        return false;
    }

    return false;
}

bool CMUPType::isInternalType() const
{
    switch(getValue())
    {
    case MUSTRING:
        return true;
    default:
        return false;
    }

    return false;
}

bool CMUPType::isCustomerType() const
{
    return MUIDENTIFIER == getValue();
}

CMUPVar::CMUPVar( )
{
}

CMUPVar::CMUPVar( 
    std::string const& name,
    unsigned int value,
    unsigned int lineNo )
    : CMUPObject(name, value, lineNo)
    , _type(NULL)
{
}

CMUPVar::~CMUPVar( )
{
}

void CMUPVar::setType( 
    CMUPType* type )
{
    _type = type;
}

CMUPType* CMUPVar::getType( ) const
{
    return _type;
}

void CMUPVar::release( )
{
    assert(_type);
    _type->release();

    CMUPObject::release();
}

/**
 ** Var类型和名称不允许相同
 **
 ***/
bool CMUPVar::verify( bool assignFlag /* = false */ )
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;
        _illegalFlag = verifyVar(this, assignFlag);
    }

    return _illegalFlag;
}


std::string CMUPVar::getCalcSizeString( ) const
{
    return getVarCalcSizeString(this,"_");
}

std::string CMUPVar::getDecodeString( ) const
{
    return getVarDecodeString(this,"_");
}

std::string CMUPVar::getEncodeString( ) const
{
    return getVarEncodeString(this,"_");
}

