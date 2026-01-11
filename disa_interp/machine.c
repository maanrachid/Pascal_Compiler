#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcodes.h"
#include "machine.h"


void checkline(FILE *f, char *s, int lnum);
void getnum(FILE *f, int *num, int lnum);
void checkeof(FILE *f);
void readerr(int linenum);
void loadprog(char *s);

int codeSize, stringSize, globalVarSpace, startAddr;
int code[CODEMEM];
either_type st[STACKMAX];

int main(int argc, char **argv)
{
    int i;

    if (argc != 2) {
        fprintf(stderr, "interp: Usage: interp file\n");
        exit(1);
    }

    loadprog(argv[1]);

    /* To build the disassembler, give command-line option -DDISASM to the compiler. */
#ifdef DISASM
    disassemble(codeSize, stringSize, globalVarSpace, startAddr);
#endif

#ifndef DISASM
    interpret(stringSize, globalVarSpace, startAddr);
#endif
}

void checkline(FILE *f, char *s, int lnum)
{
    char arr[100];
    if (fgets(arr, 100, f) == NULL){
      printf("asda\n");
        readerr(lnum);
    }
    if (strcmp(arr, s)) {
        readerr(lnum);
    }
}

void getnum(FILE *f, int *num, int lnum)
{
    char arr[100];
    if (fgets(arr, 100, f) == NULL){
      readerr(lnum);
    }
    if (sscanf(arr, "%d", num) != 1) {
        printf("%s\n", arr);
        readerr(lnum);
    }
}

void checkeof(FILE *f)
{
    if (fgetc(f) != EOF) {
        fprintf(stderr, "Program file contains more lines than expected.\n");
        exit(1);
    }
}

void readerr(int linenum)
{
    fprintf(stderr, "Error reading code file on line %d.\n", linenum);
    exit(1);
}

void loadprog(char *s)
{
    FILE *f;
    int i;
    int linenum;

    f = fopen(s, "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR--file %s could not be opened.\n", s);
        exit(1);
    }

    linenum = 1;
    checkline(f, "***#!@&\n", linenum++);
    getnum(f, &codeSize, linenum++);
    getnum(f, &stringSize, linenum++);
    getnum(f, &globalVarSpace, linenum++);
    getnum(f, &startAddr, linenum++);
    checkline(f, "*****\n", linenum++);
    for (i = 0; i < codeSize; i++)
        getnum(f, &code[i], linenum++);
    checkline(f, "*****\n", linenum++);
    for (i = 0; i < stringSize; i++)
        getnum(f, &st[i].i, linenum++);
    checkeof(f);

    fclose(f);
}
