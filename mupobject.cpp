#include "mupobject.h"
#include <assert.h>

CMUPObject::CMUPObject( )
    : _illegalFlag(false)
    , _verifiedFlag(false)
{
}

CMUPObject::CMUPObject( 
    std::string const& name,
    unsigned int value,
    unsigned int lineNo )
    :_name(name)
    , _nameValue(value)
    , _nameLineNo(lineNo)
    , _illegalFlag(false)
    , _verifiedFlag(false)
{
}

CMUPObject::~CMUPObject( )
{
}

void CMUPObject::setName( 
    std::string const& name )
{
    _name = name;
}

std::string const& CMUPObject::getName( ) const
{
    return _name;
}

void CMUPObject::setValue( 
    unsigned int value )
{
    _nameValue = value;
}

unsigned int CMUPObject::getValue( ) const
{
    return _nameValue;
}

void CMUPObject::setLineNo( 
    unsigned int lineNo )
{
    _nameLineNo = lineNo;
}

unsigned int CMUPObject::getLineNo( ) const
{
    return _nameLineNo;
}

void CMUPObject::release( )
{
    delete this;
}

bool CMUPObject::verify( )
{
    assert(false);
    return _illegalFlag;
}

std::string CMUPObject::getDeclareString( ) const
{
    return _name;
}

std::string CMUPObject::getDefineString( ) const
{
    assert(false);
    return "";
}
