
#include "header.h"


int compare_types(char * type1, char * type2){
	//if(!strcmp(type2,"undef")) return 1;
	//if(!strcmp(type1,"undef")) return 1;
	//printf("oi\n");

	if(!strcmp(type1,"int") || !strcmp(type1,"char") || !strcmp(type1,"short")){
		if(strcmp(type2,"int") && strcmp(type2,"char") && strcmp(type2,"short")){
			return 1;
		}
		return 0;
	}
	else if(!strcmp(type1,"double")){
		if(strcmp(type2,"double") && strcmp(type2,"int") && strcmp(type2,"char") && strcmp(type2,"short")){
			return 1;
		}
		return 0;
	}
	/*
	else if(!strcmp(type1,"short")){
		if(!strcmp(type2,"double") || !strcmp(type2,"int")) return 1;

	}*/
	return 1;
}


int check_function(node * n1, node * n2){
	char * type1 = n1->annotate;
	char * type2 = NULL;
	if(n2 != NULL){
		type2 = n2->annotate;
	}
	if(strcmp(type1,"char") && strcmp(type1,"int") && strcmp(type1,"void") && strcmp(type1,"double") && strcmp(type1,"short") && strcmp(type1,"undef")){
		return 1;
	
	}
	if(type2 != NULL && strcmp(type2,"char") && strcmp(type2,"int") && strcmp(type2,"void") && strcmp(type2,"double") && strcmp(type2,"short") && strcmp(type2,"undef")){
		return 1;
	}
	return 0;
}


int check_void(node * n1, node * n2){
	if(!strcmp(n1->annotate,"void") || (n2 != NULL && !strcmp(n2->annotate,"void"))){
		return 1;
	}
	return 0;
}


void clear_param(param_list * param){
	if(param->next != NULL){
		clear_param(param->next);
		param->next = NULL;
	}
	if(param->id != NULL) free(param->id);
	free(param->type);
	free(param);
}

void clear_var(var_list * var){
	if(var->next != NULL){
	 clear_var(var->next);
		var->next = NULL;
	}
	free(var->id);
	free(var->type);
	if(var->parameters != NULL){
		clear_param(var->parameters);
		var->parameters = NULL;
	}
	free(var);
}

void clear_tables(sym_table * table){
	if(table->variables != NULL){
		clear_var(table->variables);
		table->variables = NULL;
	}
	/*
	if(table->parameters != NULL){
		clear_param(table->parameters);
		table->parameters = NULL;
	}*/
	if(table->next != NULL){
		clear_tables(table->next);
		table->next = NULL;
	}
	free(table->id);
	free(table->type);
	free(table);
}



sym_table * create_table(int function){
	sym_table * res = (sym_table * ) malloc (sizeof(sym_table));
	if(function){
		res->function = 1;
	}
	else{
		res->function = 0;
	}
	res->variables = NULL;
	res->parameters = NULL;
	res->nparameters = 0;
	res->defined = 0;
	res->next = NULL;
	res->id = NULL;
	res->type = NULL;

	return res;
}



var_list * create_var_list(int function){
	var_list * res;
	res = (var_list *) malloc(sizeof(var_list));
	if(function){
		res->function = 1;
	}
	else{
		res->function = 0;
	}
	res->parameters = NULL;
	res->nparameters = 0;
	res->next = NULL;
	res->id = NULL;
	res->type = NULL;
	return res;
}

param_list * create_param_list(){
	param_list * res;
	res = (param_list *) malloc(sizeof(param_list));
	res->next = NULL;
	res->id = NULL;
	res->type = NULL;
	return res;
}


void add_sym_table(sym_table ** table , sym_table * new){
	sym_table * aux = *table;
	if(aux == NULL){
		*table = new;
		return;
	}
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = new;
}

void add_var_list(var_list ** variables, var_list * new){
	new->type[0] = tolower(new->type[0]);
	var_list * aux = *variables;
	if(aux == NULL){
		*variables = new;
		return;
	}
	while(aux->next != NULL){
		aux = aux->next;
	} 
	aux->next = new;
	//printf("||||||||||||||||  %s  %s\n",aux->id,new->id);
}

