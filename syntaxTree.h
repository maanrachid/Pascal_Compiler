#ifndef _syntaxTree_h
#define _syntaxTree_h

#include "scan.h"

/* ************************************************************** */
/* *******************   identifier lists   ********************* */
/* ************************************************************** */

typedef struct idListRec {
    tokenRec *theIdent;
    struct idListRec *next;
} idListRec;

idListRec *makeIdListRec(tokenRec *name, idListRec *next);

/* ************************************************************** */
/* *******************        types         ********************* */
/* ************************************************************** */

typedef struct {
    tokenRec *sign;
    tokenRec *valToken;
} signedIntRec;

typedef enum {NAMEDENOTER, ARRAYDENOTER, RECORDDENOTER } typeDenoterKind;

typedef struct {
    signedIntRec *lowBound;
    signedIntRec *highBound;
} indexRangeRec;

typedef struct {
    idListRec *names;
    struct typeDenoter *theType;
} recordSectionRec;

typedef struct fieldList {
    recordSectionRec *recSect;
    struct fieldList *next;
} fieldList;

typedef struct typeDenoter {
    typeDenoterKind kind;
    tokenRec *name;                     /* name type denoters (identifiers) */
    indexRangeRec *range;               /* arrays */
    struct typeDenoter *componentType;  /* arrays */
    fieldList *theFields;               /* records */
} typeDenoter;

typedef struct {
    tokenRec *name;
    typeDenoter *theType;
} typeDefRec;

typedef struct typeDefList {
    typeDefRec *def;
    struct typeDefList *next;
} typeDefList;

signedIntRec *makeSignedIntRec(tokenRec *sign, tokenRec *valToken);
indexRangeRec *makeIndexRangeRec(signedIntRec *lowBound,
				 signedIntRec *highBound);
recordSectionRec *makeRecordSectionRec(idListRec *names,
				       struct typeDenoter *theType);
fieldList *makeFieldList(recordSectionRec *recSect, fieldList *next);
typeDenoter *makeNameTypeDenoter(tokenRec *name);
typeDenoter *makeArrayTypeDenoter(indexRangeRec *range,
				  typeDenoter *componentType);
typeDenoter *makeRecordTypeDenoter(fieldList *theFields);
typeDefRec *makeTypeDefRec(tokenRec *name, typeDenoter *theType);
typeDefList *makeTypeDefList(typeDefRec *def, typeDefList *next);

/* ************************************************************** */
/* *************      variable declarations       *************** */
/* ************************************************************** */

typedef struct {
    idListRec *ids;
    typeDenoter *td;
} varDecRec;

typedef struct varDecList {
    varDecRec *dec;
    struct varDecList *next;
} varDecList;

varDecRec *makeVarDecRec(idListRec *ids, typeDenoter *td);
varDecList *makeVarDecList(varDecRec *dec, varDecList *next);

/* ************************************************************** */
/* *********   procedure and function declarations    *********** */
/* ************************************************************** */

typedef struct {
    int isVarParamSpec;
    idListRec *ids;
    typeDenoter *td;   /*  Must be NAME type denoter */
} formalParamSection;

typedef struct formalParamList {
    formalParamSection *pSect;
    struct formalParamList *next;
} formalParamList;

typedef struct {
    tokenRec *procName;
    formalParamList *pList;
} procHeading;

typedef struct {
    procHeading *heading;
    struct blockRec *block;
} procDec;

typedef struct procList {
    procDec *dec;
    struct procList *next;
} procList;

formalParamSection *makeFormalParamSection(int isVarParamSpec,
					   idListRec *ids,
					   typeDenoter *td);
formalParamList *makeFormalParamList(formalParamSection *pSect,
				     formalParamList *next);
procHeading *makeProcHeading(tokenRec *procName,
			     formalParamList *pList);
procDec *makeProcDec(procHeading *heading, struct blockRec *block);
procList *makeProcList(procDec *dec, procList *next);

/* ************************************************************** */
/* ***************      variable accesses       ***************** */
/* ************************************************************** */

typedef enum {ENTIREVAR, INDEXEDVAR, FIELDVAR} varAccessKind;

typedef struct varAccessRec {
    varAccessKind kind;
    int startPos;
    tokenRec *name;               /* entire variables */
    struct varAccessRec *baseVar; /* indexed and field variables */
    struct exprRec *index;        /* indexed variables */
    tokenRec *fieldName;          /* field variables */
    struct varData *vData;        /* for type checker to put info */
} varAccessRec;

