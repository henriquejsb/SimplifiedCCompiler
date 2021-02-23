
#include "header.h"


//---------------------------------------------------META 4 -----------------------------------------------------------

int temp_vars = 1;
int label_number = 1;
sym_table * curr_func = NULL;
int found_ret = 0;
int andor_label_number = 1;
char * main_func; 


void generate_LLVM(node * root){
	print_globals(root);
	convert_LLVM(root);
	//print_main();
}






void store_globals(node * current){
	node * aux, * aux2 ,* aux3;
	aux = current->children; //TYPE
	aux2 = aux->brother; //ID
	aux3 = aux2->brother; //VALUE

	convert_LLVM(aux3);
	if(!strcmp(aux->nodeType,"Double")){
		if(strcmp(aux3->annotate,"double")){
			printf("\t%%%d = sitofp %s %%%d to double\n",temp_vars, convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		printf("\tstore double %%%d, double* @%s, align 8\n", temp_vars-1, aux2->value);
	}
	else if(!strcmp(aux->nodeType, "Int")){
		if(!strcmp(aux3->annotate,"double")){
			printf("\t%%%d = fptosi %s %%%d to i32\n",temp_vars,convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		else if(strcmp(aux3->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		printf("\tstore i32 %%%d, i32* @%s, align 4\n", temp_vars-1, aux2->value);

	}else if(!strcmp(aux->nodeType,"Short")){
		if(!strcmp(aux3->annotate,"char")){
			printf("\t%%%d = sext %s %%%d to i16\n",temp_vars, convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		else if(!strcmp(aux3->annotate,"int")){
			printf("\t%%%d = trunc %s %%%d to i16\n",temp_vars, convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		printf("\tstore i16 %%%d, i16* @%s, align 2\n", temp_vars-1, aux2->value);

	}else if(!strcmp(aux->nodeType,"Char")){
		if(strcmp(aux3->annotate,"char")){
			printf("\t%%%d = trunc %s %%%d to i8\n",temp_vars, convert_type(aux3->annotate),temp_vars-1);
			temp_vars++;
		}
		printf("\tstore i8 %%%d, i8* @%s, align 1\n", temp_vars-1, aux2->value);


	}
}

void print_globals(node * root){
	node * aux2, * aux;
	printf("declare i32 @putchar(i32)\n");
	printf("declare i32 @getchar()\n");	
	var_list * global_vars = global_table->variables;
	while(global_vars != NULL){

		if(!global_vars->function){

			if(!strcmp(global_vars->type, "int")){
				printf("@%s = common global i32 0, align 4\n",global_vars->id);

			}else if(!strcmp(global_vars->type,"short")){
				printf("@%s = common global i16 0, align 2\n",global_vars->id);
		
			}else if(!strcmp(global_vars->type,"double")){
				printf("@%s = common global double  0.000000e+00, align 8\n",global_vars->id);
		

			}else if(!strcmp(global_vars->type,"char")){
				printf("@%s = common global i8 0, align 1\n",global_vars->id);
		
			}
		}
		global_vars = global_vars->next;
	}
	printf("\ndefine i32 @main(i32 %%argc, i8** %%argv) {\n");
	node * aux3 = root->children;
	while(aux3 != NULL){
		if(!strcmp(aux3->nodeType,"Declaration")){
			aux = aux3->children;
			aux2 = aux->brother;
			if(aux2->brother != NULL){

				store_globals(aux3);
			}
		}
		aux3 = aux3->brother;
	}

	sym_table * aux_sym = search_function("main");
	//printf("%s",main_func);
	printf("\tcall %s @f.main()\n",convert_type(aux_sym->type));

    	printf("\tret i32 0\n");
    printf("}\n");
	//printf("define i32 @main(i32 %%argc, i8** %%argv) {\n");
    	
}


char * convert_type(char * type){
	char * res = (char *) malloc(sizeof(char) * 7);
	if(!strcmp(type,"Int") || !strcmp(type,"int")){
		strcpy(res,"i32");
	}
	else if(!strcmp(type,"Short") || !strcmp(type,"short")){
		strcpy(res,"i16");
	}
	else if(!strcmp(type,"Char") || !strcmp(type,"char")){
		strcpy(res,"i8");
	}
	else if(!strcmp(type,"Double") || !strcmp(type,"double")){
		strcpy(res,"double");
	}
	else if(!strcmp(type,"Void") || !strcmp(type,"void")){
		strcpy(res,"void");
	}
	return res;
}



void func_declaration(node * current){
	node * aux;
	aux = current->children;
	printf("declare ");
	printf("%s ",convert_type(aux->nodeType));
	aux = aux->brother;
	printf("@f.%s(",aux->value);
	aux = aux->brother->children;
	while(aux != NULL){
		if(!strcmp(aux->children->nodeType,"Void")) break;
		printf("%s",convert_type(aux->children->nodeType));
		if(aux->brother != NULL) printf(",");
		aux = aux->brother;
	}
	printf(")\n");
}

void add(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(current->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fadd double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	/*else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = add nsw i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}*/
	
	else if(!strcmp(current->annotate,"int")){
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = add nsw i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else if(!strcmp(current->annotate,"short")){
		if(strcmp(aux1->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = add nsw i16 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		printf("\t%%%d = add nsw i8 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
}



void sub(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(current->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		else if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fsub double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	/*else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = sub nsw i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}*/
	
	else if(!strcmp(current->annotate,"int")){
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = sub nsw i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else if(!strcmp(current->annotate,"short")){
		if(strcmp(aux1->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		else if(strcmp(aux2->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = sub  nsw i16 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		printf("\t%%%d = sub nsw i8 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
}



void mul(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(current->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		else if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fmul double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	/*else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
	}*/
	
	else if(!strcmp(current->annotate,"int")){
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = mul nsw i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else if(!strcmp(current->annotate,"short")){
		if(strcmp(aux1->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		else if(strcmp(aux2->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = mul nsw i16 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		printf("\t%%%d = mul nsw i8 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
}



void div_func(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(current->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		else if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fdiv double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	/*else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
	}*/
	
	else if(!strcmp(current->annotate,"int")){
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = sdiv i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else if(!strcmp(current->annotate,"short")){
		if(strcmp(aux1->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		else if(strcmp(aux2->annotate,"short")){
			printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = sdiv i16 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		printf("\t%%%d = sdiv i8 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
}




void mod(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(strcmp(aux1->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
		var1 = temp_vars;
		temp_vars++;
	}
	if(strcmp(aux2->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
		var2 = temp_vars;
		temp_vars++;
	}
	printf("\t%%%d = srem i32 %%%d, %%%d\n", temp_vars, var1, var2);
    temp_vars++;	
}

void minus(node * current){
	node * aux1 = current->children;
    if(!strcmp(current->annotate, "double")){
        printf("\t%%%d = fsub double -0.000000e+00, %%%d\n", temp_vars, temp_vars-1);
        temp_vars++;
    }
    else{
    	
		
    	printf("\t%%%d = sub nsw %s 0, %%%d\n", temp_vars ,convert_type(aux1->annotate), temp_vars-1);
        temp_vars++;
       
    }
}

void plus(node * current){
	node * aux1 = current->children;
    if(!strcmp(current->annotate, "double")){
       printf("\t%%%d = fadd double -0.000000e+00, %%%d\n", temp_vars, temp_vars-1);
        temp_vars++;
    }
    else{
    	/*
    	if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), temp_vars-1);
		
			temp_vars++;
		}*/
		
    	printf("\t%%%d = add nsw %s 0, %%%d\n", temp_vars , convert_type(aux1->annotate), temp_vars-1);
        temp_vars++;
        
    }
}

void not(node * current){
	node * aux1 = current->children;
	if(strcmp(aux1->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), temp_vars-1);
		
		temp_vars++;
	}

	printf("\t%%%d = icmp eq i32 %%%d, 0\n", temp_vars,  temp_vars-1);
	temp_vars++;
	

	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}

void eq(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp oeq double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp eq i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}

	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}


void ne(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp une double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp ne i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}


void lt(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp olt double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp slt i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}

void gt(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp ogt double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp sgt i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}

void le(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp ole double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp sle i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}


void ge(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(aux1->annotate,"double") || !strcmp(aux2->annotate,"double")){
		if(strcmp(aux1->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux1->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var2);
			 var2 = temp_vars;
			 temp_vars++;
		}
		printf("\t%%%d = fcmp oge double %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	else{
		if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
		printf("\t%%%d = icmp sge i32 %%%d, %%%d\n", temp_vars, var1, var2);
        temp_vars++;
	}
	
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars,temp_vars-1);
	temp_vars++;
}



void bitwiseand(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	
	if(strcmp(aux1->annotate,"int")){
		printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
		var1 = temp_vars;
		temp_vars++;
	}
	
	if(strcmp(aux2->annotate,"int")){
		printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
		var2 = temp_vars;
		temp_vars++;
	}
	
	printf("\t%%%d = and i32 %%%d, %%%d\n", temp_vars, var1, var2);
    temp_vars++;
 
}

void bitwiseor(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	
	if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		
	if(strcmp(aux2->annotate,"int")){
		printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
		var2 = temp_vars;
		temp_vars++;
	}
	
	printf("\t%%%d = or i32 %%%d, %%%d\n", temp_vars, var1, var2);
    temp_vars++;

}

void bitwisexor(node * current, int var1, int var2){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	
	if(strcmp(aux1->annotate,"int")){
			printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux1->annotate), var1);
			var1 = temp_vars;
			temp_vars++;
		}
		
		if(strcmp(aux2->annotate,"int")){
			printf("\t%%%d = zext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var2);
			var2 = temp_vars;
			temp_vars++;
		}
	
	printf("\t%%%d = xor i32 %%%d, %%%d\n", temp_vars, var1, var2);
    temp_vars++;
	
}

int check_var(char * var){
	if(curr_func == NULL) return 0;
	
	var_list * aux_var = curr_func->variables;
	while(aux_var != NULL){
		//printf("M\n");
		
		
			if(!strcmp(aux_var->id,var)) return 1;
			aux_var = aux_var->next;
		
	}
	return 0;
}

int check_parameter(char * var){
	if(curr_func == NULL) return 0;
	param_list * aux_param;
	aux_param = curr_func->parameters;
	while(aux_param != NULL ){
		//printf("M\n");
		
		if(aux_param->id != NULL)
			if(!strcmp(aux_param->id,var)) return aux_param->var;
		aux_param = aux_param->next;
		
		
	}
	return 0;
}




void store(node * current, int var1){
	node * aux1 , * aux2;
	aux1 = current->children;
	aux2 = aux1->brother;
	if(!strcmp(current->annotate,"double")){
		if(strcmp(aux2->annotate,"double")){
			 printf("\t%%%d = sitofp %s %%%d to double\n", temp_vars, convert_type(aux2->annotate), var1);
			 var1 = temp_vars;
			 temp_vars++;
		}
		int param = check_parameter(aux1->value);
		if(param){
			printf("\tstore double %%%d, double* %%%d, align 8\n", var1, param);
		}
		else if(check_var(aux1->value)){
			printf("\tstore double %%%d, double* %%%s, align 8\n", var1, aux1->value);
			
		}
		else{
			printf("\tstore double %%%d, double* @%s, align 8\n", var1, aux1->value);
		}
    }
	else if(!strcmp(current->annotate,"int")){
		
		if(strcmp(aux2->annotate,"int")){
		

				printf("\t%%%d = sext %s %%%d to i32\n", temp_vars, convert_type(aux2->annotate), var1);
				var1 = temp_vars;
				temp_vars++;
			
		}
		
        int param = check_parameter(aux1->value);
		if(param){
			printf("\tstore i32 %%%d, i32* %%%d, align 4\n", var1, param);
		}
		else if(check_var(aux1->value)){
			printf("\tstore i32 %%%d, i32* %%%s, align 4\n", var1, aux1->value);
			
		}
		else{
			printf("\tstore i32 %%%d, i32* @%s, align 4\n", var1, aux1->value);
		}
	}
	else if(!strcmp(current->annotate,"short")){
		
		if(strcmp(aux2->annotate,"short")){
			if(!strcmp(aux2->annotate,"int")){
				printf("\t%%%d = trunc %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var1);
				var1 = temp_vars;
				temp_vars++;	
			}
			else{
				printf("\t%%%d = sext %s %%%d to i16\n", temp_vars, convert_type(aux2->annotate), var1);
				var1 = temp_vars;
				temp_vars++;	
			}
			
			
		}
		int param = check_parameter(aux1->value);
		if(param){
			printf("\tstore i16 %%%d, i16* %%%d, align 2\n", var1, param);
		}
		else if(check_var(aux1->value)){
			printf("\tstore i16 %%%d, i16* %%%s, align 2\n", var1, aux1->value);
			
		}
		else{
			printf("\tstore i16 %%%d, i16* @%s, align 2\n", var1, aux1->value);
		}
	}
	else{
		if(strcmp(aux2->annotate,"char")){
			printf("\t%%%d = trunc %s %%%d to i8\n", temp_vars, convert_type(aux2->annotate), var1);
			var1 = temp_vars;
			temp_vars++;	
		}
		int param = check_parameter(aux1->value);
		if(param){
			printf("\tstore i8 %%%d, i8* %%%d, align 1\n", var1, param);
		}
		else if(check_var(aux1->value)){
			printf("\tstore i8 %%%d, i8* %%%s, align 1\n", var1, aux1->value);
			
		}
		else{
			printf("\tstore i8 %%%d, i8* @%s, align 1\n", var1, aux1->value);
		}
	}
}

void reallit(node * current){
	printf("\t%%%d = fadd double 0.000000e+00, %.16E\n", temp_vars, atof(current->value));
    temp_vars++;
}




int power(int base, int exp){
	//if(exp == 0) return 1;
	int res = 1;
	for(int i = 0; i < exp; i++){
		res *= base;
	}
	return res;
}


int convert_octal(char * octal){
	//char * aux = (char *) malloc(sizeof(char) * 3);
	//sprintf(aux,"");
	int i = 0;
	while(octal[i] != '\0' && octal[i] != '\''){
	//	aux[i] = octal[i];
		i++;
	}
	//printf("----> %s\n",aux);
	int res = 0;
	int j = 0;
	while(j < i){
		
		//printf("->->-> %d\n",aux);
		res += (octal[j] - '0') * power(8,i-j-1);
		j++;
	}
	return res;
}

void intlit(node * current){
	int res;
	if(current->value[0] == '0'){
		res = convert_octal(current->value);
	}else{
		res = atoi(current->value);
	}
	printf("\t%%%d = add nsw i32 0, %d\n", temp_vars, res);
    temp_vars++;
}

void chrlit(node * current){
	//printf("----------------\n -> %d  %c \n -------------\n",*(current->value+1));
	int ascii;
	if(current->value[1] == '\\'){
		if(current->value[2] == 'n'){
			ascii = 10;
		}
		else if(current->value[2] == 't'){
			ascii = 9;
		}
		else if(current->value[2] == '\\'){
			ascii = 92;
		}
		else if(current->value[2] == '\''){
			ascii = 39;
		}
		else if(current->value[2] == '"'){
			ascii = 34;
		}
		else{
			ascii = convert_octal(&(current->value[2]));
		}
	}
	else{
		ascii = *(current->value+1);
	}
	printf("\t%%%d = add nsw i32 0, %d \n", temp_vars,ascii);
    temp_vars++;	
}

void declaration(node * current){
	node * aux = current->children; //TYPE
	node * aux2 = aux->brother; //ID
	
	
		if(!strcmp(aux->nodeType,"Int")){
			 printf("\t%%%s = alloca i32, align 4\n", aux2->value);
		}
		else if(!strcmp(aux->nodeType,"Double")){
			 printf("\t%%%s = alloca double, align 8\n", aux2->value);
		}
		else if(!strcmp(aux->nodeType,"Short")){

			 printf("\t%%%s = alloca i16, align 2\n", aux2->value);
		}
		else if(!strcmp(aux->nodeType,"Char")){

			 printf("\t%%%s = alloca i8, align 1\n", aux2->value);
		}
		if(aux2->brother != NULL){

			convert_LLVM(aux2->brother);
			if(!strcmp(aux->nodeType,"Double")){
				if(strcmp(aux2->brother->annotate,"double")){
					printf("\t%%%d = sitofp %s %%%d to double\n",temp_vars,convert_type(aux2->brother->annotate),temp_vars-1);
					temp_vars++;
				}
				printf("\tstore double %%%d, double* %%%s, align 8\n",temp_vars-1,aux2->value);
			}
			else if(!strcmp(aux->nodeType,"Int")){
				
				if(strcmp(aux2->brother->annotate,"int")){
					printf("\t%%%d = sext %s %%%d to i32\n",temp_vars,convert_type(aux2->brother->annotate),temp_vars-1);
					temp_vars++;
				}
				printf("\tstore i32 %%%d, i32* %%%s, align 4\n",temp_vars-1, aux2->value);
			}
			else if(!strcmp(aux->nodeType,"Short")){
				if(!strcmp(aux2->brother->annotate,"int")){
					printf("\t%%%d = trunc %s %%%d to i16\n",temp_vars,convert_type(aux2->brother->annotate),temp_vars-1);
					temp_vars++;
				}
				else if(!strcmp(aux2->brother->annotate,"char")){
					printf("\t%%%d = sext %s %%%d to i16\n",temp_vars,convert_type(aux2->brother->annotate),temp_vars-1);
					temp_vars++;
				}
				printf("\tstore i16 %%%d, i16* %%%s, align 2\n",temp_vars-1, aux2->value);
			}
			else if(!strcmp(aux->nodeType,"Char")){
				if(strcmp(aux2->brother->annotate,"char")){
					printf("\t%%%d = trunc %s %%%d to i8\n",temp_vars,convert_type(aux2->brother->annotate),temp_vars-1);
					temp_vars++;
				}
				printf("\tstore i8 %%%d, i8* %%%s, align 1\n",temp_vars-1, aux2->value);
			}

		}
	
}



void id(node * current){
//	printf("oi3\n");
	int param = 0;
	param = check_parameter(current->value);

		//param_list * aux_param = search_param(curr_func->parameters,current->value);
		//var_list * aux_var = search_var(curr_func,current->value);
	if(param){
		
		if(!strcmp(current->annotate,"int")){
			printf("\t%%%d = load i32, i32* %%%d, align 4\n",temp_vars,param);
		}
		else if(!strcmp(current->annotate,"short")){
			printf("\t%%%d = load i16, i16* %%%d, align 2\n",temp_vars,param);
		}
		else if(!strcmp(current->annotate,"char")){

			printf("\t%%%d = load i8, i8* %%%d, align 1\n",temp_vars,param);
		}
		else if(!strcmp(current->annotate,"double")){

			printf("\t%%%d = load double, double* %%%d, align 8\n",temp_vars,param);
		}
	}
	else if(check_var(current->value)){
		
		if(!strcmp(current->annotate,"int")){
			printf("\t%%%d = load i32, i32* %%%s, align 4\n",temp_vars,current->value);
		}
		else if(!strcmp(current->annotate,"short")){
			printf("\t%%%d = load i16, i16* %%%s, align 2\n",temp_vars,current->value);
		}
		else if(!strcmp(current->annotate,"char")){

			printf("\t%%%d = load i8, i8* %%%s, align 1\n",temp_vars,current->value);
		}
		else if(!strcmp(current->annotate,"double")){

			printf("\t%%%d = load double, double* %%%s, align 8\n",temp_vars,current->value);
		}
	}


 	else{
	
		if(!strcmp(current->annotate,"int")){
				printf("\t%%%d = load i32, i32* @%s, align 4\n",temp_vars,current->value);
			}
			else if(!strcmp(current->annotate,"short")){
				printf("\t%%%d = load i16, i16* @%s, align 2\n",temp_vars,current->value);
			}
			else if(!strcmp(current->annotate,"char")){
				printf("\t%%%d = load i8, i8* @%s, align 1\n",temp_vars,current->value);
			}
			else if(!strcmp(current->annotate,"double")){
				printf("\t%%%d = load double, double* @%s, align 8\n",temp_vars,current->value);
		}
	}
	
	temp_vars++;
	//printf("oi4\n");
	
}

void return_func(node * current){
	node * aux = current->children;
	if(current->children == NULL){
		printf("\tret void\n");
	}
	else{
		convert_LLVM(current->children);
		if(!strcmp(curr_func->type,"void")){
			printf("\tret void\n");
			return;
		}
		if(!strcmp(curr_func->type,"double")){
			if(strcmp(aux->annotate,"double")){
				printf("\t%%%d = sitofp %s %%%d to double\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(curr_func->type,"int")){
			
			if(strcmp(aux->annotate,"int")){
				printf("\t%%%d = sext %s %%%d to i32\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(curr_func->type,"short")){
			if(!strcmp(aux->annotate,"int")){
				printf("\t%%%d = trunc %s %%%d to i16\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
			else if(!strcmp(aux->annotate,"char")){
				printf("\t%%%d = sext %s %%%d to i16\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(curr_func->type,"char")){
			if(strcmp(aux->annotate,"char")){
				printf("\t%%%d = trunc %s %%%d to i8\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		printf("\tret %s %%%d\n",convert_type(curr_func->type),temp_vars-1);
		
	}
}


void func_definition(node * current){
	node * aux, * aux2;
	aux = current->children; // TYPESPEC
	aux2 = aux->brother; //ID
	curr_func = search_function(aux2->value);
	param_list * aux_param = curr_func->parameters;
	char * strng = convert_type(aux->nodeType);
	//printf("oi %s\n",strng);
	printf("define %s @f.%s(",strng,aux2->value);
	//printf("oi %s\n",strng);
	aux = aux2->brother->children; //1st ParamDeclaration
	//aux3 = aux;
	while(aux != NULL){
		aux2 = aux->children;

		if(!strcmp(aux2->nodeType,"Void")) break;
		strng = convert_type(aux2->nodeType);
		printf("%s ",strng);
		if(aux2->brother != NULL){
			
			aux2 = aux2->brother;
			printf("%%%s",aux2->value);
		}
		if(aux->brother != NULL){
			printf(",");
		}
		aux = aux->brother;
	}
	printf("){\n");
	//printf("entry: \n");
	if(strcmp(current->children->nodeType,"Void")){
		if(!strcmp(current->children->nodeType,"Double")){
			printf("\t%%1 = fadd double 0.000000e+00,0.000000e+00\n");
			temp_vars++;
		}
		else{
			printf("\t%%1 = add %s 0,0\n",convert_type(current->children->nodeType));
			temp_vars++;
		}
	}
	while(aux_param != NULL){
		if(!strcmp(aux_param->type,"void")){
			break;
		}
		if(aux_param->id == NULL){
			aux_param = aux_param->next;
			continue;
		}
		if(!strcmp(aux_param->type,"int")){
			 printf("\t%%%d = alloca i32, align 4\n", temp_vars);
			 printf("\tstore i32 %%%s, i32* %%%d, align 4\n",aux_param->id, temp_vars);
			 aux_param->var = temp_vars;
			 temp_vars++;
		}else if(!strcmp(aux_param->type,"short")){
			 printf("\t%%%d = alloca i16, align 2\n", temp_vars);
			 printf("\tstore i16 %%%s, i16* %%%d, align 2\n",aux_param->id, temp_vars);
			 aux_param->var = temp_vars;
			 temp_vars++;
		}else if(!strcmp(aux_param->type,"char")){
			 printf("\t%%%d = alloca i8, align 1\n", temp_vars);
			 printf("\tstore i8 %%%s, i8* %%%d, align 1\n",aux_param->id, temp_vars);
			 aux_param->var = temp_vars;
			 temp_vars++;
		}else if(!strcmp(aux_param->type,"double")){
			 printf("\t%%%d = alloca double, align 8\n", temp_vars);
			 printf("\tstore double %%%s, double* %%%d, align 8\n",aux_param->id, temp_vars);
			 aux_param->var = temp_vars;
			 temp_vars++;
		}
		aux_param = aux_param->next;
	}

	//aux = aux3;

}


void call(node * current){
	char * call_instruction = (char*) malloc(sizeof(char) * 1000);
	//char * aux_str = (char *) malloc(sizeof(char) * 1000);
	//char * aux_str2 = (char * ) malloc(sizeof(char) * 1000);
	//char * parameters = (char *) malloc(sizeof(char) * 1000);
	//sprintf(parameters,"");
	node * aux;
	sym_table * aux_sym = search_function(current->children->value);
	param_list * aux_param = aux_sym->parameters;
	/*
	while(aux_param != NULL){
		strcat(parameters,convert_type(aux_param->type));
		if(aux_param->brother != NULL){
			strcat(parameters,",");
		}
		aux_param = aux_param->next;
	}*/

	aux = current->children->brother;
	//aux_str = convert_type(current->annotate);
	//printf("---> %s\n",aux_str);
	if(!strcmp(current->children->value,"putchar") || !strcmp(current->children->value,"getchar")){
		sprintf(call_instruction,"call %s @%s(",convert_type(current->annotate),current->children->value);
		
	}
	else{

		sprintf(call_instruction,"call %s @f.%s(",convert_type(current->annotate),current->children->value);
	}
	//temp_vars++;
	//printf("%s\n",call_instruction);
	while(aux != NULL){
		convert_LLVM(aux);
		//if(!strcmp(aux_param->type,"void")) break;
		if(!strcmp(aux_param->type,"double")){
			if(strcmp(aux->annotate,"double")){
				printf("\t%%%d = sitofp %s %%%d to double\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(aux_param->type,"int")){
			
			if(strcmp(aux->annotate,"int")){
				printf("\t%%%d = sext %s %%%d to i32\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(aux_param->type,"short")){
			if(!strcmp(aux->annotate,"int")){
				printf("\t%%%d = trunc %s %%%d to i16\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
			else if(!strcmp(aux->annotate,"char")){
				printf("\t%%%d = sext %s %%%d to i16\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		else if(!strcmp(aux_param->type,"char")){
			if(strcmp(aux->annotate,"char")){
				printf("\t%%%d = trunc %s %%%d to i8\n",temp_vars,convert_type(aux->annotate),temp_vars-1);
				temp_vars++;
			}
		}
		
		char * aux_str = (char *) malloc(sizeof(char) * 20);
		sprintf(aux_str,"%s %%%d",convert_type(aux_param->type),temp_vars-1);
		strcat(call_instruction,aux_str);
		free(aux_str);
		if(aux->brother != NULL) strcat(call_instruction,", ");
		aux = aux->brother;
		aux_param = aux_param->next;
	}
	if(strcmp(current->annotate,"void")){

		char * res = (char * ) malloc(sizeof(char) * 1000);
		sprintf(res,"\t%%%d = ",temp_vars);
		temp_vars++;
		strcat(call_instruction,")\n");
		strcat(res,call_instruction);
		printf("%s",res);
		free(res);
	}else{
		printf("\t");
		strcat(call_instruction,")\n");
		printf("%s",call_instruction);
	}
	free(call_instruction);

}

void if_func(node * current){
	node * aux = current->children; //CONDITION
	node * aux2 = aux->brother; //THEN
	node * aux3 = aux2->brother; //ELSE

	int aux_label = label_number;
	label_number++;

	convert_LLVM(aux);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp ne i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;
	if(!strcmp(aux3->nodeType,"Null")){

	    printf("\tbr i1 %%%d, label %%if.%d, label %%finishif.%d\n", temp_vars-1, aux_label, aux_label);
	    printf("if.%d:\n",aux_label);
	    convert_LLVM(aux2);
	    if (!found_ret) printf("\tbr label %%finishif.%d\n",aux_label);
	    printf("finishif.%d: \n",aux_label);
	}
	else{
		printf("\tbr i1 %%%d, label %%if.%d, label %%else.%d\n", temp_vars-1, aux_label, aux_label);
	    printf("if.%d:\n",aux_label);
	    convert_LLVM(aux2);
	    if (!found_ret) printf("\tbr label %%finishif.%d\n",aux_label);
	    printf("else.%d: \n",aux_label);
	    convert_LLVM(aux3);
	    if (!found_ret) printf("\tbr label %%finishif.%d\n",aux_label);
	    printf("finishif.%d: \n",aux_label);
	}


}

void while_func(node * current){
	node * aux = current->children; //CONDITION
	node * aux2 = aux->brother;
	int aux_label = label_number;
	label_number++;

	printf("\tbr label %%while.begin.%d\n",aux_label);
	printf("while.begin.%d:\n",aux_label);
	convert_LLVM(aux);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp ne i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;
	printf("\tbr i1 %%%d, label %%while%d, label %%finishwhile%d\n", temp_vars-1, aux_label, aux_label);
	printf("while%d:\n",aux_label);
	convert_LLVM(aux2);
	if(!found_ret) printf("\tbr label %%while.begin.%d\n",aux_label);
	printf("finishwhile%d:\n",aux_label);

}

void and(node * current){
	node * aux = current->children;
	convert_LLVM(aux);
	int aux_andor = andor_label_number;
	andor_label_number++;
	
	printf("\tbr label %%andor.begin%d\n",aux_andor);
	printf("andor.begin%d:\n",aux_andor);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp ne i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;

	printf("\tbr i1 %%%d, label %%andor.second%d , label %%andor.end%d\n",temp_vars-1,aux_andor, aux_andor);
	printf("andor.second%d: \n",aux_andor);
	aux = aux->brother;
	convert_LLVM(aux);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp ne i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;
	
	printf("\tbr label %%andor.end%d\n",aux_andor);
	printf("andor.end%d:\n",aux_andor);
	if(aux_andor == andor_label_number-1){ // ONLY ONE AND
		printf("\t%%%d = phi i1 [0 , %%andor.begin%d] , [%%%d , %%andor.second%d]\n",temp_vars, aux_andor, temp_vars-1,  aux_andor);
	}
	else{
	
		printf("\t%%%d = phi i1 [0 , %%andor.begin%d] , [%%%d , %%andor.end%d]\n",temp_vars, aux_andor, temp_vars-1, aux_andor + 1);
	}
	temp_vars++;
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars, temp_vars-1);
	temp_vars++;

}


void or(node * current){
	node * aux = current->children;

	convert_LLVM(aux);
	int aux_andor = andor_label_number;
	andor_label_number++;

	
	printf("\tbr label %%andor.begin%d\n",aux_andor);
	printf("andor.begin%d:\n",aux_andor);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp eq i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;
	
	printf("\tbr i1 %%%d, label %%andor.second%d , label %%andor.end%d\n",temp_vars-1,aux_andor, aux_andor);
	printf("andor.second%d: \n",aux_andor);


	aux = aux->brother;
	convert_LLVM(aux);
	if(strcmp(aux->annotate,"int")){
		printf("\t%%%d = sext %s %%%d to i32\n",temp_vars, convert_type(aux->annotate), temp_vars-1);
		temp_vars++;
	}
	printf("\t%%%d = icmp ne i32 %%%d, 0\n", temp_vars, temp_vars-1);
	temp_vars++;
	
	printf("\tbr label %%andor.end%d\n",aux_andor);
	printf("andor.end%d:\n",aux_andor);
	if(aux_andor == andor_label_number-1){ // ONLY ONE AND
		printf("\t%%%d = phi i1 [1 , %%andor.begin%d] , [%%%d , %%andor.second%d]\n",temp_vars, aux_andor, temp_vars-1,  aux_andor);
	}
	else{
		printf("\t%%%d = phi i1 [1 , %%andor.begin%d] , [%%%d , %%andor.end%d]\n",temp_vars, aux_andor, temp_vars-1, aux_andor + 1);
	}
	temp_vars++;
	printf("\t%%%d = zext i1 %%%d to i32\n",temp_vars, temp_vars-1);
	temp_vars++;
	

}




void convert_LLVM(node * current){
	node * aux;
	if(current == NULL) return;
	//printf("---> %s\n",current->nodeType);
	if(!strcmp(current->nodeType,"Null")) return;
	if(!strcmp(current->nodeType,"Program")){
		aux = current->children;
		while(aux != NULL){
			convert_LLVM(aux);
			aux = aux->brother;
		}
	}
	else if(!strcmp(current->nodeType,"FuncBody")){
		aux = current->children;
		while(aux != NULL){
			convert_LLVM(aux);
			if(!strcmp(aux->nodeType,"Return")){
				break;
			}
			aux = aux->brother;
		}
	}
	else if(!strcmp(current->nodeType,"FuncDefinition")){
	//	global_vars = temp_vars;
		temp_vars = 1;
		found_ret = 0;
		func_definition(current);
		aux = current->children->brother->brother->brother;
		convert_LLVM(aux);
		if(!found_ret){
			if(!strcmp(current->children->nodeType,"Void")){

		 		printf("\tret void\n");
			}
			else{
				printf("\tret %s %%1\n",convert_type(current->children->nodeType));
			}
		}
		printf("}\n");
		curr_func = NULL;
	//	temp_vars = global_vars;
		//convert_LLVM(current->brother);
		
	}

	else if(!strcmp(current->nodeType,"FuncDeclaration")){
		//func_declaration(current);
		//convert_LLVM(current->brother);
		//do things
	}

	else if(!strcmp(current->nodeType,"Declaration") && curr_func != NULL){
		//aux = current->children->brother; // ID
		declaration(current);
		//if(curr_func == NULL) convert_LLVM(current->brother);
	}		

	else if(!strcmp(current->nodeType,"StatList")){
		aux = current->children;
		while(aux != NULL){
			convert_LLVM(aux);
			if(!strcmp(aux->nodeType,"Return")) break;
			aux = aux->brother;
		}
		//do things
	}

	else if(!strcmp(current->nodeType,"If")){
		if_func(current);
	}

	else if(!strcmp(current->nodeType,"While")){
		while_func(current);
	}

	else if(!strcmp(current->nodeType,"Return")){
		//do things
		found_ret = 1;
		return_func(current);
	}

	else if(!strcmp(current->nodeType,"Or")){
		or(current);
	}

	else if(!strcmp(current->nodeType,"And")){
		and(current);
	}

	else if(!strcmp(current->nodeType,"Eq")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		eq(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Ne")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		ne(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Lt")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		lt(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Gt")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		gt(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Le")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		le(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Ge")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		ge(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Add")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		add(current,var1,var2);
		
	}

	else if(!strcmp(current->nodeType,"Sub")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		sub(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Mul")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		mul(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Div")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		div_func(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Mod")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		mod(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"Not")){
		aux = current->children;
		convert_LLVM(aux);
		not(current);
	}

	else if(!strcmp(current->nodeType,"Minus")){
		aux = current->children;
		convert_LLVM(aux);
		minus(current);
	}

	else if(!strcmp(current->nodeType,"Plus")){
		aux = current->children;
		convert_LLVM(aux);
		plus(current);
	}

	else if(!strcmp(current->nodeType,"Store")){
		aux = current->children;
		convert_LLVM(aux->brother);
		store(current,temp_vars-1);
		id(aux);
		//do things
	}

	else if(!strcmp(current->nodeType,"Comma")){
		aux = current->children;
		convert_LLVM(aux);
		aux = aux->brother;
		convert_LLVM(aux);
	}

	else if(!strcmp(current->nodeType,"Call")){
		//do things
		call(current);
	}

	else if(!strcmp(current->nodeType,"BitWiseAnd")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		bitwiseand(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"BitWiseOr")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		bitwiseor(current,var1,var2);
	}

	else if(!strcmp(current->nodeType,"BitWiseXor")){
		aux = current->children;
		convert_LLVM(aux);
		int var1 = temp_vars-1;
		aux = aux->brother;
		convert_LLVM(aux);
		int var2 = temp_vars-1;
		bitwisexor(current,var1,var2);
	}
	else if(!strcmp(current->nodeType,"Id")){
		id(current);
	}
	else if(!strcmp(current->nodeType,"IntLit")){
		intlit(current);
	}
	else if(!strcmp(current->nodeType,"RealLit")){
		reallit(current);
	}
	else if(!strcmp(current->nodeType,"ChrLit")){
		chrlit(current);
	}
}

















