#ifndef _scan_h
#define _scan_h

#include <stdio.h>

typedef int tokenType;

typedef struct {
    int firstChar;
    int lastChar;
} locRec;

typedef struct {
    locRec location;
    tokenType tType;
    char *spell;
} tokenRec;

tokenRec *getTokenFromScanner(void);

/*
void initScan(FILE *inFileParam);

tokenRec *gTok(void);
*/


#endif
