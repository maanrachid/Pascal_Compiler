#ifndef _symtab_h
#define _symtab_h
#include "typeinfo.h"

void initSymtab(void);
void addScopeLevel(void);
void removeScopeLevel(void);
void insertInSymtab(char *name, symtabInfo val);
int definedInCurrentLevel(char *name);
int searchSymtab(char *name, symtabInfo *pVal);
int getCurrentTime(void);
int getLastLookupTime(char *name);

/****************************************************************

This symbol table implementation enables a program to ask whether a
given identifier has been used (in an applied occurrence, not a
defining occurence) since a specified point in time.

Such questions can be dealt with as follows:

A client program can call getCurrentTime to find out what the symbol
table considers to be the current time.  It can (later) call
getLastLookupTime  to learn the time at which the last successful
lookup (using searchSymtab) of a given identifier occurred.  It can
then compare the values so obtained.

The value that the symbol table considers to be the current time
is incremented when  getCurrentTime  is called (the incremented
value is returned) and just before the time at which a successful
search is noted in the symbol table by  searchSymtab.
The function initSymtab initializes the current time.  No other
function modifies the current time.

****************************************************************/

#endif