void add_param_list(param_list ** parameters, param_list * new){
	new->type[0] = tolower(new->type[0]);
	param_list * aux = *parameters;
	if(aux == NULL){
		*parameters = new;
		return;
	}
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = new;
}

void is_func_declaration(node * current_node){
	
	node * aux, * aux2,  * aux4;
	param_list * aux_param, * aux_param2;
	var_list * aux_var, * aux_var2;
	sym_table * aux_sym;
	aux_var = create_var_list(1);
	aux = current_node->children;
	aux_var->type = strdup(aux->nodeType);
	aux_var->type[0] = tolower(aux_var->type[0]);
	aux = aux->brother;
	aux4 = aux;
	aux_var->id = strdup(aux->value);
	/*
	if(search_variable(global_table, aux_var->id) != NULL){
		current_node = current_node->brother;
		continue;
	}*/
	int discard  = 0;
	//int voidT = 0;
	aux = aux->brother; //ParamList
	aux = aux->children; // ParamDeclaration's
	while(aux != NULL){
	//	aux_var->nparameters++;
		aux_param2 = NULL;
		aux_param = create_param_list();
		aux2 = aux->children; //Typespec
		aux_param->type = strdup(aux2->nodeType);
		aux_param->type[0] = tolower(aux_param->type[0]);
		
		if(aux2->brother != NULL){
			aux2 = aux2->brother;
			aux_param->id = strdup(aux2->value);
			aux_param2 = search_param(aux_var->parameters,aux_param->id);
		}
		if(aux_param2 != NULL){
			
				printf("Line %d, col %d: Symbol %s already defined\n",aux2->nline, aux2->ncol, aux2->value);
				semanticErrors++;
				aux_param->id = NULL;
			
		}
		if(!strcmp(aux_param->type,"void") && (aux->children->brother != NULL || aux->brother != NULL || aux_var->nparameters > 0)){
			
			printf("Line %d, col %d: Invalid use of void type in declaration\n",aux->children->nline, aux->children->ncol);
			semanticErrors++;
			return;

		}
		if(strcmp(aux_param->type,"void")){
			aux_var->nparameters++;
		}

		add_param_list(&aux_var->parameters,aux_param);
		aux = aux->brother;

	}


	aux_var2 = search_var(global_table,aux_var->id);
	if(aux_var2 == NULL){

		if(!discard) add_var_list(&global_table->variables,aux_var);
		aux_sym = create_table(1);
		aux_sym->parameters = aux_var->parameters;
		aux_sym->nparameters = aux_var->nparameters;
		aux_sym->type = aux_var->type;
		aux_sym->id = aux_var->id;
		if(!discard) add_sym_table(&functions_table,aux_sym);

	}
	
	else{
		
		char * type = get_type(aux_var);
		char * type2 = get_type(aux_var2);
			if(strcmp(type,type2)){
				semanticErrors++;
				printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux4->nline, aux4->ncol, type, type2);
			}
	
		free(type);
		free(type2);
		//free(aux_var);
	}
}


