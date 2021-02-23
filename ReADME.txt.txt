To run:

lex uccompiler.l && yacc -d uccompiler.y && cc -g -Wall -o uccompiler y.tab.c  AST.c SEMANTIC.c CODE_GEN.c header.h lex.yy.c -ll -ly 

then

./uccompiler file.c

Does not support arrays, pointers, etc. Simple variable arithmetics, if/else, for, while,function declaration and definition.