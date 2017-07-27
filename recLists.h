#ifndef _recLists_h
#define _recLists_h

void initProcList(void);
void addToProcList(procRec *p);
procListRec *getProcList(void);

void initVarList(void);
void addToVarList(varInfoRec *var);
varListRec *getVarList(void);

#endif
