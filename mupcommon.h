#ifndef __CMUP_COMMON_H_2258__
#define __CMUP_COMMON_H_2258__
#include <iostream>
#include <string>
#include <direct.h>
#include <list>

class CMUPReplaceTag
{
public:
    CMUPReplaceTag( 
        char srcTag, 
        char dstTag)
        : _srcTag(srcTag)
        , _dstTag(dstTag)
    {
    }

    char operator()( 
        char element) const
    {
        if (element == _srcTag)
        {
            return _dstTag;
        }

        return element;
    }

protected:
    char _srcTag;
    char _dstTag;
};

class CMUPCommon
{
public:
    static void traceInfo( 
        std::string const& type, 
        std::string const& fileName, 
        unsigned int lineNo, 
        std::string const& info );

    static bool isIdentifier( 
        std::string const& str );

    static bool isExist( 
        std::string fullPath );

    static bool makeDir( 
        std::string path );

    static bool deleteDir( 
        std::string path );

    static std::string getCurrentWorkPath( );

    static bool changeCurrentWorkpath( 
        std::string path );

    static std::string spliteFileName( 
        std::string fullPath );

    static std::string spliteFilePath( 
        std::string fullPath );

    static std::string spliteName( 
        std::string fileName );

    static std::string splitExtension( 
        std::string fileName );

    static std::string upperFirstTag( 
        std::string str );

    static bool isAbsolutePath(
        std::string path );

    static std::string verifyPath( 
        std::string path );

    static std::string relativePath2absolutePath( 
        std::string path );
};


#endif // __CMUP_COMMON_H_2258__

