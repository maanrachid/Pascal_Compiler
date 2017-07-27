#include "tools.h"
#include "syntaxTree.h"
#include "check.h"
#include "stdObjects.h"
#include "procCodes.h"
#include "checkerTools.h"
#include "checkerGlobals.h"
/* ********************************************************************** */
/*                              Some constants                            */
/* ********************************************************************** */

enum {VALUEP, VARP};

/* ********************************************************************** */
/*                  Prototype for functions defined below                 */
/* ********************************************************************** */

static varListRec *addToPLst(typeRec *t, int paramKind, varListRec *lst);
static varListRec *makeBuiltinParamList1(typeRec *t, int paramKind);
static varListRec *makeBuiltinParamList2(typeRec *t1, int paramKind1,
					 typeRec *t2, int paramKind2);
static varListRec *makeBuiltinParamList3(typeRec *t1, int paramKind1,
					 typeRec *t2, int paramKind2,
					 typeRec *t3, int paramKind3);
static void installBuiltinProc(char *name, varListRec *params,
			       int whichBuiltin);
static void installBuiltinFunction(char *name, varListRec *params,
				   typeRec *returnType, int whichBuiltin);

/* ********************************************************************** */
/*   Tools for building representations of standard procs and functions   */
/* ********************************************************************** */

static varListRec *addToPLst(typeRec *t, int paramKind, varListRec *lst)
{
    varInfoRec *param;
    switch (paramKind) {
	case VALUEP:
	    param = makeValueParamInfoRec("", t, BUILTINPARAMLEVEL);
	    break;
	case VARP:
	    param = makeVarParamInfoRec("", t, BUILTINPARAMLEVEL);
	    break;
	default:
	    internalError("bad call to addToPLst");
	    break;
    }
    return makeVarListRec(param, lst);
}

static varListRec *makeBuiltinParamList1(typeRec *t, int paramKind)
{
    return addToPLst(t, paramKind, NULL);
}

static varListRec *makeBuiltinParamList2(typeRec *t1, int paramKind1,
					 typeRec *t2, int paramKind2)
{
    return addToPLst(t1, paramKind1,
		     addToPLst(t2, paramKind2, NULL));
}

static varListRec *makeBuiltinParamList3(typeRec *t1, int paramKind1,
					 typeRec *t2, int paramKind2,
					 typeRec *t3, int paramKind3)
{
    return addToPLst(t1, paramKind1,
		     addToPLst(t2, paramKind2,
			       addToPLst(t3,  paramKind3, NULL)));
}

static void installBuiltinProc(char *name, varListRec *params,
			       int whichBuiltin)
{
    procRec *proc;
    proc = makeBuiltinProc(name, params, whichBuiltin);
    putProcIntoSymtab(name, proc);
    addToProcList(proc, &builtinProcs);
}

static void installBuiltinFunction(char *name, varListRec *params,
				   typeRec *returnType, int whichBuiltin)
{
    procRec *fun;
    fun = makeBuiltinFunction(name, params, returnType, whichBuiltin);
    putProcIntoSymtab(name, fun);
    addToProcList(fun, &builtinProcs);
}

/* ********************************************************************** */
/*                           Exported procedures                          */
/* ********************************************************************** */

void initStdTypes(void)
{
    integerType = makeSimpleType(INTTYPE);
    putTypeIntoSymtab("integer", integerType);
    realType = makeSimpleType(REALTYPE);
    putTypeIntoSymtab("real", realType);
    boolType = makeSimpleType(BOOLTYPE);
    putTypeIntoSymtab("boolean", boolType);
    charType = makeSimpleType(CHARTYPE);
    putTypeIntoSymtab("char", charType);
    stringType = makeSimpleType(STRINGTYPE);  /* not put into symbol table */
}

void initStdProcs(void)
{
    installBuiltinProc("writeln",
		       NULL,
		       WRITELNproc);

    installBuiltinProc("writeInteger",
		       makeBuiltinParamList1(integerType, VALUEP),
		       WRITEINTEGERproc);

    installBuiltinProc("writeInteger2",
		       makeBuiltinParamList2(integerType, VALUEP,
					     integerType, VALUEP),
		       WRITEINTEGER2proc);

    installBuiltinProc("writeReal",
		       makeBuiltinParamList1(realType, VALUEP),
		       WRITEREALproc);

    installBuiltinProc("writeReal2",
		       makeBuiltinParamList2(realType, VALUEP,
					     integerType, VALUEP),
		       WRITEREAL2proc);

    installBuiltinProc("writeReal3",
		       makeBuiltinParamList3(realType, VALUEP,
					     integerType, VALUEP,
					     integerType, VALUEP),
		       WRITEREAL3proc);

    installBuiltinProc("writeChar",
		       makeBuiltinParamList1(charType, VALUEP),
		       WRITECHARproc);

    installBuiltinProc("writeString",
		       makeBuiltinParamList1(stringType, VALUEP),
		       WRITESTRINGproc);

    installBuiltinProc("readln",
		       NULL,
		       READLNproc);

    installBuiltinProc("readInteger",
		       makeBuiltinParamList1(integerType, VARP),
		       READINTEGERproc);

    installBuiltinProc("readReal",
		       makeBuiltinParamList1(realType, VARP),
		       READREALproc);

    installBuiltinProc("readChar",
		       makeBuiltinParamList1(charType, VARP),
		       READCHARproc);

    installBuiltinFunction("trunc",
			   makeBuiltinParamList1(realType, VALUEP),
			   integerType,
			   TRUNCfun);

    installBuiltinFunction("round",
			   makeBuiltinParamList1(realType, VALUEP),
			   integerType,
			   ROUNDfun);

    installBuiltinFunction("sqrt",
			   makeBuiltinParamList1(realType, VALUEP),
			   realType,
			   SQRTfun);

    installBuiltinFunction("eof",
			   NULL,
			   boolType,
			   EOFfun);

    installBuiltinFunction("eoln",
			   NULL,
			   boolType,
			   EOLNfun);

    installBuiltinFunction("chr",
			   makeBuiltinParamList1(integerType, VALUEP),
			   charType,
			   CHRfun);

    installBuiltinFunction("ord",
			   makeBuiltinParamList1(charType, VALUEP),
			   integerType,
			   ORDfun);
}
