#ifndef _typeinfo_h
#define _typeinfo_h
#include "check.h"

typedef enum { TYPERECPOINTER, VARRECPOINTER, PROCRECPOINTER } entryType;

typedef struct {
    entryType kind;
    union {
	typeRec *t;
	varInfoRec *v;
	procRec *p;
    } item;
} symtabInfo;

#endif
