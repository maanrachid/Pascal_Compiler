#include "tools.h"
#include "syntaxTree.h"
#include "check.h"
#include "checkerGlobals.h"
#include <string.h>


static typeRec *makeGENERALType(char *caller);
static varInfoRec *makeGENERALVarInfoRec(char *caller);
static procRec *makeGENERALProc(char *caller);
static exprData *makeGENERALExprData(char *caller);

/* *********************************************************************** */
/*                             Type denoters                               */
/* *********************************************************************** */

static typeRec *makeGENERALType(char *caller)
{
    typeRec *rec;
    rec = allocMem(sizeof(typeRec), caller);
    rec->lowBound = 0;
    rec->highBound = 0;
    rec->componentType = NULL;
    rec->fi = NULL;
    rec->size = -1;
    return rec;
}

typeRec *makeSimpleType(typeType tt)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    typeRec *rec = makeGENERALType(nonConstPtr);
    rec->tt = tt;
    return rec;
}

typeRec *makeArrayType(int lowBound, int highBound, typeRec *componentType)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    typeRec *rec = makeGENERALType(nonConstPtr);
    rec->tt = ARRAYTYPE;
    rec->lowBound = lowBound;
    rec->highBound = highBound;
    rec->componentType = componentType;
    return rec;
}

typeRec *makeRecordType(struct fieldInfoRec *fi)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    typeRec *rec = makeGENERALType(nonConstPtr);
    rec->tt = RECORDTYPE;
    rec->fi = fi;
    return rec;
}

fieldInfoRec *makeFieldInfoRec(char *fieldName, typeRec *fieldType,
			       fieldInfoRec *next)
{
    fieldInfoRec *rec;
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    rec = allocMem(sizeof(fieldInfoRec), nonConstPtr);
    rec->fieldName = fieldName;
    rec->fieldType = fieldType;
    rec->size = 0;
    rec->offset = 0;
    rec->next = next;
    return rec;
}

/* *********************************************************************** */
/*               Variable (and parameter) declarations                     */
/* *********************************************************************** */

static varInfoRec *makeGENERALVarInfoRec(char *caller)
{
    varInfoRec *rec;
    rec = allocMem(sizeof(varInfoRec), caller);
    rec->name = NULL;
    rec->vType = NULL;
    rec->vLevel = -1;  /* Replace this in each case  */
    rec->vSize = 0;
    rec->vOffset = 0;
/*  rec->inWhichProc = NULL; */
    return rec;
}

varInfoRec *makeLocalVarInfoRec(char *name, typeRec *vType, int vLevel)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    varInfoRec *rec = makeGENERALVarInfoRec(nonConstPtr);
    rec->vKind = LOCALVAR;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varInfoRec *makeValueParamInfoRec(char *name, typeRec *vType, int vLevel)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    varInfoRec *rec = makeGENERALVarInfoRec(nonConstPtr);
    rec->vKind = VALUEPARAM;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varInfoRec *makeVarParamInfoRec(char *name, typeRec *vType, int vLevel)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    varInfoRec *rec = makeGENERALVarInfoRec(nonConstPtr);
    rec->vKind = VARPARAM;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varListRec *makeVarListRec(varInfoRec *theVar, varListRec *next)
{
    varListRec *listNode;
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    listNode = allocMem(sizeof(varListRec), nonConstPtr);
    listNode->theVar = theVar;
    listNode->next = next;
    return listNode;
}

/* *********************************************************************** */
/*                  Procedures (and main program)                          */
/* *********************************************************************** */

static procRec *makeGENERALProc(char *caller)
{
    procRec *rec;
    rec = allocMem(sizeof(procRec), caller);
    rec->name = NULL;
    rec->entryPoint = -1;          /*  Perhaps change  */
    rec->params = NULL;
    rec->localVars = NULL;
    rec->paramSpace = -1;
    rec->varSpace = -1;
    rec->returnType = NULL;
    rec->theHeading = NULL;
    rec->childProcs = NULL;
    rec->level = -1;    /*  Replace this in each case  */
    rec->body = NULL;
    rec->code = NULL;
    rec->whichBuiltin = -1;
    return rec;
}

