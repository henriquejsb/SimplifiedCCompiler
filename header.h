#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <float.h>


typedef struct node
{
	struct node* children;
    struct node* brother;
    char * nodeType;
    char * value;
	char * annotate;
	int nline;
	int ncol;
}node;


typedef struct token
{
	char * value;
	int nline;
	int ncol;

}token;




typedef struct _param_list{
	char * type;
	char * id;
	int var;
	struct _param_list * next;
}param_list;

typedef struct _var_list{
	int function; //Indica se e funcao
	
	char * id;
	char * type;
	int nparameters;
	param_list * parameters;
	struct _var_list * next;
}var_list;

typedef struct _sym_table{
	int function; //TRUE -> Function table FALSE -> GLOBAL
	int defined; //Se ja foi definida ou nao
	char * type;
	char * id;
	int nparameters;
	var_list * variables;
	param_list * parameters;
	struct _sym_table * next;
}sym_table;


extern sym_table * global_table;
extern sym_table * functions_table;

int semanticErrors;

node* createnode(char* nodeType,char* value, int nline, int ncol);
token * createtoken(char * value, int nline, int ncol);
void addBro(node* old, node* new);
void addChild(node* father, node* aux);
void printTree(node* atual,int nivel);
int countBros(node * atual);
void clearTree(node * atual);

sym_table * create_table(int function);


var_list * create_var_list(int function);

param_list * create_param_list();
char * get_type(var_list * var);
void add_sym_table(sym_table ** table , sym_table * new);
void add_var_list(var_list ** variables, var_list * new);
sym_table * search_function(char * function_name);
char * search_variable(sym_table * table, char * variable_name);
void add_param_list(param_list ** parameters, param_list * new);
void create_semantic_table(node * root);

void print_global_table(sym_table *table);

void print_functions_table(sym_table *table);

var_list * search_var(sym_table * table, char * name);
param_list * search_param(param_list * table, char * name);
void annotate_AST(node * current, sym_table * current_function, int expression);




void generate_LLVM(node * root);
void print_globals();
char * convert_type(char * type);
void func_declaration(node * current);
void add(node * current, int var1, int var2);
void sub(node * current, int var1, int var2);
void mul(node * current, int var1, int var2);
void div_func(node * current, int var1, int var2);
void mod(node * current, int var1, int var2);
void minus(node * current);
void plus(node * current);
void eq(node * current, int var1, int var2);
void ne(node * current, int var1, int var2);
void lt(node * current, int var1, int var2);
void gt(node * current, int var1, int var2);
void le(node * current, int var1, int var2);
void ge(node * current, int var1, int var2);
void bitwiseand(node * current, int var1, int var2);
void bitwiseor(node * current, int var1, int var2);
void bitwisexor(node * current, int var1, int var2);
int check_global(char * var);
void store(node * current, int var1);
void reallit(node * current);
void intlit(node * current);
void chrlit(node * current);
void declaration(node * current);
void id(node * current);
void func_definition(node * current);
void convert_LLVM(node * current);

void clear_tables(sym_table * table);
void clear_var(var_list * var);
void clear_param(param_list * param);
