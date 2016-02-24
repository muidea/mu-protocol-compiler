
#ifndef _MPPARSER_H_2048_
#define _MPPARSER_H_2048_

enum MUP_TOKEN_TYPE
{
    MUPS_INVALID = -1,
    MUP_PACKAGE_TOKEN,
    MUP_IMPORT_TOKEN,
    MUP_VAR_TOKEN,
    MUP_BASE_TOKEN,
    MUP_STRUCT_MODULE_TOKEN,
    MUP_CLASS_MODULE_TOKEN,
    MUP_LIST_MODULE_TOKEN,
    MUP_VECTOR_MODULE_TOKEN,
    MUP_ASSIGN_MODULE_TOKEN,
    MUP_SCRIPT_TOKEN
};

extern int g_nCurrentLineNo;

void outputInvalidSyntaxInfo(int lineNo, char* errorInfo);

int initParser(int argc, char *argv[] );

void parseFile();

void finalParser();

void appendTag(int tagID,char* tagValue);

void parseToken(int tokenValue);

#endif
