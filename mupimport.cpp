#include "mupimport.h"
#include "mupcommon.h"
#include "mupcompilerutil.h"
#include <sstream>
#include <assert.h>

CMUPImport::CMUPImport( )
{
}

CMUPImport::CMUPImport( 
    std::string const& name,
    unsigned int value,
    unsigned int lineNo )
    : CMUPObject(name, value, lineNo)
{
}

CMUPImport::~CMUPImport( )
{

}

void CMUPImport::linkPackage( 
    CMUPPackage* package )
{
    _packageList.push_back(package);
}

std::list<CMUPPackage*> const& CMUPImport::getPackages( ) const
{
    return _packageList;
}

void CMUPImport::getImportFile( 
    std::string& fileName )
{
    if (_packageList.empty())
    {
        return ;
    }

    std::list<CMUPPackage*>::const_iterator currentIter = _packageList.begin();
    std::list<CMUPPackage*>::const_iterator nextIter;
    for ( ; currentIter != _packageList.end(); ++currentIter )
    {
        CMUPPackage* package = *currentIter;
        fileName += package->getName();

        nextIter = currentIter;
        if (++nextIter != _packageList.end())
        {
            fileName += "\\";
        }
    }

    fileName += ".mps";
}

void CMUPImport::release( )
{
    std::list<CMUPPackage*>::const_iterator currentIter = _packageList.begin();
    for ( ; currentIter != _packageList.end(); ++currentIter )
    {
        CMUPPackage* package = *currentIter;
        package->release();
    }
    _packageList.clear();

    CMUPPackage::release();
}

bool CMUPImport::verify( )
{
    if (!_verifiedFlag)
    {
        _verifiedFlag = true;
        _illegalFlag = verifyImport(this);
    }

    return _illegalFlag;
}

std::string CMUPImport::getDeclareString( ) const
{
    return declareImport(this);
}

std::string CMUPImport::getDefineString( ) const
{
    return defineImport(this);
}
