#ifndef _check_h
#define _check_h

#include "syntaxTree.h"
/*  Usage of structs defined in this file:
 *
 *  typeRec:
 *	specification of a type
 *	bound to name(s) in symbol table
 *	references involving declarations in typeRecs, varInfoRecs, 
 *          fieldInfoRecs, and (for predefined functions) in procRecs
 *      references involving applied occurrences in varData and exprData
 *          records
 *
 *  fieldInfoRec:
 *      specification of a field in a record
 *      list of fieldInfoRecs pointed at by those typeRecs that define a record
 *
 *  varInfoRec:
 *      specification of a local variable, value param, or var param
 *      bound to name in symbol table
 *      member of list (of local vars or params) in procRec
 *      referenced (through varData structs) from those varAccessRecs
 *          that describe entire variables
 *
 *  varListRec:
 *      used to link varInfoRecs together into a list
 *
 *  procRec:
 *      used to summarize info about a procedure
 *      bound to name in symbol table (except for main procedure)
 *      contains lists of local variables and parameters
 *      contains pointer to representation in abstract syntax tree of
 *          statement part of the procedure's block
 *      referenced from exprData records (function calls) and stmntData
 *          records (procedure calls)
 *
 *  procListRec:
 *      used to link procRecs together into a list
 *      pointer to procListRec returned by checkProgram
 *
 *  varData:
 *      used to link type info and varInfoRecs to variable
 *          accesses (in abstract syntax tree)
 *      pointed at by vData field in varAccessRecs (in abstract syntax tree)
 *      contains type of the variable access (all variable accesses)
 *      contains pointer to info about the variable itself (as represented
 *          in varInfoRec) for entire variable accesses
 *     
 *  exprData:
 *      used to link type and other info to expressions (in abstract
 *          syntax tree)
 *      pointed at by eData field in exprRecs (in abstract syntax tree)
 *      contains type of the expression (all expressions)
 *      for constant expressions, contains value (binary form, not ASCII)
 *      for function calls, contains pointer to procRec for the function
 *
 *  stmntData:
 *      used to link info to certain statements (in abstract syntax tree)
 *      pointed at by sData field in stmntRecs (in abstract syntax tree)
 *      used only for procedure calls; then it contains pointer to
 *          procRec for the procedure
 */

/* *********************************************************************** */
/*                               Constants                                 */
/* *********************************************************************** */

#define OUTERSCOPELEVEL 0
#define BUILTINPARAMLEVEL 1

/* *********************************************************************** */
/*                             Type denoters                               */
/* *********************************************************************** */

typedef enum { INTTYPE, REALTYPE, BOOLTYPE, CHARTYPE, STRINGTYPE,
	       ARRAYTYPE, RECORDTYPE } typeType;

typedef struct typeRec {
    typeType tt;
    int lowBound;                    /* for arrays */
    int highBound;                   /* for arrays */
    struct typeRec *componentType;   /* for arrays */
    struct fieldInfoRec *fi;         /* for records */
    int size;
} typeRec;

typeRec *makeSimpleType(typeType tt);
typeRec *makeArrayType(int lowBound, int highBound, typeRec *componentType);
typeRec *makeRecordType(struct fieldInfoRec *fi);

typedef struct fieldInfoRec {
    char *fieldName;
    typeRec *fieldType;
    int size;                        /* for use by code generator */
    int offset;                      /* for use by code generator */
    struct fieldInfoRec *next;
} fieldInfoRec;

fieldInfoRec *makeFieldInfoRec(char *fieldName, typeRec *fieldType,
			       fieldInfoRec *next);

/* *********************************************************************** */
/*               Variable (and parameter) declarations                     */
/* *********************************************************************** */

typedef enum { LOCALVAR, VALUEPARAM, VARPARAM } varKind;

typedef struct varInfoRec {
    varKind vKind;
    char *name;                      /* probably not needed */
    typeRec *vType;
    int vLevel;
    int vSize;                       /* for use by code generator */
    int vOffset;                     /* for use by code generator */
/*  struct procRec *inWhichProc;      maybe not needed; not used now */
} varInfoRec;

varInfoRec *makeLocalVarInfoRec(char *name, typeRec *vType, int vLevel);
varInfoRec *makeValueParamInfoRec(char *name, typeRec *vType, int vLevel);
varInfoRec *makeVarParamInfoRec(char *name, typeRec *vType, int vLevel);

typedef struct varListRec {
    varInfoRec *theVar;
    struct varListRec *next;
} varListRec;

varListRec *makeVarListRec(varInfoRec *theVar, varListRec *next);

/* *********************************************************************** */
/*                  Procedures (and main program)                          */
/* *********************************************************************** */

typedef enum { USERDEFINEDPROC, BUILTINPROC, BUILTINFUNCTION } procKind;

typedef struct procRec {
    procKind pKind;
    char *name;
    int entryPoint;                  /* Perhaps change to char *   */
    varListRec *params;
    varListRec *localVars;
    int paramSpace;
    int varSpace;
    typeRec *returnType;
    procHeading *theHeading;
    struct procListRec *childProcs;
    int level;
    stmntRec *body;
    struct codeRec *code;
    int whichBuiltin;
} procRec;

procRec *makeUserDefProc(char *name, int level);
procRec *makeForwardProc(char *name, int level, procHeading *theHeading);
procRec *makeBuiltinProc(char *name, varListRec *params, int whichBuiltin);
procRec *makeBuiltinFunction(char *name, varListRec *params,
			     typeRec *returnType, int whichBuiltin);

typedef struct procListRec {
    procRec *theProc;
    struct procListRec *next;
} procListRec;

procListRec *makeProcListRec(procRec *theProc, procListRec *next);

/* *********************************************************************** */
/*      Data attached to variable accesses in abstract syntax trees        */
/* *********************************************************************** */

typedef struct varData {
    typeRec *t;
    varInfoRec *vi;
} varData;

varData *makeVarData(typeRec *t, varInfoRec *vi);

/* *********************************************************************** */
/*          Data attached to expressions in abstract syntax trees          */
/* *********************************************************************** */

typedef enum { INTCONSTKIND, REALCONSTKIND, BOOLCONSTKIND, STRCONSTKIND,
	       FUNCALLKIND, OTHEREXPRKIND } exprDataKind;

typedef struct exprData {
    exprDataKind kind;
    typeRec *t;
    int intConstVal;
    double realConstVal;
    int boolConstVal;
    char *strConstVal;
    int stringLeng;
    char charConstVal;
    procRec *f;
} exprData;

exprData *makeIntConstExprData(int val);
exprData *makeRealConstExprData(double val);
exprData *makeBooleanConstExprData(int val);
exprData *makeStrConstExprData(char *val);
exprData *makeFunCallExprData(typeRec *t, procRec *f);
exprData *makeOtherExprData(typeRec *t);

/* *********************************************************************** */
/*          Data attached to statements in abstract syntax trees           */
/* *********************************************************************** */

typedef struct stmntData {
    procRec *p;
} stmntData;

stmntData *makeProcCallStmntData(procRec *p);

/* *********************************************************************** */
/*                Procedure to call to start type checking                 */
/* *********************************************************************** */

procListRec *checkProgram(programRec *prog);

/* *********************************************************************** */
/*                          Extern declarations                            */
/* *********************************************************************** */

extern procListRec *builtinProcs;

extern typeRec *integerType;
extern typeRec *realType;
extern typeRec *boolType;
extern typeRec *charType;
extern typeRec *stringType;

#endif
