
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mupcompiler.h"
#include "mupcompilerutilapi.h"

int g_nCurrentLineNo = 1;

extern int yyparse (void);
extern FILE *yyin;

void outputInvalidSyntaxInfo( 
    int lineNo, 
    char* errorInfo )
{
    outputInvalidSyntaxInfoImpl(lineNo, errorInfo);
}

int initParser(int argc, char *argv[] )
{
    return initParserImpl(argc, argv);
}

void finalParser()
{
    finalParserImpl();
}

void parseFile( )
{
    FILE* fileHandle = NULL;

    fileHandle = getNextFileImpl();
    while(fileHandle)
    {
        g_nCurrentLineNo = 1;

        yyin = fileHandle;
        yyparse();
        fclose(fileHandle);
        fileHandle = getNextFileImpl();
    }
}

void appendTag( 
    int tagID, 
    char* tagValue )
{
    appendTagImpl(tagID, tagValue, g_nCurrentLineNo);
}

void parseToken( 
    int tokenValue )
{
    switch (tokenValue)
    {
    case MUP_PACKAGE_TOKEN:
        parsePackageImpl();
        break;
    case MUP_IMPORT_TOKEN:
        parseImportImpl();
        break;
    case MUP_VAR_TOKEN:
        parseVarImpl();
        break;
    case MUP_BASE_TOKEN:
        parseBaseImpl();
        break;
    case MUP_STRUCT_MODULE_TOKEN:
        parseStructImpl();
        break;
    case MUP_CLASS_MODULE_TOKEN:
        parseClassImpl();
        break;
    case MUP_LIST_MODULE_TOKEN:
        parseListImpl();
        break;
    case MUP_VECTOR_MODULE_TOKEN:
        parseVectorImpl();
        break;
    case MUP_ASSIGN_MODULE_TOKEN:
        parseAssignImpl();
        break;
    case MUP_SCRIPT_TOKEN:
        parseScriptImpl();
        break;
    default:
        break;
    }
}