sym_table * is_func_definition(node * current_node){
	node * aux, * aux2, *aux3, * aux4;
	param_list * aux_param, * aux_param2;
	var_list * aux_var, * aux_var2;
	sym_table * aux_sym;
	aux_var = create_var_list(1);
	aux = current_node->children; //TYPESPEC

	aux_var->type = strdup(aux->nodeType);
	aux_var->type[0] = tolower(aux_var->type[0]);
	aux = aux->brother; //ID
	aux4 = aux;
	aux_var->id = strdup(aux->value);
	aux3 = aux->brother; //ParamList
	int discard = 0;
	//int voidT = 0;




	aux = aux3->children; // ParamDeclaration's
	while(aux != NULL){
		aux_param2 = NULL;
		
		aux_param = create_param_list();
		aux2 = aux->children; //Typespec
		//printf("%s\n",aux2->nodeType);
		aux_param->type = strdup(aux2->nodeType);
		aux_param->type[0] = tolower(aux_param->type[0]);
		

		if(aux2->brother != NULL){

			aux2 = aux2->brother;
			aux_param->id = strdup(aux2->value);
			aux_param2 = search_param(aux_var->parameters,aux_param->id);
		}
		if(aux_param2 != NULL){
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s already defined\n",aux2->nline, aux2->ncol, aux2->value);
			
			aux_param->id = NULL;
			
		}
		if( !strcmp(aux_param->type,"void") && (aux->children->brother != NULL || aux->brother != NULL || aux_var->nparameters > 0)){
			semanticErrors++;
			
			printf("Line %d, col %d: Invalid use of void type in declaration\n",aux->children->nline, aux->children->ncol);
			return NULL;
			
		}
		if(strcmp(aux_param->type,"void")){
			aux_var->nparameters++;
		}
		add_param_list(&(aux_var->parameters), aux_param);
		aux = aux->brother;

	}

	aux_sym = search_function(aux_var->id);
	aux_var2 = search_var(global_table, aux_var->id);
	
	if(aux_var2 == NULL){
		if(!discard) add_var_list(&(global_table->variables), aux_var);
		aux_sym = create_table(1);
		aux_sym->parameters = aux_var->parameters;
		aux_sym->nparameters = aux_var->nparameters;
		aux_sym->type = aux_var->type;
		aux_sym->id = aux_var->id;
		if(!discard) add_sym_table(&functions_table,aux_sym);

	}
	else{

		if(!aux_var2->function){
			discard = 1;
			aux_sym = create_table(1);
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s already defined\n",aux4->nline, aux4->ncol, aux4->value);

		}else{
			
				char * type = get_type(aux_var);
				char * type2 = get_type(aux_var2);
				if(aux_sym->defined){
				discard = 1;
				aux_sym = create_table(1);
				semanticErrors++;
				//current_node = current_node->brother;
					//continue;
				printf("Line %d, col %d: Symbol %s already defined\n",aux4->nline, aux4->ncol, aux4->value);

				}
				else if(strcmp(type, type2)){
					semanticErrors++;
					discard = 1;
					aux_sym = create_table(1);
					printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux4->nline, aux4->ncol, type, type2);
					//current_node = current_node->brother;
					//continue;
				}
				
			

				
			
			
		}
	}
	aux_sym->parameters = aux_var->parameters;
	aux_sym->type = aux_var->type;
	aux_sym->defined = 1;
	return aux_sym;
}

void is_declaration(node * aux3, sym_table * aux_sym2){
	var_list * aux_var, * aux_var2;
	node * aux, * aux2;
	param_list * aux_param;
	sym_table * aux_sym;
	int discard = 0;
	aux_param = NULL;
	aux_sym = aux_sym2;
	if(aux_sym2 == NULL) aux_sym = global_table;
	aux_var = create_var_list(0);
	aux = aux3->children;
	aux_var->type = strdup(aux->nodeType);
	//aux2 = aux;
	aux_var->type[0] = tolower(aux_var->type[0]);

	aux = aux->brother;
	if(!strcmp(aux_var->type,"void")){
		discard = 1;
		semanticErrors++;
		printf("Line %d, col %d: Invalid use of void type in declaration\n",aux->nline, aux->ncol);
		//return;
	}
	aux_var->id = strdup(aux->value);
	aux2 = aux;
	aux_param = search_param(aux_sym->parameters,aux_var->id);
	aux_var2 = search_var(aux_sym, aux_var->id);
	
	if(aux_param == NULL && aux_var2 == NULL){
		if(!discard) add_var_list(&(aux_sym->variables),aux_var);
	}
	else{
		if(aux_param != NULL ){
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s already defined\n",aux->nline, aux->ncol, aux->value);
		
		}else if(aux_sym2 != NULL && aux_var != NULL){
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s already defined\n",aux->nline, aux->ncol, aux->value);
		}
		else if(strcmp(aux_var->type, aux_var2->type)){
			//printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux->nline, aux->ncol,aux_var->type, aux_var2->type);
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s already defined\n",aux->nline, aux->ncol, aux->value);	
		
		}
	}

	if(aux3->children->brother->brother != NULL  ){
			annotate_AST(aux3->children->brother->brother,aux_sym2,1);
			if(strcmp(aux_var->type,"void") && compare_types(aux_var->type,aux3->children->brother->brother->annotate)){
				semanticErrors++;
				printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux2->nline, aux2->ncol,aux3->children->brother->brother->annotate, aux_var->type);
			}

			//if(!strcmp(current->children->brother->brother->nodeType,"Id")) current->children->brother->brother->annotate = NULL;
		}
}


