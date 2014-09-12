#ifndef __MUP_PRE_PARSER_H_2153__
#define __MUP_PRE_PARSER_H_2153__
#include <stdio.h>
#include <list>
#include <map>

#pragma warning(disable:4996)

typedef unsigned int FILE_LINE;

typedef std::map<FILE_LINE, FILE_LINE> CMUFileLineInfo;

class CMUPreUtil
{
protected:
    struct SLineInfo
    {
        FILE_LINE rawNo;
        FILE_LINE newNo;
    };

    typedef std::list<SLineInfo> CMUFileLineVector;

public:
    static FILE* preParse( 
        FILE* hFile, 
        CMUFileLineInfo& fileLineInfo);

protected:
    static FILE* skipComments( 
        FILE* hFile, CMUFileLineInfo& fileLineInfo);

    static size_t findCommentFlagPos( 
        char* cpDataPtr, 
        char* cpCommentFlags);
};


#endif // __MUP_PRE_PARSER_H_2153__

