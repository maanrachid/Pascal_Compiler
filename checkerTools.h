#ifndef _checkerTools_h
#define _checkerTools_h

void putTypeIntoSymtab(char *name, typeRec *tRec);
void putVarIntoSymtab(char *name, varInfoRec *vRec);
void putProcIntoSymtab(char *name, procRec *pRec);
void addToProcList(procRec *p, procListRec **pLst);
void addToVarList(varInfoRec *v, varListRec **pVLst);

#endif
