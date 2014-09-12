@echo off
@title flex and bison complie

set GnuWin32Path=D:\GnuWin32\bin
set path=%path%;%GnuWin32Path%

if exist mupcompiler_lexer.c del mupcompiler_lexer.c 
if exist mupcompiler_parser.c del mupcompiler_parser.c
if exist mupcompiler_parser.h del mupcompiler_parser.h
echo make clean!

flex lexer.l
rename lex.yy.c mupcompiler_lexer.c
echo flex complied!

bison -d parser.y
rename parser.tab.c mupcompiler_parser.c
rename parser.tab.h mupcompiler_parser.h
echo bison complied!

echo compling finished!

pause
