%{
#include <stdlib.h>
#include <stdio.h>
#include "mupcompiler_parser.h"
#include "mupcompiler.h"

static int yyAnalyse(int nTokenID);
static int yywrap(void);
%}

%%
[\n]							{ g_nCurrentLineNo++; }
"asm"							{ return yyAnalyse(ASM); }
"auto"							{ return yyAnalyse(AUTO); }
"bool"							{ return yyAnalyse(BOOL); }
"break"							{ return yyAnalyse(BREAK); }
"case"							{ return yyAnalyse(CASE); }
"catch"							{ return yyAnalyse(CATCH); }
"char"							{ return yyAnalyse(CHAR); }
"class"							{ return yyAnalyse(CLASS); }
"const"							{ return yyAnalyse(CONST); }
"const_cast"					{ return yyAnalyse(CONST_CAST); }
"continue"						{ return yyAnalyse(CONTINUE); }
"default"						{ return yyAnalyse(DEFAULT); }
"delete"						{ return yyAnalyse(DELETE); }
"do"							{ return yyAnalyse(DO); }
"double"						{ return yyAnalyse(DOUBLE); }
"dynamic_cast"					{ return yyAnalyse(DYNAMIC_CAST); }
"else"							{ return yyAnalyse(ELSE); }
"enum"							{ return yyAnalyse(ENUM); }
"explicit"						{ return yyAnalyse(EXPLICIT); }
"export"						{ return yyAnalyse(EXPORT); }
"extern"						{ return yyAnalyse(EXTERN); }
"false"							{ return yyAnalyse(FALSE); }
"float"							{ return yyAnalyse(FLOAT); }
"for"							{ return yyAnalyse(FOR); }
"friend"						{ return yyAnalyse(FRIEND); }
"goto"							{ return yyAnalyse(GOTO); }
"if"							{ return yyAnalyse(IF); }
"inline"						{ return yyAnalyse(INLINE); }
"int"							{ return yyAnalyse(INT); }
"long"							{ return yyAnalyse(LONG); }
"mutable"						{ return yyAnalyse(MUTABLE); }
"namespace"						{ return yyAnalyse(NAMESPACE); }
"new"							{ return yyAnalyse(NEW); }
"operator"						{ return yyAnalyse(OPERATOR); }
"private"						{ return yyAnalyse(PRIVATE); }
"protected"						{ return yyAnalyse(PROTECTED); }
"public"						{ return yyAnalyse(PUBLIC); }
"register"						{ return yyAnalyse(REGISTER); }
"reinterpret_cast"				{ return yyAnalyse(REINTERPRET_CAST); }
"return"						{ return yyAnalyse(RETURN); }
"short"							{ return yyAnalyse(SHORT); }
"signed"						{ return yyAnalyse(SIGNED); }
"sizeof"						{ return yyAnalyse(SIZEOF); }
"static"						{ return yyAnalyse(STATIC); }
"static_cast"					{ return yyAnalyse(STATIC_CAST); }
"struct"						{ return yyAnalyse(STRUCT); }
"switch"						{ return yyAnalyse(SWITCH); }
"template"						{ return yyAnalyse(TEMPLATE); }
"this"							{ return yyAnalyse(THIS); }
"throw"							{ return yyAnalyse(THROW); }
"true"							{ return yyAnalyse(TRUE); }
"try"							{ return yyAnalyse(TRY); }
"typedef"						{ return yyAnalyse(TYPEDEF); }
"typeid"						{ return yyAnalyse(TYPEID); }
"typename"						{ return yyAnalyse(TYPENAME); }
"union"							{ return yyAnalyse(UNION); }
"unsigned"						{ return yyAnalyse(UNSIGNED); }
"using"							{ return yyAnalyse(USING); }
"virtual"						{ return yyAnalyse(VIRTUAL); }
"void"							{ return yyAnalyse(VOID); }
"volatile"						{ return yyAnalyse(VOLATILE); }
"wchar_t"						{ return yyAnalyse(WCHAR_T); }
"while"							{ return yyAnalyse(WHILE); }

"{"								{ return yyAnalyse(LBRACE); }
"}"								{ return yyAnalyse(RBRACE); }
";"								{ return yyAnalyse(SEMICOLON); }
"import"						{ return yyAnalyse(MUIMPORT); }	
"string"						{ return yyAnalyse(MUSTRING); }
"mustruct"						{ return yyAnalyse(MUSTRUCT); }
"muclass"						{ return yyAnalyse(MUCLASS); }
"mulist"						{ return yyAnalyse(MULIST); }
"muvector"						{ return yyAnalyse(MUVECTOR); }
"int8"							{ return yyAnalyse(MUINT8); }
"int16"							{ return yyAnalyse(MUINT16); }
"int32"							{ return yyAnalyse(MUINT32); }
"int64"							{ return yyAnalyse(MUINT64); }
"uint"							{ return yyAnalyse(MUUINT); }
"uint8"							{ return yyAnalyse(MUUINT8); }
"uint16"						{ return yyAnalyse(MUUINT16); }
"uint32"						{ return yyAnalyse(MUUINT32); }
"uint64"						{ return yyAnalyse(MUUINT64); }
"float32"						{ return yyAnalyse(MUFLOAT32); }
"float64"						{ return yyAnalyse(MUFLOAT64); }
"ulong"							{ return yyAnalyse(MUULONG); }
"="								{ return yyAnalyse(MUASSIGEN); }
":"								{ return yyAnalyse(MUBASE); }
[.]								{ return yyAnalyse(MUDOT); }
[a-zA-Z_][a-zA-Z0-9_]*			{ return yyAnalyse(MUIDENTIFIER); }

%%

static int yyAnalyse(int nTagID)
{
	appendTag(nTagID,yytext);
	
	return nTagID;
}

static int yywrap(void)
{
	return(1);
}

int main( int argc, char **argv )
{
	do {
		if (0 != initParser(argc, argv)) {
			break;
		}

		parseFile();

		finalParser();

	} while(0);

	return 0;
}