varAccessRec *makeEntireVarAccessRec(tokenRec *name);
varAccessRec *makeIndexedVarAccessRec(varAccessRec *baseVar, 
				      struct exprRec *index);
varAccessRec *makeFieldVarAccessRec(varAccessRec *baseVar,
				    tokenRec *fieldName);

/* ************************************************************** */
/* ******************      expressions       ******************** */
/* ************************************************************** */

typedef enum {UINT, UREAL, USTRING, UBOOL} unsignedConstKind;

typedef enum {CONSTEXPR, VAREXPR, UNARYOPEXPR, BINARYOPEXPR,
	      FUNCALLEXPR} exprKind;

typedef struct {
    unsignedConstKind kind;
    tokenRec *val;
} unsignedConstRec;

typedef struct actualParamList {
    struct exprRec *param;
    struct actualParamList *next;
} actualParamList;

typedef struct exprRec {
    exprKind kind;
    int startPos;
    unsignedConstRec *constVal;   /* constant expressions */
    varAccessRec *theVar;         /* variable expressions */
    tokenRec *op;                 /* unary and binary expressions */
    struct exprRec *left;         /* unary and binary expressions */
    struct exprRec *right;        /* binary expressions */
    tokenRec *name;               /* function calls */
    actualParamList *actualParams;/* function calls */
    struct exprData *eData;       /* for type checker to put info */
} exprRec;

unsignedConstRec *makeUnsignedConstRec(tokenRec *val);
exprRec *makeConstExprRec(unsignedConstRec *constVal);
exprRec *makeVariableExprRec(varAccessRec *theVar);
exprRec *makeUnaryOpExprRec(tokenRec *op, exprRec *left);
exprRec *makeBinaryOpExprRec(tokenRec *op, exprRec *left, exprRec *right);
exprRec *makeFunCallExprRec(tokenRec *name, actualParamList *actualParams);

/* ************************************************************** */
/* *******************     statements       ********************* */
/* ************************************************************** */

typedef enum {NULLSTMNT, ASSIGNSTMNT, COMPOUNDSTMNT,
              IFSTMNT, WHILESTMNT, PROCSTMNT} stmntKind;

typedef struct stmntRec {
    stmntKind kind;
    varAccessRec *left;           /* assignment statements */
    exprRec *right;               /* assignment statements */
    struct stmntList *stmnts;     /* compound statements */
    exprRec *testExpr;            /* if and while statements */
    struct stmntRec *thenPart;    /* if statements */
    struct stmntRec *elsePart;    /* if statements */
    struct stmntRec *body;        /* while statements */
    tokenRec *procName;           /* procedure statements */
    actualParamList *actualParams;/* procedure call statements*/
    struct stmntData *sData;      /* for type checker to put info */
} stmntRec;

typedef struct stmntList {
    stmntRec *stmnt;
    struct stmntList *next;
} stmntList;

actualParamList *makeActualParamList(exprRec *param, actualParamList *next);
stmntRec *makeNullStmntRec(void);
stmntRec *makeAssignmentStmntRec(varAccessRec *left, exprRec *right);
stmntRec *makeCompoundStmntRec(struct stmntList *stmnts);
stmntRec *makeIfStmntRec(exprRec *testExpr,
			 stmntRec *thenPart,
			 stmntRec *elsePart);
stmntRec *makeWhileStmntRec(exprRec *testExpr, stmntRec *body);
stmntRec *makeProcStmntRec(tokenRec *procName, actualParamList *actualParams);
stmntList *makeStmntList(stmntRec *stmnt, stmntList *next);

/* ************************************************************** */
/* ********************      blocks       *********************** */
/* ************************************************************** */

typedef struct blockRec{
    typeDefList *typeDefs;
    varDecList *varDecs;
    procList *procDecs;
    stmntRec *stmntPart;
} blockRec;

blockRec *makeBlockRec(typeDefList *typeDefs,
		       varDecList *varDecs,
		       procList *procDecs,
		       stmntRec *stmntPart);

/* ************************************************************** */
/* ********************      programs       ********************* */
/* ************************************************************** */

typedef struct {
    tokenRec *progName;
    idListRec *progParams;
    blockRec *body;
} programRec;

programRec *makeProgramRec(tokenRec *name, idListRec *params, blockRec *body);

#endif
