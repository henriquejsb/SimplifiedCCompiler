 
#include "header.h"


 
token * createtoken(char * value, int nline, int ncol){
	token * res = (token *)malloc(sizeof(token));
	if(value != NULL) res->value = strdup(value);
	res->nline = nline;
	res->ncol = ncol;
	return res;
}


node* createnode(char* nodeType,char* value, int nline, int ncol){
	
	
	//printf("A criar nó:%s - %s\n",nodeType, value);
	node* novo =(node*)malloc(sizeof(node));
	novo->children = NULL;
	novo->brother = NULL;
	if(novo==NULL){
		return novo;
	}
	novo->nodeType = strdup(nodeType);
	novo->annotate = NULL;
	if(value != NULL){
		novo->value = strdup(value);
	}else{
		novo->value = NULL;
	}
	novo->nline = nline;
	novo->ncol = ncol;

	
	//printf("Fim do criar nó\n");
	return novo;
}



void addBro(node* old, node* new){
	//printf("[Adicionar Irmao]  ");
	//printf(" || old=%s || new= %s\n",old->nodeType,new->nodeType);
	if(old== NULL || new == NULL){
		return;
	}
	node *aux = old;
	while(aux->brother !=NULL){
		aux = aux->brother;
	}
	aux->brother = new;

}




void addChild(node* father, node* aux){
	//printf("[Adicionar Filho]  ");
	//printf(" || pai=%s || new= %s\n",father->nodeType,aux->nodeType);
	if(father == NULL || aux == NULL){
		return;
	}
	father-> children = aux;
}




void printTree(node* atual, int nivel){
	//printf("print\n");
	for(int i = 0; i < nivel * 2; i++) printf(".");
	
	if(strcmp(atual->nodeType,"Id") == 0 || strcmp(atual->nodeType,"IntLit") == 0 || strcmp(atual->nodeType,"ChrLit") == 0 || strcmp(atual->nodeType,"RealLit") == 0){
		printf("%s(%s)",atual->nodeType,atual->value);
	}
	else{
		printf("%s",atual->nodeType);
	}
	if(atual->annotate != NULL) printf(" - %s",atual->annotate);
	//printf(" -%d - %d\n",atual->nline, atual->ncol);
	printf("\n");
	if(atual->children != NULL){
		printTree(atual->children, nivel + 1);

	}
	if(atual->brother != NULL){
		
		printTree(atual->brother, nivel);
	}

		

}


int countBros(node * atual){
	int conta = 0;
	node * aux = atual;
	while(aux != NULL){
		aux = aux->brother;
		conta += 1;
	}
	return conta;
}


void clearTree(node * atual){

	if(atual->children != NULL) clearTree(atual->children);
	if(atual->brother != NULL) clearTree(atual->brother);
	if(atual->annotate != NULL){
		free(atual->annotate);
		atual->annotate = NULL;
	}
	free(atual->nodeType);
	atual->nodeType = NULL;
	if(atual->value != NULL){
		free(atual->value);
		atual->value = NULL;
	}
	free(atual);

}