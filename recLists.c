#include "syntaxTree.h"
#include "check.h"
#include "recLists.h"

static procListRec *procListHead;
static procListRec *procListTail;

static varListRec *varListHead;
static varListRec *varListTail;

void initProcList(void)
{
    procListHead = NULL;
    procListTail = NULL;
}

void addToProcList(procRec *p)
{
    procListRec *temp;
    temp = makeProcListRec(p, NULL);
    if (procListHead == NULL)
	procListHead = procListTail =temp;
    else
	procListTail = procListTail->next = temp;
}

procListRec *getProcList(void)
{
    return procListHead;
}

void initVarList(void)
{
    varListHead = NULL;
    varListTail = NULL;
}

void addToVarList(varInfoRec *var)
{
    varListRec *temp;
    temp = makeVarListRec(var, NULL);
    if (varListHead == NULL)
	varListHead = varListTail =temp;
    else
	varListTail = varListTail->next = temp;
}

varListRec *getVarList(void)
{
    return varListHead;
}
