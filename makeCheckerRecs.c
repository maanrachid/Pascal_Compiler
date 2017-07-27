#include "tools.h"
#include "syntaxTree.h"
#include "check.h"
#include "checkerGlobals.h"


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
    typeRec *rec = makeGENERALType(__func__);
    rec->tt = tt;
    return rec;
}

typeRec *makeArrayType(int lowBound, int highBound, typeRec *componentType)
{
    typeRec *rec = makeGENERALType(__func__);
    rec->tt = ARRAYTYPE;
    rec->lowBound = lowBound;
    rec->highBound = highBound;
    rec->componentType = componentType;
    return rec;
}

typeRec *makeRecordType(struct fieldInfoRec *fi)
{
    typeRec *rec = makeGENERALType(__func__);
    rec->tt = RECORDTYPE;
    rec->fi = fi;
    return rec;
}

fieldInfoRec *makeFieldInfoRec(char *fieldName, typeRec *fieldType,
			       fieldInfoRec *next)
{
    fieldInfoRec *rec;
    rec = allocMem(sizeof(fieldInfoRec), __func__);
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
    varInfoRec *rec = makeGENERALVarInfoRec(__func__);
    rec->vKind = LOCALVAR;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varInfoRec *makeValueParamInfoRec(char *name, typeRec *vType, int vLevel)
{
    varInfoRec *rec = makeGENERALVarInfoRec(__func__);
    rec->vKind = VALUEPARAM;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varInfoRec *makeVarParamInfoRec(char *name, typeRec *vType, int vLevel)
{
    varInfoRec *rec = makeGENERALVarInfoRec(__func__);
    rec->vKind = VARPARAM;
    rec->name = name;
    rec->vType = vType;
    rec->vLevel = vLevel;
    return rec;
}

varListRec *makeVarListRec(varInfoRec *theVar, varListRec *next)
{
    varListRec *listNode;
    listNode = allocMem(sizeof(varListRec), __func__);
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
    procRec *rec = makeGENERALProc(__func__);
    rec->pKind = USERDEFINEDPROC;
    rec->name = name;
    rec->level = level;
    return rec;
}

procRec *makeForwardProc(char *name, int level, procHeading *theHeading)
{
    procRec *rec = makeGENERALProc(__func__);
    rec->pKind = USERDEFINEDPROC;
    rec->name = name;
    rec->level = level;
    rec->theHeading = theHeading;
    return rec;
}

procRec *makeBuiltinProc(char *name, varListRec *params, int whichBuiltin)
{
    procRec *rec = makeGENERALProc(__func__);
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
    procRec *rec = makeGENERALProc(__func__);
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
    rec = allocMem(sizeof(procListRec), __func__);
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
    rec = allocMem(sizeof(varData), __func__);
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
    exprData *rec = makeGENERALExprData(__func__);
    rec->kind = INTCONSTKIND;
    rec->t = integerType;
    rec->intConstVal = val;
    return rec;
}

exprData *makeRealConstExprData(double val)
{
    exprData *rec = makeGENERALExprData(__func__);
    rec->kind = REALCONSTKIND;
    rec->t = realType;
    rec->realConstVal = val;
    return rec;
}

exprData *makeBooleanConstExprData(int val)
{
    exprData *rec = makeGENERALExprData(__func__);
    rec = allocMem(sizeof(exprData), __func__);
    rec->kind = BOOLCONSTKIND;
    rec->t = boolType;
    rec->boolConstVal = val;
    return rec;
}

exprData *makeStrConstExprData(char *val)
{
    exprData *rec = makeGENERALExprData(__func__);
    rec->kind = STRCONSTKIND;
    rec->t = stringType;
    rec->strConstVal = val;
    rec->stringLeng = strlen(val);
    rec->charConstVal = rec->stringLeng == 1 ? val[0] : '\0';
    return rec;
}

exprData *makeFunCallExprData(typeRec *t, procRec *f)
{
    exprData *rec = makeGENERALExprData(__func__);
    rec->kind = FUNCALLKIND;
    rec->t = t;
    rec->f = f;
    return rec;
}

exprData *makeOtherExprData(typeRec *t)
{
    exprData *rec = makeGENERALExprData(__func__);
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
    rec = allocMem(sizeof(stmntData), __func__);
    rec->p = p;
    return rec;
}
