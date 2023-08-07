#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtab.h"
#include "tools.h"


typedef struct cell{
  char *name;
  symtabInfo info;
  struct cell *next;
  int time;
}cell;

typedef struct level{
  cell *head;
  struct level *below;
}level;

static void freeRec(cell *ptr);
static int currenttime;
static level *stack;


void initSymtab(void){
  stack=NULL;
  addScopeLevel();
  currenttime=0;

}

void addScopeLevel(void){
  level *temp;
  temp = stack;
  stack= allocMem(sizeof(level),"stack level");
  stack->below=temp;
  stack->head=NULL;
  //  printf("addlevel\n");
}

void removeScopeLevel(void){
  level *temp;
  if (stack==NULL) internalError("bad call to removeScopeLevel");
  temp=stack;
  freeRec(stack->head);
  stack=stack->below;
  free(temp);
  // printf("removelevel\n");
}

void freeRec(cell *ptr){
  if (ptr==NULL) return;
  freeRec(ptr->next);
  free(ptr);
  // printf("freeRec\n");
}


void insertInSymtab(char *name, symtabInfo val){
  cell *temp;
  temp=allocMem(sizeof(cell),"cell");
  temp->info=val;
  temp->name=copyString(name);
  temp->next=stack->head;
  temp->time=-1;
  stack->head= temp;
  
 
      
  //  printf("insertin %s\n",name);
}

int definedInCurrentLevel(char *name){
  cell* temp;
  temp=stack->head;
  while(temp!=NULL){
    if (!strcmp(temp->name,name))
      return 1;
    temp=temp->next;
  }
  // printf("definedin %s\n",name);
  return 0;
}


int searchSymtab(char *name, symtabInfo *pVal){
  cell* temp;
  level* l1;
  l1=stack; 
  while(l1!=NULL){
    temp=l1->head;
    while(temp!=NULL){
      if (!strcmp(temp->name,name)){
	currenttime++;
	temp->time=currenttime;
	*pVal= temp->info;
	return 1;
      }
      temp=temp->next;
    }
    l1=l1->below;
  }
  // printf("search %s\n",name);
  return 0;
}

int getCurrentTime(void){
  currenttime++;
  return currenttime;
}

int getLastLookupTime(char *name){
  cell* temp;
  level* l1;
  l1=stack;

  while(l1!=NULL){
    temp=l1->head;
    while(temp!=NULL){
      if (!strcmp(temp->name,name))
	return temp->time;
      temp=temp->next;
    }
    l1=l1->below;
  }

  return -1;
}