void create_semantic_table(node * root){
	
	param_list * aux_param;
	var_list * aux_var;
	sym_table * aux_sym;

	global_table = create_table(0);

	//putchar
	aux_var = create_var_list(1);
	aux_var->type = strdup("int");
	aux_var->id = strdup("putchar");
	aux_param = create_param_list();
	aux_var->nparameters = 1;
	aux_param->type = strdup("int");
	add_param_list(&aux_var->parameters,aux_param);
	add_var_list(&global_table->variables,aux_var);

	aux_sym = create_table(1);
	aux_sym->nparameters = 1;
	aux_sym->id = strdup("putchar");
	aux_sym->type = strdup("int");
	add_param_list(&aux_sym->parameters,aux_param);
	add_sym_table(&functions_table, aux_sym);
	//getchar
	aux_var = create_var_list(1);
	aux_var->type = strdup("int");
	aux_var->id = strdup("getchar");
	aux_param = create_param_list();
	aux_param->type = strdup("void");
	add_param_list(&aux_var->parameters,aux_param);
	add_var_list(&global_table->variables,aux_var);

	aux_sym = create_table(1);
	aux_sym->id = strdup("getchar");
	aux_sym->type = strdup("id");
	add_param_list(&aux_sym->parameters,aux_param);
	add_sym_table(&functions_table, aux_sym);
	
	annotate_AST(root,NULL,0);
	//printf("fim\n");

}

sym_table * search_function(char * function_name){
	sym_table * aux = functions_table;
	while(aux != NULL){
		if(strcmp(aux->id,function_name) == 0){
			return aux;
		}
		aux = aux->next;
	}
	return aux;
}

var_list * search_var(sym_table * table, char * name){
	var_list * aux = table->variables;
	while(aux != NULL){
		//printf("# %s # %s\n",aux->id,name);
		if(strcmp(aux->id,name) == 0){
			return aux;
		}
		aux = aux->next;
	}
	return NULL;
}

param_list * search_param(param_list * table, char * name){
	param_list * aux;
	aux = table;
	while(aux != NULL){
		//printf("-> %s\n",aux->id);

		if(aux->id != NULL && !strcmp(aux->id,name)){
			return aux;
		}
		aux = aux->next;
	}
	return NULL;
}

char * get_type(var_list  *  var){
	char * res;
	res =  (char*) malloc(sizeof(var->type) + 20 + 7 * var->nparameters);
	sprintf(res,"%s",var->type);
	param_list * aux;
	if(var->parameters != NULL){
		strcat(res,"(");
		aux = var->parameters;
		while(aux != NULL){
			strcat(res,aux->type);
			if(aux->next != NULL) strcat(res,",");
			aux = aux->next;
		}
		strcat(res,")");
	}
	return res;
}



char * search_variable(sym_table * table, char * variable_name){
	param_list * aux_param;
	var_list * aux_var;
	aux_param = table->parameters;
	aux_var = table->variables;
	while(aux_param != NULL){
		//printf("while %s\n",aux_param->id);
		if(aux_param->id == NULL){
			aux_param = aux_param->next;
			continue;
		}
		if(strcmp(aux_param->id,variable_name) == 0){
			return aux_param->type;
		}
		aux_param = aux_param->next;
	}
	while(aux_var != NULL){
		//printf("while %s\n",aux_param->id);
		if(aux_var->id == NULL){
			aux_var = aux_var->next;
			continue;
		}
		if(strcmp(aux_var->id,variable_name) == 0){
			return aux_var->type;
		}
		aux_var = aux_var->next;
	}
	return NULL;
}



