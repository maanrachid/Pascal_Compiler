#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"

void programError(char *msg, int location)
{
  printf("%d \"%s\"\n", location, msg);
  exit(1);
}


void errorInInputToCompiler(char *msg, int location)
{
    fprintf(stderr, "%d \"%s\"\n", location, msg);
    exit(1);
}


void internalError(char *msg)
{
    fprintf(stderr, "-1 \"Internal error--%s\"\n", msg);
    exit(1);
}

void miscError(char *msg1, char *msg2)
{
    fprintf(stderr, "-1 \"Error--%s", msg1);
    if (msg2 != NULL)
	fprintf(stderr, "%s", msg2);
    fprintf(stderr, "\"\n");
    exit(1);
}

void *allocMem(int size, char *errLocation)
{
    void *pt;
    pt = malloc(size);
    if (pt == NULL)
	miscError("Out of memory in ", errLocation);
    return pt;
}

char *copyString(char *s)
{
    char *pt;
    pt = allocMem(strlen(s) + 1, "copyString");
    strcpy(pt, s);
    return pt;
}
