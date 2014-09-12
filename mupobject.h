#ifndef __CMUP_OBJECT_H_2329__
#define __CMUP_OBJECT_H_2329__
#include "mupcompiler_parser.h"
#include "mupdeclarestream.h"
#include "mupdefinestream.h"
#include <string>
#include <iostream>
#include <list>

#define MUP_PREFLAG "CMU"

class CMUPObject
{
public:
    CMUPObject( );

    CMUPObject( 
        std::string const& name, 
        unsigned int value, 
        unsigned int lineNo );

    virtual ~CMUPObject( );

    void setName( 
        std::string const& name );

    std::string const& getName( ) const;

    void setValue( 
        unsigned int value );

    unsigned int getValue( ) const;

    void setLineNo( 
        unsigned int lineNo );

    unsigned int getLineNo( ) const;

    virtual void release( );

    virtual bool verify( );

    virtual std::string getDeclareString( ) const;

    virtual std::string getDefineString( ) const;

protected:
    CMUPObject& operator=( 
        CMUPObject const& right );

    std::string _name;
    unsigned int _nameValue;
    unsigned int _nameLineNo;

    bool _illegalFlag;
    bool _verifiedFlag;
};

#endif // __CMUP_OBJECT_H_2329__
