%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "header.h"
    int yylex(void);
    void yyerror (const char *s);
    
    node* root = NULL;
    node* aux = NULL;
    node* aux1 = NULL;
    node* aux2 = NULL;
    node* aux3 = NULL;
    int nerros = 0;
    int first = 0;
    
%}


%token SEMI RBRACE LBRACE UNARY RPAR LPAR RESERVED 

%token <tok> ID INTLIT CHRLIT REALLIT RETURN IF ELSE WHILE ASSIGN OR AND BITWISEXOR COMMA BITWISEOR BITWISEAND EQ NE LE LT GT GE PLUS MINUS MUL DIV MOD NOT VOID  CHAR INT  SHORT DOUBLE 

%nonassoc NO_ELSE
%nonassoc ELSE

%left COMMA
%right ASSIGN
%left OR
%left AND
%left BITWISEOR
%left BITWISEXOR
%left BITWISEAND
%left EQ NE
%left LE LT GT GE 
%left PLUS MINUS
%left MUL DIV MOD
%right UNARY NOT
%left RPAR LPAR



%type <node> functions_and_declarations function_definition function_body declarations_and_statements function_declaration function_declarator parameter_list parameter_declaration declaration aux_declaration type_spec declarator statement aux_statement expr logical_or_expr logical_and_expr relation_expr additive_expression mult_expression unary_expression function_expression primary_expression error_statement  aux_call  assignment_expression equality_expr and_expr exclusive_or_expr inclusive_or_expr 



%union{
    struct node* node;
    struct token * tok;
}





%%

start: functions_and_declarations 	{root = createnode("Program",NULL,0,0);
					addChild(root,$1);}
	;

functions_and_declarations:
	function_definition  		{$$ = $1;}

	| function_declaration     	{$$ = $1;}
													
	| declaration     		{$$ = $1;}
													
	|functions_and_declarations function_definition    	{$$ = $1; addBro($1,$2);}
																				
	|functions_and_declarations  function_declaration   	{$$ = $1; addBro($1,$2);} 


	|functions_and_declarations  declaration    		{$$ = $1; addBro($1,$2);}
	;


function_definition: 					
	type_spec function_declarator function_body   	{$$ = createnode("FuncDefinition",NULL,0,0);
							addChild($$,$1);
							addBro($1,$2);
							addBro($1,$3);}
	;




function_body:						
	LBRACE RBRACE					{$$ = 	createnode("FuncBody",NULL,0,0);}

	| LBRACE declarations_and_statements RBRACE    	{$$ = 	createnode("FuncBody",NULL,0,0);
							addChild($$,$2);}	
	;

declarations_and_statements: 				
	 declarations_and_statements statement  	{$$ = $1; addBro($$,$2);}

	|  declarations_and_statements declaration  	{$$ = $1; addBro($$,$2);}

	|statement     				{ $$ = $1;}

	|declaration    			{$$ = $1;}
							
	;


function_declaration: 					
	type_spec function_declarator SEMI 	{$$ = createnode("FuncDeclaration",NULL,0,0);
						addChild($$,$1);
						addBro($1,$2);}
	; 


function_declarator: 					
	ID LPAR parameter_list RPAR	{
					
					$$ = createnode("Id",$1->value,$1->nline,$1->ncol);
					free($1);
					aux = createnode("ParamList",NULL,0,0);
					addChild(aux,$3);
					addBro($$,aux);					
					
					
					}

	;




parameter_list: 					
	parameter_declaration   	{$$ = $1;}

	| parameter_list  COMMA   parameter_declaration {$$ = $1; addBro($1,$3); }
	;


parameter_declaration: 
	type_spec       	{$$=createnode("ParamDeclaration",NULL,0,0);
				
				addChild($$,$1);}

	| type_spec ID  	{$$=createnode("ParamDeclaration",NULL,0,0);
				
				addChild($$,$1);
				
				addBro($1,createnode("Id",$2->value,$2->nline,$2->ncol));
				}

	
	;


