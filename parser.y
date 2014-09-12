%{
#include <stdlib.h>
#include <stdio.h>
#include "mupcompiler.h"

#pragma warning(disable:4273)

extern int yylex();
void yyerror(char *s);
%}

%token ASM AUTO
%token BOOL BREAK
%token CASE CATCH CHAR CLASS CONST CONST_CAST CONTINUE
%token DEFAULT DELETE DO DOUBLE DYNAMIC_CAST
%token ELSE ENUM EXPLICIT EXPORT EXTERN
%token FALSE FLOAT FOR FRIEND
%token GOTO
%token IF INLINE INT
%token LONG
%token MUTABLE
%token NAMESPACE NEW
%token OPERATOR 
%token PRIVATE PROTECTED PUBLIC
%token REGISTER REINTERPRET_CAST RETURN
%token SHORT SIGNED SIZEOF STATIC STATIC_CAST STRUCT SWITCH
%token TEMPLATE THIS THROW TRUE TRY TYPEDEF TYPEID TYPENAME
%token UNION UNSIGNED USING
%token VIRTUAL VOID VOLATILE
%token WCHAR_T WHILE

%token MUDOT
%token MUIMPORT
%token MUIDENTIFIER
%token LBRACE RBRACE SEMICOLON MUASSIGEN MUBASE
%token MUSTRUCT MUCLASS MULIST MUVECTOR MUSTRING MUUINT MUULONG MUINT8 MUINT16 MUINT32 MUINT64 MUUINT8 MUUINT16 MUUINT32 MUUINT64 MUFLOAT32 MUFLOAT64
%token MUCUSTOMER_NODE_ID

%start mupparser_specific_start
%%
mupparser_type
		: INT
		| LONG
		| FLOAT
		| DOUBLE
		| BOOL
		| CHAR
		| MUINT8
		| MUINT16
		| MUINT32
		| MUINT64
		| MUUINT
		| MUUINT8
		| MUUINT16
		| MUUINT32
		| MUUINT64
		| MUFLOAT32
		| MUFLOAT64
		| MUULONG
		| MUSTRING
		| MUIDENTIFIER
		;

mupparser_package_list
		: mupparser_package
		| mupparser_package_list mupparser_package
		;

mupparser_package
		: MUIDENTIFIER MUDOT { parseToken(MUP_PACKAGE_TOKEN); }
		;

mupparser_import_list
		: mupparser_import
		| mupparser_import_list mupparser_import
		;

mupparser_import
		: MUIMPORT mupparser_package_list MUIDENTIFIER SEMICOLON { parseToken(MUP_IMPORT_TOKEN); }
		;

mupparser_struct_list
		: mupparser_struct
		| mupparser_struct_list mupparser_struct
		;

mupparser_struct
		: MUSTRUCT MUIDENTIFIER LBRACE mupparser_var_list RBRACE SEMICOLON { parseToken(MUP_STRUCT_MODULE_TOKEN); }
		| MUCLASS MUIDENTIFIER mupparser_base_struct LBRACE mupparser_var_list RBRACE SEMICOLON { parseToken(MUP_CLASS_MODULE_TOKEN); }
		| MULIST MUIDENTIFIER MUASSIGEN mupparser_type SEMICOLON { parseToken(MUP_LIST_MODULE_TOKEN); }
		| MUVECTOR MUIDENTIFIER MUASSIGEN mupparser_type SEMICOLON { parseToken(MUP_VECTOR_MODULE_TOKEN); }
		| MUIDENTIFIER MUASSIGEN mupparser_type SEMICOLON { parseToken(MUP_ASSIGN_MODULE_TOKEN); }
		;

mupparser_base_struct
		: MUBASE MUIDENTIFIER { parseToken(MUP_BASE_TOKEN); }
		| 
		;

mupparser_var_list
		: mupparser_var
		| mupparser_var_list mupparser_var
		;

mupparser_var
		: mupparser_type MUIDENTIFIER SEMICOLON { parseToken(MUP_VAR_TOKEN); }
		;

mupparser_body
		: mupparser_struct_list { parseToken(MUP_SCRIPT_TOKEN); }
		| mupparser_import_list mupparser_struct_list { parseToken(MUP_SCRIPT_TOKEN); }

mupparser_specific_start
		: mupparser_body
		;
%%

void yyerror(char *s) 
{ 
		outputInvalidSyntaxInfo(g_nCurrentLineNo, s); 
}
