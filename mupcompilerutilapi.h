#ifndef __MUPPARSER_UTIL_API_H_2242__
#define __MUPPARSER_UTIL_API_H_2242__
#include <stdio.h>

void outputInvalidSyntaxInfoImpl( 
    int nLineNo, 
    char* cpErrInfo);

int initParserImpl( 
    int argc, 
    const char *argv[]);

FILE* getNextFileImpl();

void finalParserImpl();

void appendTagImpl( 
    int iTagID, 
    char* cpTagValue, 
    int iLineNo);

void parsePackageImpl();

void parseImportImpl();

void parseBaseImpl();

void parseVarImpl();

void parseStructImpl();

void parseClassImpl();

void parseListImpl();

void parseVectorImpl();

void parseAssignImpl();

void parseScriptImpl();

#endif // __MUPPARSER_UTIL_API_H_2242__

