#include "syntaxTree.h"
#include "check.h"
#include "typeinfo.h"
#include "symtab.h"
#include "checkerTools.h"

void putTypeIntoSymtab(char *name, typeRec *tRec)
{
    symtabInfo s;
    s.kind = TYPERECPOINTER;
    s.item.t = tRec;
    insertInSymtab(name, s);
}

void putVarIntoSymtab(char *name, varInfoRec *vRec)
{
    symtabInfo s;
    s.kind = VARRECPOINTER;
    s.item.v = vRec;
    insertInSymtab(name, s);
}

void putProcIntoSymtab(char *name, procRec *pRec)
{
    symtabInfo s;
    s.kind = PROCRECPOINTER;
    s.item.p = pRec;
    insertInSymtab(name, s);
}

void addToProcList(procRec *p, procListRec **pLst)
{
    procListRec *temp;
    if (*pLst == NULL)
	*pLst = makeProcListRec(p, NULL);
    else {
	temp = *pLst;
	while (temp->next != NULL)
	    temp = temp->next;
	temp->next = makeProcListRec(p, NULL);
    }
}

void addToVarList(varInfoRec *v, varListRec **pVLst)
{
    varListRec *temp;
    if (*pVLst == NULL)
	*pVLst = makeVarListRec(v, NULL);
    else {
	temp = *pVLst;
	while (temp->next != NULL)
	    temp = temp->next;
	temp->next = makeVarListRec(v, NULL);
    }
}
