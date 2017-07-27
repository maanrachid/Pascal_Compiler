#include <stdio.h>
#include "scan.h"
#include "syntaxTree.h"
#include "check.h"
#include "codeDetails.h"
#include "tools.h"
#include "opcodes.h"

#define STRMAX 1000

enum codeRecKind {LABELREC, MARKORCALLREC, JUMPREC, SIMPLEREC};

typedef struct codeRec {
    enum codeRecKind kind;
    int opcode;
    int numOps;
    int op1;
    int op2;
    int op3;
    struct codeRec *target;
    procRec *theProc;
    int addr;
    struct codeRec *next;
} codeRec;

int codeSize, stringSize, globalVarSpace, startAddr;
int strMem[STRMAX];

static codeRec *makeCodeRec(void);
static void insertCodeRecNode(codeRec *rec);
int handleCodeRecList(codeRec *rec, int initAddr, FILE *outFile);
void printHeader(FILE *outFile);

static codeRec *codeListHead, *codeListTail;

static codeRec *makeCodeRec(void)
{
    codeRec *rec;
    rec = allocMem(sizeof(codeRec), __func__);
    rec->opcode = -1;
    rec->numOps = -1;
    rec->op1 = -1;
    rec->op2 = -1;
    rec->op3 = -1;
    rec->target = NULL;
    rec->theProc = NULL;
    rec->addr = -1;
    rec->next = NULL;
    return rec;
}

static void insertCodeRecNode(codeRec *rec)
{
    if (codeListHead == NULL) {
	codeListHead = rec;
	codeListTail = rec;
    } else {
	codeListTail->next = rec;
	codeListTail = rec;
    }
    rec->next = NULL;      /* for safety--currently redundant */
}

struct codeRec *makeNewLabel(void)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = LABELREC;
    return rec;
}

void emitLabel(struct codeRec *theLabel)
{
    insertCodeRecNode(theLabel);
}

void emitMarkOrCall(int opcode, procRec *theProc)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = MARKORCALLREC;
    rec->opcode = opcode;
    rec->theProc = theProc;
    insertCodeRecNode(rec);
}

void emitJump(int opcode, struct codeRec *target)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = JUMPREC;
    rec->opcode = opcode;
    rec->target = target;
    insertCodeRecNode(rec);
}

void emitInstr0(int opcode)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = SIMPLEREC;
    rec->opcode = opcode;
    rec->numOps = 0;
    insertCodeRecNode(rec);
}

void emitInstr1(int opcode, int operand1)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = SIMPLEREC;
    rec->opcode = opcode;
    rec->numOps = 1;
    rec->op1 = operand1;
    insertCodeRecNode(rec);
}

void emitInstr2(int opcode, int operand1, int operand2)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = SIMPLEREC;
    rec->opcode = opcode;
    rec->numOps = 2;
    rec->op1 = operand1;
    rec->op2 = operand2;
    insertCodeRecNode(rec);
}

void emitInstr3(int opcode, int operand1, int operand2, int operand3)
{
    codeRec *rec;
    rec = makeCodeRec();
    rec->kind = SIMPLEREC;
    rec->opcode = opcode;
    rec->numOps = 3;
    rec->op1 = operand1;
    rec->op2 = operand2;
    rec->op3 = operand3;
    insertCodeRecNode(rec);
}

void initCodeList(void)
{
    codeListHead = NULL;
    codeListTail = NULL;
}

struct codeRec *getCodeList(void)
{
    return codeListHead;
}

void calculateCodeAddrs(procListRec *procs)
{
    procListRec *lst;
    procRec *theProc;
    int addr;
    addr = 0;
    lst = procs;
    while (lst != NULL) {
	theProc = lst->theProc;
	theProc->entryPoint = addr;
	addr = handleCodeRecList(theProc->code, addr, NULL);
	lst = lst->next;
	if (strcmp(theProc->name, "") == 0)
	    globalVarSpace = theProc->varSpace;
    }
    codeSize = addr;
    startAddr = 0;
}

void printHeader(FILE *outFile)
{
    fprintf(outFile, "***#!@&\n");
    fprintf(outFile, "%d\n", codeSize);
    fprintf(outFile, "%d\n", stringSize);
    fprintf(outFile, "%d\n", globalVarSpace);
    fprintf(outFile, "%d\n", startAddr);
    fprintf(outFile, "*****\n");
}

