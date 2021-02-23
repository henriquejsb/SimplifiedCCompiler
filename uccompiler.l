%{
	//AUTORES:
	//FILIPE GOOD
	//HENRIQUE BRANQUINHO
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "y.tab.h"
	#include "header.h"
	
	sym_table * global_table;
	sym_table * functions_table;				
	
%}

newline				[\n\r]
space				[ \f]

int				"int"
char				"char"
double 				"double"
short				"short"
void 				"void"
return 				"return"
while				"while"
if 				"if"
else				"else"

id				[a-zA-Z\_][a-zA-Z\_0-9]*


plus				\+
mul				\*
minus				\-
and				\&\&
div 				\/
mod				\%
or				\|\|

eq				\=\=
ne				\!\=
lt 				\<
gt				\>
le 				\<\=
ge 				\>\=

lbrace				\{
rbrace				\}
lpar				\(
rpar				\)
bitwiseand			\&
bitwiseor			\|
bitwisexor			\^
assign				\=
comma				\,
semi				\;
not 				\!



onelinecomm             	\/\/


intlit 				[0-9]+

reallit			([0-9]+\.[0-9]*([eE][\+\-]?[0-9]+)?)|(\.[0-9]+([eE][\+\-]?[0-9]+)?)|([0-9]+([eE][\+\-]?[0-9]+))


