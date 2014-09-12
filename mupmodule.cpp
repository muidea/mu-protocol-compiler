#include "mupmodule.h"
#include "mupcommon.h"
#include "mupcompilerutil.h"
#include <sstream>
#include <assert.h>

CMUPBase::CMUPBase( )
{
}

CMUPBase::CMUPBase( 
    std::string const& name, 
    unsigned int value, 
    unsigned int lineNo )
    : CMUPType(name, value, lineNo)
{
}

CMUPBase::~CMUPBase( )
{
}

/**
 ** 必须是自定义类型
 **
 ***/
bool CMUPBase::verify( )
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;
        _illegalFlag = verifyBase(this);
    }

    return _illegalFlag;
}

std::string CMUPBase::getDeclareString( ) const
{
    return CMUPType::getDeclareString();
}

std::string CMUPBase::getDefineString( ) const
{
    return CMUPType::getDefineString();
}

std::string CMUPBase::getCalcSizeString() const
{
    return getDefineString() + "::calcSize()";
}

std::string CMUPBase::getDecodeString() const
{
    return getDefineString() + "::decode(pDataPtr, uDataSize, uRemainSize)";
}

std::string CMUPBase::getEncodeString() const
{
    return getDefineString() + "::encode(pBuffPtr, uBuffSize, uRemainSize)";
}

CMUPModule::CMUPModule( )
    : _base(NULL)
{
}

CMUPModule::CMUPModule( 
    std::string const& name,
    unsigned int value,
    unsigned int lineNo )
    : CMUPObject(name, value, lineNo)
    , _base(NULL)
    ,_type(-1)
{
}

CMUPModule::~CMUPModule( )
{
}

void CMUPModule::setType( 
    unsigned int type )
{
    _type = type;
}

unsigned int CMUPModule::getType( ) const
{
    return _type;
}

void CMUPModule::setBase( 
    CMUPBase* base )
{
    _base = base;
}

CMUPBase* CMUPModule::getBase( ) const
{
    return _base;
}

void CMUPModule::linkVar( 
    CMUPVar* var )
{
    _varList.push_back(var);
}

std::list<CMUPVar*> const& CMUPModule::getVars( ) const
{
    return _varList;
}

void CMUPModule::release( )
{
    if (_base)
    {
        _base->release();
    }

    std::list<CMUPVar*>::const_iterator _Iter = _varList.begin();
    for ( ; _Iter != _varList.end(); ++_Iter)
    {
        CMUPVar* var = *_Iter;
        var->release();
    }

    CMUPObject::release();
}

bool CMUPModule::verify( )
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;

        _illegalFlag = verifyModule(this);
    }

    return _illegalFlag;
}

std::string CMUPModule::getDeclareString( ) const
{
    return declareModule(this);
}

std::string CMUPModule::structDeclare( ) const
{
    std::stringstream os;
    os << "struct " << MUP_PREFLAG << getName() << "\n";
    CMUPBase* base = getBase();
    assert(!base);

    os << "{\n";
    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
    CMUPVar* pVar = *vIter;
    CMUPType* pType = pVar->getType();
    os << "    " << pType->getDeclareString() << " " << pVar->getName() << ";\n";
    }
    os << "};\n\n";

    os << "inline UINT32 getSize(" << MUP_PREFLAG << getName() << " const& value)\n";
    os << "{\n";
    os << "    UINT32 uRet = 0;\n\n";
    vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    uRet += " << getVarCalcSizeString(pVar, "value.") << ";\n";
    }
    os << "    return uRet;\n";
    os << "};\n\n";

    os << "inline bool encode(" << MUP_PREFLAG << getName() << " const& value, void* pBuffPtr, UINT32 uBuffSize, UINT32& uRemainSize)\n";
    os << "{\n";
    os << "    bool bRet = false;\n\n";
    vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    bRet = " << getVarEncodeString(pVar, "value.") << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    os << "    return bRet;\n";
    os << "};\n\n";

    os << "inline bool decode(const void* pDataPtr, UINT32 uDataSize, " << MUP_PREFLAG << getName() << "& value, UINT32& uRemainSize)\n";
    os << "{\n";
    os << "    bool bRet = false;\n\n";
    vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    bRet = " << getVarDecodeString(pVar, "value.") << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    os << "    return bRet;\n";
    os << "};\n\n";
    return os.str();
}


