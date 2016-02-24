#include "mupprecompiler.h"

FILE* CMUPreUtil::preParse( 
    FILE* hFile, 
    CMUFileLineInfo& fileLineInfo)
{
    FILE* hRet = NULL;
    if (NULL == hFile)
    {
        return hRet;
    }

    fileLineInfo.clear();

    hRet = skipComments(hFile, fileLineInfo);
    if (NULL == hRet)
    {
        return hRet;
    }

    return hRet;
}

FILE* CMUPreUtil::skipComments( 
    FILE* hFile, 
    CMUFileLineInfo& fileLineInfo)
{
    FILE* hRet = NULL;
    if (NULL == hFile) {
        return hRet;
    }

    hRet = ::fopen("skipcomments", "a+tD");
    if (NULL == hRet) {
        return hRet;
    }

    FILE_LINE rawNo = 0;
    FILE_LINE newNo = 0;
    char cpBuff[1024] = {0};
    size_t iPos = 0;
    while(1) {
        ::memset(cpBuff, 0, 1024);
        if (NULL == ::fgets(cpBuff,1024,hFile)) {
            break;
        }

        rawNo++;
        iPos = findCommentFlagPos(cpBuff,"#");
        if (iPos > 0) {
            newNo++;
            if (cpBuff[iPos-1] != '\n')
            {
                cpBuff[iPos] = '\n';
                ::fwrite(cpBuff,sizeof(char),iPos+1,hRet);
            }
            else
            {
                ::fwrite(cpBuff,sizeof(char),iPos,hRet);
            }

            fileLineInfo[newNo] = rawNo;
        }
    }

    ::fseek(hRet,0,SEEK_SET);
    ::fclose(hFile);

    return hRet;
}

size_t CMUPreUtil::findCommentFlagPos( 
    char* cpDataPtr, 
    char* cpCommentFlags)
{
    char* cpPosPtr = ::strstr(cpDataPtr,cpCommentFlags);
    if (NULL == cpPosPtr) {
        return ::strlen(cpDataPtr);
    }

    return cpPosPtr - cpDataPtr;
}