void sendCodeToFile(procListRec *procs, FILE *outFile)
{
    procListRec *lst;
    procRec *theProc;
    int i;
    printHeader(outFile);
    lst = procs;
    while (lst != NULL) {
	theProc = lst->theProc;
	handleCodeRecList(theProc->code, 0, outFile);
	lst = lst->next;
    }
    fprintf(outFile, "*****\n");
    for (i = 0; i < stringSize; i++)
	fprintf(outFile, "%d\n", strMem[i]);
}

void initStringMem(void)
{
    stringSize = 0;
}

int getNextStringAddr(void)
{
    return stringSize;
}

void insertStringChar(int ch)
{
    if (stringSize >= STRMAX - 1)
	miscError("out of space for strings", NULL);
    else
	strMem[stringSize++] = ch;
}

int handleCodeRecList(codeRec *rec, int initAddr, FILE *outFile)
{
    int nextAddr;
    nextAddr = initAddr;
    while (rec != NULL) {
	if (rec->kind == LABELREC) {
	    if (outFile == NULL)
		rec->addr = nextAddr;
	    rec = rec->next;
	    continue;
	}
	switch (rec->opcode) {
	    case QUIT:        /*  No-operand instructions */
	    case RETPROC:
	    case GETINDIRECT:
	    case NOTOP:
	    case NEGINT:
	    case STORE:
	    case EQREAL:
	    case NEQREAL:
	    case LTREAL:
	    case LEQREAL:
	    case GTREAL:
	    case GEQREAL:
	    case EQINT:
	    case NEQINT:
	    case LTINT:
	    case LEQINT:
	    case GTINT:
	    case GEQINT:
	    case OROP:
	    case ADDINT:
	    case SUBINT:
	    case ADDREAL:
	    case SUBREAL:
	    case ANDOP:
	    case MULINT:
	    case DIVINT:
	    case MODINT:
	    case MULREAL:
	    case DIVREAL:
	    case READLNOP:
	    case WRITELNOP:
	    case NEGREAL:
	    case READCHAR:
	    case READINT:
	    case READREAL:
	    case WRITECHAR:
	    case WRITEINT:
	    case WRITEREAL2:
	    case WRITESTRING:
	    case WRITEREAL3:
	    case CHRFCN:
	    case ORDFCN:
	    case ROUNDFCN:
	    case TRUNCFCN:
	    case SQRTFCN:
	    case EOFFCN:
	    case EOLNFCN:
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		}
		nextAddr += 1;
		break;
	    case OFFSET:      /*  One-operand instructions */
	    case LOADBLOCK:
	    case COPYBLOCK:
	    case LOADIMMED:
	    case FLOAT:
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->op1);
		}
		nextAddr += 2;
		break;
	    case LDADDR:      /*  Two-operand instructions */
	    case LDVAL:
	    case LDINDIRECT:
	    case FIXDISPLAY:
	    case INDEX1:
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->op1);
		    fprintf(outFile, "%d\n", rec->op2);
		}
		nextAddr += 3;
		break;
	    case INDEX:      /*  Three-operand instructions */
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->op1);
		    fprintf(outFile, "%d\n", rec->op2);
		    fprintf(outFile, "%d\n", rec->op3);
		}
		nextAddr += 4;
		break;
	    case MARK:        /*  Mark instruction */
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->theProc->paramSpace);
		}
		nextAddr += 2;
		break;
	    case CALL:        /*  Call instruction */
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->theProc->entryPoint);
		    fprintf(outFile, "%d\n", rec->theProc->paramSpace);
		    fprintf(outFile, "%d\n", rec->theProc->varSpace);
		    fprintf(outFile, "%d\n", rec->theProc->level);
		}
		nextAddr += 5;
		break;
	    case JMP:         /*  Jump instructions */
	    case JMPIFFALSE:
		if (outFile == NULL)
		    rec->addr = nextAddr;
		else {
		    fprintf(outFile, "%d\n", rec->opcode);
		    fprintf(outFile, "%d\n", rec->target->addr);
		}
		nextAddr += 2;
		break;
	    default:
		internalError("unrecognized opcode in handleCodeRecList");
		break;
	}
	rec = rec->next;
    }
    return nextAddr;
}