void print_global_table(sym_table *table){
	param_list * aux_param;
	printf("===== Global Symbol Table =====\n");
	var_list * aux = table->variables;

	while(aux != NULL){
		
		printf("%s\t%s",aux->id,aux->type);
		if(aux->function){
			printf("(");
			aux_param = aux->parameters;
			while(aux_param != NULL){
				printf("%s",aux_param->type);
				if(aux_param->next != NULL) printf(",");
				aux_param = aux_param->next;
			}
			printf(")");
		}

		printf("\n");
		
		aux = aux->next;
	}
	printf("\n");
}


void print_functions_table(sym_table *table){
	var_list * aux_var;
	param_list * aux_param;
	sym_table * aux_sym = table;
	while(aux_sym != NULL){
		if(!aux_sym->defined || !strcmp(aux_sym->id,"getchar") || !strcmp(aux_sym->id,"putchar")){
			aux_sym = aux_sym->next;
			continue;
		}
		printf("===== Function %s Symbol Table =====\n",aux_sym->id);
		printf("return\t%s\n",aux_sym->type);
		aux_param = aux_sym->parameters;
		while(aux_param != NULL){
			if(!strcmp(aux_param->type,"void") || aux_param->id == NULL){
				aux_param = aux_param->next;
				continue;
			} 	
			//if(aux_param->id == NULL) printf("null");
			printf("%s",aux_param->id);
			printf("\t%s\tparam\n", aux_param->type);
			aux_param = aux_param->next;
		}
		aux_var = aux_sym->variables;
		while(aux_var != NULL){
			printf("%s\t%s\n",aux_var->id, aux_var->type);
			aux_var = aux_var->next;
		}
		aux_sym = aux_sym->next;
		printf("\n");
	}

}


void clear_annotate(node * current){
	if(current == NULL) return;
	clear_annotate(current->brother);
	clear_annotate(current->children);
	current->annotate = NULL;
}

int check_undef(node * n1, node * n2){
	
	if(!strcmp(n1->annotate,"undef") || (n2 != NULL && !strcmp(n2->annotate,"undef"))){
		return 1;
	}
	return 0;
}


int check_double(node * n1, node * n2){
	
	if(check_undef(n1,n2)){
		return 1;
	}
	if(!strcmp(n1->annotate,"double") || (n2 != NULL && !strcmp(n2->annotate,"double"))){
		return 1;
	}
	return 0;
}



