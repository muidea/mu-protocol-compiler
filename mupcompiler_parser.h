
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ASM = 258,
     AUTO = 259,
     BOOL = 260,
     BREAK = 261,
     CASE = 262,
     CATCH = 263,
     CHAR = 264,
     CLASS = 265,
     CONST = 266,
     CONST_CAST = 267,
     CONTINUE = 268,
     DEFAULT = 269,
     DELETE = 270,
     DO = 271,
     DOUBLE = 272,
     DYNAMIC_CAST = 273,
     ELSE = 274,
     ENUM = 275,
     EXPLICIT = 276,
     EXPORT = 277,
     EXTERN = 278,
     FALSE = 279,
     FLOAT = 280,
     FOR = 281,
     FRIEND = 282,
     GOTO = 283,
     IF = 284,
     INLINE = 285,
     INT = 286,
     LONG = 287,
     MUTABLE = 288,
     NAMESPACE = 289,
     NEW = 290,
     OPERATOR = 291,
     PRIVATE = 292,
     PROTECTED = 293,
     PUBLIC = 294,
     REGISTER = 295,
     REINTERPRET_CAST = 296,
     RETURN = 297,
     SHORT = 298,
     SIGNED = 299,
     SIZEOF = 300,
     STATIC = 301,
     STATIC_CAST = 302,
     STRUCT = 303,
     SWITCH = 304,
     TEMPLATE = 305,
     THIS = 306,
     THROW = 307,
     TRUE = 308,
     TRY = 309,
     TYPEDEF = 310,
     TYPEID = 311,
     TYPENAME = 312,
     UNION = 313,
     UNSIGNED = 314,
     USING = 315,
     VIRTUAL = 316,
     VOID = 317,
     VOLATILE = 318,
     WCHAR_T = 319,
     WHILE = 320,
     MUDOT = 321,
     MUIMPORT = 322,
     MUIDENTIFIER = 323,
     LBRACE = 324,
     RBRACE = 325,
     SEMICOLON = 326,
     MUASSIGEN = 327,
     MUBASE = 328,
     MUSTRUCT = 329,
     MUCLASS = 330,
     MULIST = 331,
     MUVECTOR = 332,
     MUSTRING = 333,
     MUUINT = 334,
     MUULONG = 335,
     MUINT8 = 336,
     MUINT16 = 337,
     MUINT32 = 338,
     MUINT64 = 339,
     MUUINT8 = 340,
     MUUINT16 = 341,
     MUUINT32 = 342,
     MUUINT64 = 343,
     MUFLOAT32 = 344,
     MUFLOAT64 = 345,
     MUCUSTOMER_NODE_ID = 346
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