std::string CMUPModule::classDeclare( ) const
{
    std::stringstream os;
    os << "class " << MUP_PREFLAG << getName();
    CMUPBase* base = getBase();
    if (base)
    {
        os << " : public " << base->getDeclareString() << "\n";
    }
    else
    {
        os << " : public IMUProtocol\n";
    }

    os << "{\n";
    os << "public:\n";
    os << "    " << MUP_PREFLAG << getName() << "();\n\n";
    if (base)
    {
        os << "    " << MUP_PREFLAG << getName() << "(" << base->getDeclareString() << " const& value);\n\n";
    }
    os << "    virtual ~" << MUP_PREFLAG << getName() << "();\n\n";
    os << "    virtual bool encode(void* pBuffPtr, UINT32 uBuffSize, UINT32& uRemainSize) const;\n\n";
    os << "    virtual bool decode(const void* pDataPtr, UINT32 uDataSize, UINT32& uRemainSize);\n\n";
    if (base)
    {
    os << "    virtual bool pureDecode(const void* pDataPtr, UINT32 uDataSize, UINT32& uRemainSize);\n\n";
    }
    os << "    virtual UINT32 calcSize() const;\n\n";

    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        CMUPType* pType = pVar->getType();
        os << "    inline void set" << CMUPCommon::upperFirstTag(pVar->getName()) << "(" << pType->getDeclareString() << " const& value)\n";
        os << "    {\n";
        os << "        _" << pVar->getName() << " = value;\n";
        os << "    };\n\n";

        os << "    inline " << pType->getDeclareString() << " const& get" << CMUPCommon::upperFirstTag(pVar->getName()) << "() const\n";
        os << "    {\n";
        os << "        return _" << pVar->getName() << ";\n";
        os << "    };\n\n";

        os << "    inline " << pType->getDeclareString() << "& peer" << CMUPCommon::upperFirstTag(pVar->getName()) << "()\n";
        os << "    {\n";
        os << "        return _" << pVar->getName() << ";\n";
        os << "    };\n\n";
    }

    os << "protected:\n";
    os << "    bool operator==(" << MUP_PREFLAG << getName() << " const& right);\n\n";
    vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* var = *vIter;
        CMUPType* type = var->getType();
        os << "    "<< type->getDeclareString() << " _" << var->getDeclareString() << ";\n";
    }

    os << "};\n\n";

    return os.str();
}


std::string CMUPModule::listDeclare( ) const
{
    assert(1 == _varList.size());

    std::stringstream os;
    CMUPVar* pVar = _varList.front();
    CMUPType* pType = pVar->getType();

    os << "typedef std::list<" << pType->getDeclareString() << "> " << MUP_PREFLAG << getName() << ";\n\n";

    return os.str();
}

std::string CMUPModule::vectorDeclare( ) const
{
    assert(1 == _varList.size());

    std::stringstream os;
    CMUPVar* pVar = _varList.front();
    CMUPType* pType = pVar->getType();

    os << "typedef std::vector<" << pType->getDeclareString() << "> " << MUP_PREFLAG << getName() << ";\n\n";

    return os.str();
}


std::string CMUPModule::assignDeclare( ) const
{
    assert(1 == _varList.size());

    std::stringstream os;

    CMUPVar* pVar = _varList.front();
    CMUPType* pType = pVar->getType();
    os << "typedef " << pType->getDeclareString() << " " << MUP_PREFLAG << getName() << ";\n\n";

    return os.str();
}

std::string CMUPModule::getDefineString( ) const
{
    return defineModule(this);
}