void annotate_AST(node * current, sym_table * current_function, int expression){
	node * aux, * aux2, * aux3;
	char * type;
	sym_table * aux_sym;
	param_list * aux_param;
	var_list * aux_var, * aux_var2;
	if(current == NULL){
		//printf("null\n");
		return;
	}
	//if(current->annotate != NULL) return;
	//printf("->%s  %d %d\n",current->nodeType,current->nline,current->ncol);
	if(!strcmp(current->nodeType,"Program")){
		aux = current->children;
		while(aux != NULL){
			annotate_AST(aux, NULL, 0);
			aux = aux->brother;
		}
	}
	else if(!strcmp(current->nodeType,"FuncDeclaration")){
		is_func_declaration(current);
		//annotate_AST(current->brother, NULL, 0);
		return;
	}

	else if(!strcmp(current->nodeType,"FuncDefinition")){
		aux = current->children->brother->brother->brother->children; //FUNC BODY
		//current_function = search_function(aux->value);
		current_function = is_func_definition(current);
		if(current_function == NULL) return;
		//current_function = search_function(current_function->id);	
		while(aux != NULL){
			annotate_AST(aux,current_function,1);
			aux = aux->brother;
		}
			
		//aux->annotate = NULL;	
		//annotate_AST(current->brother, NULL,0);
		return;
	}
	else if(!strcmp(current->nodeType,"Declaration") ){
		is_declaration(current,current_function);
		//annotate_AST(current->children,current_function,1);

		
		//current->children->brother->annotate = NULL;
		//clear_annotate(current->children);
		//annotate_AST(current->brother,current_function,expression);
		return;
	}

	else if(!strcmp(current->nodeType,"IntLit") || !strcmp(current->nodeType,"ChrLit")){
		current->annotate = strdup("int");
	}
	else if(!strcmp(current->nodeType,"RealLit")){
		current->annotate = strdup("double");
	}
	else if(!strcmp(current->nodeType,"Call")){
	//	int isfunc = 1;
		aux = current->children;
		aux_sym = NULL;
		aux_var = NULL;
		aux_var2 = NULL;
		aux_param = NULL;	
		if(current_function != NULL) aux_var2 = search_var(current_function,aux->value);
		if(current_function != NULL) aux_param = search_param(current_function->parameters,aux->value);
		if(aux_var2 == NULL && aux_param == NULL){

			aux_sym  = search_function(aux->value);
			aux_var = search_var(global_table,aux->value);
		}
		annotate_AST(aux,current_function,1);
	
		if(aux_param != NULL){
			current->annotate = strdup(aux_param->type);
		
		}
		else if(aux_var2 != NULL){
			current->annotate = strdup(aux_var2->type);
		
		}
		else if(aux_var != NULL){
			current->annotate = strdup(aux_var->type);
		}
		else{
			current->annotate = strdup("undef");
			return;
		}
		if((aux_var != NULL && aux_sym == NULL) || aux_var2 != NULL || aux_param != NULL){
			semanticErrors++;
			printf("Line %d, col %d: Symbol %s is not a function\n",aux->nline, aux->ncol, aux->value);
			return;
		}


		aux_param = NULL;
		if(aux_sym != NULL) aux_param = aux_sym->parameters;

		aux2 = aux->brother;
		aux3 = aux2;
		int nparameters = 0;


		while( aux_param != NULL || aux2 != NULL){
			
			
			if(aux2 != NULL){
				annotate_AST(aux2,current_function,1);
				aux3 = aux2;
				nparameters++;
				aux2 = aux2->brother;
			}
			
			if(aux_param != NULL && aux3 != NULL && strcmp(aux_param->type,"void")){
			
				if(compare_types(aux_param->type,aux3->annotate)){
					semanticErrors++;
					printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux3->nline, aux3->ncol,aux3->annotate,aux_param->type);
					
				}
			}
			if(aux_param != NULL){	
				
				aux_param = aux_param->next;
			}
			
		}
			if(aux_sym != NULL && nparameters != aux_sym->nparameters){
				semanticErrors++;
				printf("Line %d, col %d: Wrong number of arguments to function %s (got %d, required %d)\n",aux->nline, aux->ncol, aux_sym->id,nparameters, aux_sym->nparameters);
			

		}

		
	}

	else if(!strcmp(current->nodeType,"Id") && expression){
		
		aux_var2 = NULL;
		aux_param = NULL;
		aux_var = NULL;
		if(current_function != NULL){
			aux_var2 = search_var(current_function,current->value);
			aux_param = search_param(current_function->parameters,current->value);
		}
		if(aux_var2 == NULL && aux_param == NULL) aux_var = search_var(global_table,current->value);
	
		if(aux_var == NULL && aux_var2 == NULL && aux_param == NULL){
			semanticErrors++;
			printf("Line %d, col %d: Unknown symbol %s\n",current->nline,current->ncol,current->value);
			current->annotate = strdup("undef");
		}else{
			if(aux_var2 != NULL) type = aux_var2->type;
			else if(aux_param != NULL) type = aux_param->type;
			else{
				
			 	type = get_type(aux_var);
			 	
			}
			
			current->annotate = strdup(type);

			if(aux_var2 == NULL && aux_param == NULL){
				free(type);
			}
			
		}
	}

	else if(!strcmp(current->nodeType,"Store")){
		aux = current->children;
		aux2 = aux->brother;
	//	printf("-> %d\n",current->nline);
		annotate_AST(aux,current_function,1);
		annotate_AST(aux2,current_function,1);
		current->annotate = strdup(aux->annotate);
		if(strcmp(aux->nodeType,"Id")){
			semanticErrors++;
			printf("Line %d, col %d: Lvalue required\n",aux->nline, aux->ncol);
			
		}
		if(check_function(aux,NULL)){
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline,current->ncol,current->value,aux->annotate,aux2->annotate);
			
			current->annotate = strdup("undef");
		
		}
		else if(!check_undef(aux,NULL) && compare_types(aux->annotate,aux2->annotate)){
			semanticErrors++;
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline,current->ncol,current->value,aux->annotate,aux2->annotate);
		
		}
	
		

		
	}

	else if(!strcmp(current->nodeType,"BitWiseAnd") || !strcmp(current->nodeType,"BitWiseXor") || !strcmp(current->nodeType,"BitWiseOr")){
		aux = current->children;
		aux2 = aux->brother;
		annotate_AST(aux,current_function,1);
		annotate_AST(aux2,current_function,1);
		
		if(check_double(aux,aux2) || check_function(aux,aux2) || check_void(aux,aux2)){
			
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
			semanticErrors++;
		}
	
		current->annotate = strdup("int");
		
	}


	else if(!strcmp(current->nodeType,"Le") || !strcmp(current->nodeType,"Ge") || !strcmp(current->nodeType,"Lt") || !strcmp(current->nodeType,"Gt")
		|| !strcmp(current->nodeType,"Ne") || !strcmp(current->nodeType,"Eq")){
		aux = current->children;
		aux2 = aux->brother;
		annotate_AST(aux,current_function,1);
		annotate_AST(aux2,current_function,1);
		
		if(check_undef(aux,aux2) || check_function(aux,aux2) || check_void(aux,aux2)){
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
			semanticErrors++;
		}
	
			current->annotate = strdup("int");
	
	}
	
	else if(!strcmp(current->nodeType,"Not") ){
		aux  = current->children;
		annotate_AST(aux,current_function,1);
		
		if(check_double(aux,NULL) || check_function(aux,NULL) || check_void(aux,NULL)){
			printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",current->nline, current->ncol,current->value,aux->annotate);
			semanticErrors++;
		}
		
			current->annotate = strdup("int");
		
	}

	else if(!strcmp(current->nodeType,"Plus") || !strcmp(current->nodeType,"Minus")){
		aux  = current->children;
		annotate_AST(aux,current_function,1);
		//if(!strcmp(aux->annotate,"char") || !strcmp(aux->annotate,"short")) current->annotate = strdup("int");
		
		if(check_undef(aux,NULL) || check_function(aux,NULL) ){
			semanticErrors++;
			printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",current->nline, current->ncol,current->value,aux->annotate);
			current->annotate = strdup("undef");
		}
		else{
			current->annotate = strdup(aux->annotate);
		}
	}

	else if(!strcmp(current->nodeType,"And") || !strcmp(current->nodeType,"Or")){
		aux = current->children;
		aux2 = aux->brother;
		annotate_AST(aux,current_function,1);
		annotate_AST(aux2,current_function,1);
		
		if(check_double(aux,aux2) || check_function(aux,aux2) || check_void(aux,aux2)){
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
			semanticErrors++;
		}
		
			current->annotate = strdup("int");
		

	}
	
	else if(!strcmp(current->nodeType,"Mod")  ){
		aux = current->children;
		annotate_AST(aux,current_function,1);
		aux2 = aux->brother;
		annotate_AST(aux2,current_function,1);
		
	
		if(check_double(aux,aux2) || check_function(aux,aux2) || check_void(aux,aux2)){
			
			printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
			semanticErrors++;
		}
		
			current->annotate = strdup("int");
		
	}


	else if(!strcmp(current->nodeType,"Mul")   || !strcmp(current->nodeType,"Div") || !strcmp(current->nodeType,"Add") || !strcmp(current->nodeType,"Sub")  ){
			aux = current->children;
			annotate_AST(aux,current_function,1);
			aux2 = aux->brother;
			annotate_AST(aux2,current_function,1);
			
			if(check_undef(aux,aux2) || check_function(aux,aux2) || check_void(aux,aux2)){
				
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
				current->annotate = strdup("undef");
				semanticErrors++;
			//free(operator);
			}

			else if(!strcmp(aux->annotate,"double") || !strcmp(aux2->annotate,"double")){
				current->annotate = strdup("double");
			}
			/*
			else{
				current->annotate = "int";
			}*/
			else if(!strcmp(aux->annotate,"int") || !strcmp(aux2->annotate,"int")){
				current->annotate = strdup("int");
			}else if(!strcmp(aux->annotate,"short") || !strcmp(aux2->annotate,"short")){
				current->annotate = strdup("short");
			}else {
				current->annotate = strdup("char");
			}



	}
	else if(!strcmp(current->nodeType,"Comma")){
		aux = current->children;
		aux2 = aux->brother;
		annotate_AST(aux,current_function,1);
		annotate_AST(aux2,current_function,1);
		if(check_undef(aux2,aux) || check_function(aux2,aux) ){
				semanticErrors++;
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",current->nline, current->ncol,current->value,aux->annotate,aux2->annotate);
				current->annotate = strdup("undef");
			//free(operator);
			}
			else{

			current->annotate = strdup(aux2->annotate);
			}
	}

	else if(!strcmp(current->nodeType,"While")){
		aux = current->children;
		aux2 = aux->brother;
		annotate_AST(aux,current_function,expression);
		if(check_double(aux,NULL) || check_function(aux,NULL) || check_void(aux,NULL)){
			semanticErrors++;
			printf("Line %d, col %d: Conflicting types (got %s, expected int)\n",aux->nline, aux->ncol, aux->annotate);
		}
		annotate_AST(aux2,current_function,expression);
	}

	else if(!strcmp(current->nodeType,"If")){
		aux = current->children;
		annotate_AST(aux,current_function,expression);

		if(check_double(aux,NULL) || check_function(aux,NULL) || check_void(aux,NULL)){
			semanticErrors++;
			printf("Line %d, col %d: Conflicting types (got %s, expected int)\n",aux->nline, aux->ncol, aux->annotate);
		}
		
		

		aux = aux->brother;
		annotate_AST(aux,current_function,expression);
		aux = aux->brother;
		annotate_AST(aux,current_function,expression);
		
		
	}

	else if(!strcmp(current->nodeType,"StatList")){
		aux = current->children;
		while(aux != NULL){
			annotate_AST(aux,current_function,expression);
			aux = aux->brother;
		}
	}
	
	else if(!strcmp(current->nodeType,"Return")){
		aux = current->children;
		annotate_AST(aux,current_function,expression);
		if(!strcmp(current_function->type,"void")){
			if(strcmp(aux->nodeType,"Null") && !check_void(aux,NULL)){
				semanticErrors++;
				printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux->nline, aux->ncol, aux->annotate, current_function->type);
			}
					
		}
		else{
			if(strcmp(aux->nodeType,"Null") && (compare_types(current_function->type,aux->annotate) || check_void(aux,NULL))){
				semanticErrors++;
				printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n",aux->nline, aux->ncol, aux->annotate, current_function->type);
			}
			else if(!strcmp(aux->nodeType,"Null")){
				semanticErrors++;
				printf("Line %d, col %d: Conflicting types (got void, expected %s)\n",current->nline, current->ncol, current_function->type);
			
			}
		}

	}
	

	else{

		if(current->brother != NULL && current->brother->annotate == NULL) annotate_AST(current->brother,current_function,expression);
		if(current->children != NULL && current->children->annotate == NULL) annotate_AST(current->children,current_function,expression);
	}
	


}