declaration: 
	


	 type_spec aux_declaration SEMI 	{aux = $2;
						first = 0;
						while(aux != NULL){
							aux1 = aux;
							
							aux = aux->brother;
							aux1->brother = NULL;
							aux2 = createnode("Declaration",NULL,0,0);
							aux3 = createnode($1->nodeType,NULL,$1->nline,$1->ncol);
							addChild(aux2,aux3);
							if(aux1->children != NULL){
								aux1->brother = aux1->children;
								aux1->children = NULL;							
							}
							addBro(aux3,aux1);
							if(!first){
								first = 1;							
								 $$ = aux2;
							}
							else addBro($$,aux2);
							
						
							}
						free($1);
						}
		
	| error SEMI   			{$$ = createnode("Null",NULL,0,0); nerros += 1;}
	
	;



aux_declaration: 
	declarator   			{$$ = $1; }

	| aux_declaration COMMA declarator 	{$$ = $1; addBro($1,$3);}
	;


type_spec: 	
	CHAR   		{$$ = createnode("Char",NULL,$1->nline,$1->ncol);}
		 
	| INT  		{$$ = createnode("Int",NULL,$1->nline,$1->ncol);   
			}

	| VOID    	{$$ = createnode("Void",NULL,$1->nline,$1->ncol);   
			}
	
	| SHORT    	{$$ = createnode("Short",NULL,$1->nline,$1->ncol);}
	
	| DOUBLE    	{$$ = createnode("Double",NULL,$1->nline,$1->ncol);}
	;

declarator: 	
	ID	  		{$$ = createnode("Id",$1->value,$1->nline,$1->ncol);}


	|  ID ASSIGN expr   	{$$ = createnode("Id",$1->value,$1->nline,$1->ncol);
				addChild($$,$3);}

	;

error_statement:
	error SEMI  	{$$ = createnode("Null",NULL,0,0); nerros += 1;}

	| statement     {$$=$1;}
	
	;



statement: 	
	SEMI				{$$ = createnode("Null",NULL,0,0);}
		 
	| expr SEMI    			{$$=$1;}
		
	| LBRACE RBRACE  		{$$ = createnode("Null",NULL,0,0);}
		 
	| LBRACE error RBRACE   	{$$ = createnode("Null",NULL,0,0); nerros += 1;}
		 
	| LBRACE aux_statement RBRACE   { if(countBros($2) >= 2){
						$$=createnode("StatList",NULL,0,0);
						addChild($$,$2);}
					else{
						$$ = $2;
					}}
		 
	| IF LPAR expr RPAR error_statement %prec NO_ELSE   		{ 
			 						$$=createnode("If",NULL,$1->nline,$1->ncol);
									addChild($$,$3);
									if($5 == NULL){
										addBro($3,createnode("Null",NULL,0,0));		
									}								
									else addBro($3,$5);
									addBro($3,createnode("Null",NULL,0,0));			
			 						 }
	

	| IF LPAR expr RPAR error_statement ELSE error_statement 	{ 
			 						$$=createnode("If",NULL,$1->nline,$1->ncol);
									addChild($$,$3);
									if($5 == NULL){
										addBro($3,createnode("Null",NULL,0,0));		
									}								
									else addBro($3,$5);
									if($7 == NULL){
										addBro($3,createnode("Null",NULL,0,0));		
									}								
									else addBro($3,$7);	
			 						 }


	| WHILE LPAR expr RPAR error_statement       			{ 
			 						$$=createnode("While",NULL,$1->nline,$1->ncol);
									addChild($$,$3);
									if($5 == NULL){
										addBro($3,createnode("Null",NULL,0,0));		
									}								
									else addBro($3,$5);
										
			 						 }


	| RETURN SEMI						{$$ = createnode("Return",NULL,$1->nline,$1->ncol); addChild($$,createnode("Null",NULL,0,0));}
	 

	| RETURN expr SEMI					{$$=createnode("Return",NULL,$1->nline,$1->ncol);
		 						addChild($$,$2);}
		 
	;




aux_statement: 						
	aux_statement error_statement     	{$$ = $1;
						addBro($1,$2);}
							
	| error_statement			{$$=$1;}


	;


expr:
	assignment_expression				{$$=$1;}

	| expr COMMA assignment_expression		{ $$=createnode("Comma",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);}
	;

assignment_expression:
	logical_or_expr ASSIGN assignment_expression 	{ $$=createnode("Store",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);}    		

	| logical_or_expr				{$$=$1;}

	;