procRec *makeUserDefProc(char *name, int level)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    procRec *rec = makeGENERALProc(nonConstPtr);
    rec->pKind = USERDEFINEDPROC;
    rec->name = name;
    rec->level = level;
    return rec;
}

procRec *makeForwardProc(char *name, int level, procHeading *theHeading)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    procRec *rec = makeGENERALProc(nonConstPtr);
    rec->pKind = USERDEFINEDPROC;
    rec->name = name;
    rec->level = level;
    rec->theHeading = theHeading;
    return rec;
}

procRec *makeBuiltinProc(char *name, varListRec *params, int whichBuiltin)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    procRec *rec = makeGENERALProc(nonConstPtr);
    rec->pKind = BUILTINPROC;
    rec->name = name;
    rec->params = params;
    rec->level = OUTERSCOPELEVEL;
    rec->whichBuiltin = whichBuiltin;
    return rec;
}

procRec *makeBuiltinFunction(char *name, varListRec *params,
			     typeRec *returnType, int whichBuiltin)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    procRec *rec = makeGENERALProc(nonConstPtr);
    rec->pKind = BUILTINFUNCTION;
    rec->name = name;
    rec->params = params;
    rec->returnType = returnType;
    rec->level = OUTERSCOPELEVEL;
    rec->whichBuiltin = whichBuiltin;
    return rec;
}

procListRec *makeProcListRec(procRec *theProc, procListRec *next)
{
    procListRec *rec;
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    rec = allocMem(sizeof(procListRec), nonConstPtr);
    rec->theProc = theProc;
    rec->next = next;
    return rec;
}

/* *********************************************************************** */
/*      Data attached to variable accesses in abstract syntax trees        */
/* *********************************************************************** */

varData *makeVarData(typeRec *t, varInfoRec *vi)
{
    varData *rec;
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    rec = allocMem(sizeof(procListRec), nonConstPtr);
    rec->t = t;
    rec->vi = vi;
    return rec;
}

/* *********************************************************************** */
/*          Data attached to expressions in abstract syntax trees          */
/* *********************************************************************** */

static exprData *makeGENERALExprData(char *caller)
{
    exprData *rec;
    rec = allocMem(sizeof(exprData), caller);
    rec->intConstVal = 0;
    rec->realConstVal = 0.0;
    rec->boolConstVal = 0;
    rec->strConstVal = NULL;
    rec->stringLeng = 0;
    rec->charConstVal = '\0';
    rec->f = NULL;
    return rec;
}

exprData *makeIntConstExprData(int val)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec->kind = INTCONSTKIND;
    rec->t = integerType;
    rec->intConstVal = val;
    return rec;
}

exprData *makeRealConstExprData(double val)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec->kind = REALCONSTKIND;
    rec->t = realType;
    rec->realConstVal = val;
    return rec;
}

exprData *makeBooleanConstExprData(int val)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec = allocMem(sizeof(exprData), nonConstPtr);
    rec->kind = BOOLCONSTKIND;
    rec->t = boolType;
    rec->boolConstVal = val;
    return rec;
}

exprData *makeStrConstExprData(char *val)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec->kind = STRCONSTKIND;
    rec->t = stringType;
    rec->strConstVal = val;
    rec->stringLeng = strlen(val);
    rec->charConstVal = rec->stringLeng == 1 ? val[0] : '\0';
    return rec;
}

exprData *makeFunCallExprData(typeRec *t, procRec *f)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec->kind = FUNCALLKIND;
    rec->t = t;
    rec->f = f;
    return rec;
}

exprData *makeOtherExprData(typeRec *t)
{
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    exprData *rec = makeGENERALExprData(nonConstPtr);
    rec->kind = OTHEREXPRKIND;
    rec->t = t;
    return rec;
}

/* *********************************************************************** */
/*          Data attached to statements in abstract syntax trees           */
/* *********************************************************************** */

stmntData *makeProcCallStmntData(procRec *p)
{
    stmntData *rec;
    const char *constPtr = __func__;
    char *nonConstPtr = (char *)constPtr;
    rec = allocMem(sizeof(stmntData), nonConstPtr);
    rec->p = p;
    return rec;
}