chrlitaux			([^\n'\\])|(\\('|\\|n|t|\"|([0-7]{1,3})))

increment 			\+\+
decrement 			\-\-
lbrack 				\[
rbrack				\]
auto 				auto
break 				break
case 				case
const				const
continue 			continue
default 			default
do 				do
enum 				enum
extern 				extern
for 				for
float				float
goto 				goto
inline 				inline
register 			register
long				long
restrict 			restrict
signed 				signed
sizeof 				sizeof
static 				static
struct 				struct
switch 				switch
typedef 			typedef
union 				union
unsigned 			unsigned
volatile 			volatile
bool 				\_Bool
complex 			\_Complex
imaginary 			\_Imaginary

reserved {increment}|{decrement}|{lbrack}|{rbrack}|{auto}|{break}|{case}|{const}|{continue}|{default}|{do}|{enum}|{extern}|{for}|{float}|{goto}|{inline}|{register}|{long}|{restrict}|{signed}|{sizeof}|{static}|{struct}|{switch}|{typedef}|{union}|{unsigned}|{volatile}|{bool}|{complex}|{imaginary}





%X MULTILINECOMM 
%{
int flagTree=0;
int flagLex = 0;
int flagSem = 0;
int nlinha = 1;
int flagSyntax = 0;
int ncol = 1;
int nlinhacom = 0;
int ncolcom = 0;
extern node* root;
extern int nerros;
%}
%%
{reserved} 	{if(flagLex == 1){
			 printf("RESERVED(%s)\n",yytext);}
		ncol += yyleng;
		if(flagLex == 0) return RESERVED;
		}

{int}       	{if(flagLex == 1) printf("INT\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return INT;}

{char}   	{if(flagLex == 1) printf("CHAR\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return CHAR;}

{double}  	{if(flagLex == 1) printf("DOUBLE\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return DOUBLE;}

{short} 	{if(flagLex == 1) printf("SHORT\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return SHORT;}

{while}		{if(flagLex == 1) printf("WHILE\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return WHILE;}

{if} 		{if(flagLex == 1) printf("IF\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return IF;}

{else}		{if(flagLex == 1) printf("ELSE\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return ELSE;}

{void}		{if(flagLex == 1) printf("VOID\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return VOID;}

{return}	{if(flagLex == 1) printf("RETURN\n");
		if(flagLex == 0) yylval.tok = createtoken(NULL,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return RETURN;}

{id}		{if(flagLex == 1) printf("ID(%s)\n",yytext);
		if(flagLex == 0) yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0){ 
			
			//yylval.value = strdup(yytext);			
			return ID;}}

{intlit} 	{if(flagLex == 1) printf("INTLIT(%s)\n",yytext);
		if(flagLex == 0) yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0){ 
			//yylval.value = strdup(yytext);			
			return INTLIT;}}
			
'{chrlitaux}'	{if(flagLex == 1) printf("CHRLIT(%s)\n",yytext); 
		
		if(flagLex == 0) yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0){ 
			//yylval.value = strdup(yytext);			
			return CHRLIT;}}

'([^\n'\\]|(\\.))*'		{printf("Line %d, col %d: invalid char constant (%s)\n",nlinha,ncol,yytext);
				ncol += yyleng;
				}

'([^\n'\\]|(\\.))*\\?"\n"	{printf("Line %d, col %d: unterminated char constant\n",nlinha,ncol); nlinha += 1; ncol = 1;}




{reallit} 	{if(flagLex == 1) printf("REALLIT(%s)\n",yytext);
		
		if(flagLex == 0) yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0){ 
			//yylval.value = strdup(yytext);			
			return REALLIT;}}

{plus}		{if(flagLex == 1) printf("PLUS\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);		
		ncol += yyleng;
		
		if(flagLex == 0) return PLUS;}

{minus}		{if(flagLex == 1) printf("MINUS\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return MINUS;}

{mul}		{if(flagLex == 1) printf("MUL\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return MUL;}

{div}		{if(flagLex == 1) printf("DIV\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return DIV;}

{mod}		{if(flagLex == 1) printf("MOD\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return MOD;}

{and}		{if(flagLex == 1) printf("AND\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return AND;}

{or}		{if(flagLex == 1) printf("OR\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return OR;}

{eq}		{if(flagLex == 1) printf("EQ\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return EQ;}
	
{ne}		{if(flagLex == 1) printf("NE\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return NE;}

{lt}		{if(flagLex == 1) printf("LT\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return LT;}

{gt} 		{if(flagLex == 1) printf("GT\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return GT;}

{le}		{if(flagLex == 1) printf("LE\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return LE;}

{ge}		{if(flagLex == 1) printf("GE\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return GE;}

{lbrace}	{if(flagLex == 1) printf("LBRACE\n");
		ncol += yyleng;
		if(flagLex == 0) return LBRACE;}

{rbrace}	{if(flagLex == 1) printf("RBRACE\n");
		ncol += yyleng;
		if(flagLex == 0) return RBRACE;}

{lpar}		{if(flagLex == 1) printf("LPAR\n");
		ncol += yyleng;
		if(flagLex == 0) return LPAR;}

{rpar}		{if(flagLex == 1) printf("RPAR\n");
		ncol += yyleng;
		if(flagLex == 0) return RPAR;}


{assign}	{if(flagLex == 1) printf("ASSIGN\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return ASSIGN;}

{comma}		{if(flagLex == 1) printf("COMMA\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return COMMA;}

{semi}		{if(flagLex == 1) printf("SEMI\n");
		ncol += yyleng;
		if(flagLex == 0) return SEMI;}



{bitwiseand} 	{if(flagLex == 1) printf("BITWISEAND\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return BITWISEAND;}

{bitwiseor}	{if(flagLex == 1) printf("BITWISEOR\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return BITWISEOR;}

{bitwisexor}	{if(flagLex == 1) printf("BITWISEXOR\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return BITWISEXOR;}

{not} 		{if(flagLex == 1) printf("NOT\n");
		if(flagLex == 0)yylval.tok = createtoken(yytext,nlinha,ncol);
		ncol += yyleng;
		if(flagLex == 0) return NOT;}



{onelinecomm}.*   	{ncol += yyleng;}   


"/*" 		{ BEGIN MULTILINECOMM;nlinhacom = nlinha; ncolcom = ncol;
		ncol += yyleng;}

 





<MULTILINECOMM><<EOF>> 		{printf("Line %d, col %d: unterminated comment\n",nlinhacom,ncolcom);
				ncol += 1;
				yyterminate();}

<MULTILINECOMM>"*/" 		{ BEGIN 0;
				ncol += yyleng;}

<MULTILINECOMM>{newline}	{nlinha += 1;
				ncol = 1;}

<MULTILINECOMM>"\t"		{ncol += yyleng;}

<MULTILINECOMM>.		{ncol += yyleng;}


{space}		{ncol += yyleng;}

{newline}	{nlinha += 1;
		ncol = 1;}

\t		{ncol += yyleng;}

<<EOF>>		{ncol += 1; yyterminate();}



.		{printf("Line %d, col %d: illegal character (%s)\n",nlinha,ncol,yytext);
		ncol += yyleng;}

%%

int main(int argc, char * argv[]){
	if(argc == 2){
		if(strcmp(argv[1],"-l") == 0){
			flagLex = 1;
			yylex();
		}else if(strcmp(argv[1],"-1") == 0){
			flagLex = 0;
			yylex();
		}else if(strcmp(argv[1],"-t")==0){
			flagTree =1;
		}else if(strcmp(argv[1],"-2")==0){
			flagSyntax = 1;
		}else if(strcmp(argv[1],"-s")==0){
			flagSem = 1;
		}else if(strcmp(argv[1],"-3")==0){
			flagSem = 2;
		}	
	}
	
	if(argc < 2){
		flagLex = 0;
		 yyparse();
		semanticErrors = 0;
		create_semantic_table(root);
		if(semanticErrors == 0) generate_LLVM(root);
		if(root != NULL) clearTree(root); root = NULL;
		clear_tables(global_table);
			clear_tables(functions_table);
		
	}
	
	if(flagTree==1){
		yyparse();
		
		if(nerros == 0) printTree(root,0);
		if(root != NULL) clearTree(root); root = NULL;
	}
	if(flagSyntax == 1){
		yyparse();
		if(root != NULL) clearTree(root); root = NULL;
	}
	if(flagSem == 1){
		yyparse();
		global_table = NULL;
		functions_table = NULL;
		if(nerros == 0){
			create_semantic_table(root);
			if(semanticErrors >= 0){
				print_global_table(global_table);
				print_functions_table(functions_table);
				printTree(root,0);
			}
			clear_tables(global_table);
			clear_tables(functions_table);
			
		}
		if(root != NULL) clearTree(root); root = NULL;
		
	}
	if(flagSem == 2){
		yyparse();
		global_table = NULL;
		functions_table = NULL;
		create_semantic_table(root);
		clear_tables(global_table);
			clear_tables(functions_table);
		if(root != NULL) clearTree(root); root = NULL;	

	}
	
	yylex_destroy();
	
	return 0;
}

int yywrap(){
	return 1;
}
 void yyerror (const char *s){
	nerros += 1;
	
	printf("Line %d, col %d: %s: %s\n",nlinha,(int)ncol - yyleng,s,yytext);
}