unary_expression: 					
	PLUS unary_expression %prec UNARY   		{$$=createnode("Plus",$1->value,$1->nline,$1->ncol);
							addChild($$,$2);}
							

	| MINUS unary_expression %prec UNARY    	{$$=createnode("Minus",$1->value,$1->nline, $1->ncol);
							addChild($$,$2);}
							

	| NOT unary_expression    			{$$=createnode("Not",$1->value,$1->nline,$1->ncol);
							addChild($$,$2);}
							
	| function_expression		{$$=$1;}

	;

	

logical_or_expr: 					
	logical_or_expr OR logical_and_expr  			{$$=createnode("Or",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);	}

	| logical_and_expr	{$$=$1;}

	

	;


logical_and_expr: 					
	logical_and_expr AND inclusive_or_expr 			{$$=createnode("And",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);	}
	

	| inclusive_or_expr		{$$=$1;}

	


	;

inclusive_or_expr:
	 inclusive_or_expr BITWISEOR exclusive_or_expr		{$$=createnode("BitWiseOr",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}

	| exclusive_or_expr		{$$=$1;}

	;

exclusive_or_expr:
	exclusive_or_expr BITWISEXOR and_expr   	{$$=createnode("BitWiseXor",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);}

	| and_expr			{$$=$1;}

	;

and_expr:
	and_expr BITWISEAND equality_expr   		{$$=createnode("BitWiseAnd",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);	}

	| equality_expr	{$$=$1;}

	;	

equality_expr:
	equality_expr EQ relation_expr  		{$$=createnode("Eq",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);}
					
	| equality_expr NE relation_expr		{$$=createnode("Ne",$2->value,$2->nline,$2->ncol);
							addChild($$,$1);
							addBro($1,$3);	}

	| relation_expr		{$$=$1;}

	;


relation_expr: 						
	relation_expr LT additive_expression      		{$$=createnode("Lt",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);	}

	| relation_expr GT additive_expression   		{$$=createnode("Gt",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);	}

	| relation_expr GE additive_expression   		{$$=createnode("Ge",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
				
	| relation_expr LE additive_expression   		{$$=createnode("Le",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
					
	| additive_expression	{$$=$1;}

	;

additive_expression: 					
	additive_expression PLUS mult_expression   		{$$=createnode("Add",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);	}
	
	| additive_expression MINUS mult_expression   		{$$=createnode("Sub",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
							

	| mult_expression	{$$=$1;}
	;


mult_expression: 					
	mult_expression MUL unary_expression    		{$$=createnode("Mul",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
							

	| mult_expression DIV unary_expression   		{$$=createnode("Div",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
							

	| mult_expression MOD unary_expression   		{$$=createnode("Mod",$2->value,$2->nline,$2->ncol);
								addChild($$,$1);
								addBro($1,$3);}
							

	| unary_expression		{$$=$1;}
	;


function_expression: 					
	

	 ID LPAR RPAR			{$$ = createnode("Call",NULL,$1->nline,$1->ncol);
					aux = createnode("Id",$1->value,$1->nline,$1->ncol);
					addChild($$,aux);}


	| ID LPAR aux_call RPAR   				{$$ = createnode("Call",NULL,$1->nline,$1->ncol);
								aux = createnode("Id",$1->value,$1->nline,$1->ncol);					
								addBro(aux,$3);	addChild($$,aux);}

	| ID LPAR error RPAR  		{$$ = createnode("Null",NULL,0,0); nerros += 1;}

	| primary_expression		{$$=$1;}
	
	;


aux_call:
	assignment_expression		{$$=$1;}

	| aux_call COMMA assignment_expression {addBro($1,$3); $$=$1;}

	;

primary_expression:
	ID 				{$$ = createnode("Id",$1->value,$1->nline,$1->ncol);}
		
	| INTLIT  			{ $$ = createnode("IntLit",$1->value,$1->nline,$1->ncol);}
					
	| CHRLIT  			{$$ = createnode("ChrLit",$1->value,$1->nline,$1->ncol);}

	| REALLIT   			{$$ = createnode("RealLit",$1->value,$1->nline,$1->ncol);}
							

	|LPAR error RPAR     		{$$ = createnode("Null",NULL,0,0); nerros += 1;}


	| LPAR expr RPAR 		{$$=$2;}

	;