std::string CMUPModule::classDefine( ) const
{
    CMUPBase* base = getBase();
    std::stringstream os;
    os << classConstructDefine();
    if (base)
    {
    os << classAssignConstructDefine();
    }
    os << classDestructDefine();
    os << classEncodeDefine();
    os << classDecodeDefine();
    if (base)
    {
        os << classPureDecodeDefine();
    }
    os << classCalcSizeDefine();

    return os.str();
}

std::string CMUPModule::classConstructDefine( ) const
{
    std::stringstream os;
    os << MUP_PREFLAG << getName() << "::" << MUP_PREFLAG << getName() << "()\n";
    os << "{\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classAssignConstructDefine() const
{
    CMUPBase* base = getBase();
    std::stringstream os;
    os << MUP_PREFLAG << getName() << "::" << MUP_PREFLAG << getName() << "("<< base->getDeclareString() << " const& value)\n";
    os << "\t:" << base->getDeclareString() << "(value)\n";
    os << "{\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classDestructDefine( ) const
{
    std::stringstream os;
    os << MUP_PREFLAG << getName() << "::~" << MUP_PREFLAG << getName() << "()\n";
    os << "{\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classEncodeDefine( ) const
{
    std::stringstream os;
    os << "bool " << MUP_PREFLAG << getName() << "::encode(void* pBuffPtr, UINT32 uBuffSize, UINT32& uRemainSize) const\n";
    os << "{\n";
    os << "    bool bRet = true;\n";

    CMUPBase* base = getBase();
    if (NULL != base)
    {
        os << "    bRet = " << base->getEncodeString() << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    else
    {
        os << "    uRemainSize = uBuffSize;\n\n";
    }

    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    " << pVar->getEncodeString() << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    os << "    return bRet;\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classDecodeDefine( ) const
{
    std::stringstream os;
    os << "bool " << MUP_PREFLAG << getName() << "::decode(const void* pDataPtr, UINT32 uDataSize, UINT32& uRemainSize)\n";
    os << "{\n";
    os << "    bool bRet = true;\n";
    CMUPBase* base = getBase();
    if (NULL != base)
    {
    os << "    bRet = " << base->getDecodeString() << ";\n";
    os << "    if (!bRet) {\n";
    os << "        return bRet;\n";
    os << "    }\n\n";
    }
    else
    {
    os << "    uRemainSize = uDataSize;\n\n";
    }

    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    " << pVar->getDecodeString() << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    os << "    return bRet;\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classPureDecodeDefine() const
{
    std::stringstream os;
    os << "bool " << MUP_PREFLAG << getName() << "::pureDecode(const void* pDataPtr, UINT32 uDataSize, UINT32& uRemainSize)\n";
    os << "{\n";
    os << "    bool bRet = true;\n";
    os << "    uRemainSize = uDataSize;\n\n";

    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    " << pVar->getDecodeString() << ";\n";
        os << "    if (!bRet) {\n";
        os << "        return bRet;\n";
        os << "    }\n\n";
    }
    os << "    return bRet;\n";
    os << "}\n\n";

    return os.str();
}

std::string CMUPModule::classCalcSizeDefine( ) const
{
    std::stringstream os;
    os << "UINT32 " << MUP_PREFLAG << getName() << "::calcSize() const\n";
    os << "{\n";
    os << "    UINT32 uRet = 0;\n";
    CMUPBase* base = getBase();
    if (NULL != base)
    {
        os << "    uRet += " << base->getCalcSizeString() << ";\n\n";
    }

    std::list<CMUPVar*>::const_iterator vIter = _varList.begin();
    for ( ; vIter != _varList.end(); ++vIter)
    {
        CMUPVar* pVar = *vIter;
        os << "    uRet += " << pVar->getCalcSizeString() << ";\n";
    }
    os << "\n";
    os << "    return uRet;\n";
    os << "}\n\n";

    return os.str();
}

