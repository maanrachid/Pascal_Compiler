#include "tools.h"
#include "syntaxTree.h"
#include "grammar.tab.h"

/* ************************************************************** */
/* *******************   identifier lists   ********************* */
/* ************************************************************** */

idListRec *makeIdListRec(tokenRec *name, idListRec *next)
{
    idListRec *rec;
    rec = allocMem(sizeof(idListRec), "makeIdListRec");
    rec->theIdent = name;
    rec->next = next;
    return rec;
}

/* ************************************************************** */
/* *******************        types         ********************* */
/* ************************************************************** */

signedIntRec *makeSignedIntRec(tokenRec *sign, tokenRec *valToken)
{
    signedIntRec *theInt;
    theInt = allocMem(sizeof(signedIntRec), "makeSignedIntRec");
    theInt->sign = sign;
    theInt->valToken = valToken;
    return theInt;
}

indexRangeRec *makeIndexRangeRec(signedIntRec *lowBound,
				 signedIntRec *highBound)
{
    indexRangeRec *theRec;
    theRec = allocMem(sizeof(indexRangeRec), "makeIndexRangeRec");
    theRec->lowBound = lowBound;
    theRec->highBound = highBound;
    return theRec;
}

recordSectionRec *makeRecordSectionRec(idListRec *names,
				       struct typeDenoter *theType)
{
    recordSectionRec *recSect;
    recSect = allocMem(sizeof(recordSectionRec), "makeRecordSectionRec");
    recSect->names = names;
    recSect->theType = theType;
    return recSect;
}

fieldList *makeFieldList(recordSectionRec *recSect, fieldList *next)
{
    fieldList *listRec;
    listRec = allocMem(sizeof(fieldList), "makeFieldList");
    listRec->recSect = recSect;
    listRec->next = next;
    return listRec;
}

typeDenoter *makeNameTypeDenoter(tokenRec *name)
{
    typeDenoter *td;
    td = allocMem(sizeof(typeDenoter), "makeNameTypeDenoter");
    td->kind = NAMEDENOTER;
    td->name = name;
    td->range = NULL;
    td->componentType = NULL;
    td->theFields = NULL;
    return td;
}

typeDenoter *makeArrayTypeDenoter(indexRangeRec *range,
				  typeDenoter *componentType)
{
    typeDenoter *td;
    td = allocMem(sizeof(typeDenoter), "makeArrayTypeDenoter");
    td->kind = ARRAYDENOTER;
    td->name = NULL;
    td->range = range;
    td->componentType = componentType;
    td->theFields = NULL;
    return td;
}

typeDenoter *makeRecordTypeDenoter(fieldList *theFields)
{
    typeDenoter *td;
    td = allocMem(sizeof(typeDenoter), "makeRecordTypeDenoter");
    td->kind = RECORDDENOTER;
    td->name = NULL;
    td->range = NULL;
    td->componentType = NULL;
    td->theFields = theFields;
    return td;
}

typeDefRec *makeTypeDefRec(tokenRec *name, typeDenoter *theType)
{
    typeDefRec *def;
    def = allocMem(sizeof(typeDefRec), "makeTypeDefRec");
    def->name = name;
    def->theType = theType;
    return def;
}

typeDefList *makeTypeDefList(typeDefRec *def, typeDefList *next)
{
    typeDefList *listRec;
    listRec = allocMem(sizeof(typeDefList), "makeTypeDefList");
    listRec->def = def;
    listRec->next = next;
    return listRec;
}

/* ************************************************************** */
/* *************      variable declarations       *************** */
/* ************************************************************** */

varDecRec *makeVarDecRec(idListRec *ids, typeDenoter *td)
{
    varDecRec *dec;
    dec = allocMem(sizeof(varDecRec), "makeVarDecRec");
    dec->ids = ids;
    dec->td = td;
    return dec;
}

varDecList *makeVarDecList(varDecRec *dec, varDecList *next)
{
    varDecList *listRec;
    listRec = allocMem(sizeof(varDecList), "makeVarDecList");
    listRec->dec = dec;
    listRec->next = next;
    return listRec;
}

/* ************************************************************** */
/* *********   procedure and function declarations    *********** */
/* ************************************************************** */

formalParamSection *makeFormalParamSection(int isVarParamSpec,
					   idListRec *ids,
					   typeDenoter *td)
{
    formalParamSection *paramSect;
    paramSect = allocMem(sizeof(formalParamSection), "makeFormalParamSection");
    paramSect->isVarParamSpec = isVarParamSpec;
    paramSect->ids = ids;
    paramSect->td = td;
    return paramSect;
}

formalParamList *makeFormalParamList(formalParamSection *pSect,
				     formalParamList *next)
{
    formalParamList *listRec;
    listRec = allocMem(sizeof(formalParamList), "makeformalParamList");
    listRec->pSect = pSect;
    listRec->next = next;
    return listRec;
}

procHeading *makeProcHeading(tokenRec *procName,
			     formalParamList *pList)
{
    procHeading *heading;
    heading = allocMem(sizeof(procHeading), "makeProcHeading");
    heading->procName = procName;
    heading->pList = pList;
    return heading;
}

procDec *makeProcDec(procHeading *heading, struct blockRec *block)
{
    procDec *dec;
    dec = allocMem(sizeof(procDec), "makeProcDec");
    dec->heading = heading;
    dec->block = block;
    return dec;
}

procList *makeProcList(procDec *dec, procList *next)
{
    procList *listRec;
    listRec = allocMem(sizeof(procList), "makeProcList");
    listRec->dec = dec;
    listRec->next = next;
    return listRec;
}

/* ************************************************************** */
/* ***************      variable accesses       ***************** */
/* ************************************************************** */

varAccessRec *makeEntireVarAccessRec(tokenRec *name)
{
    varAccessRec *vAcc;
    vAcc = allocMem(sizeof(varAccessRec), "makeEntireVarAccessRec");
    vAcc->kind = ENTIREVAR;
    vAcc->startPos = name->location.firstChar;
    vAcc->name = name;
    vAcc->baseVar = NULL;
    vAcc->index = NULL;
    vAcc->fieldName = NULL;
    vAcc->vData = NULL;
    return vAcc;
}

varAccessRec *makeIndexedVarAccessRec(varAccessRec *baseVar, 
				      struct exprRec *index)
{
    varAccessRec *vAcc;
    vAcc = allocMem(sizeof(varAccessRec), "makeIndexedVarAccessRec");
    vAcc->kind = INDEXEDVAR;
    vAcc->startPos = baseVar->startPos;
    vAcc->name = NULL;
    vAcc->baseVar = baseVar;
    vAcc->index = index;
    vAcc->fieldName = NULL;
    vAcc->vData = NULL;
    return vAcc;
}

varAccessRec *makeFieldVarAccessRec(varAccessRec *baseVar,
				    tokenRec *fieldName)
{
    varAccessRec *vAcc;
    vAcc = allocMem(sizeof(varAccessRec), "makeFieldVarAccessRec");
    vAcc->kind = FIELDVAR;
    vAcc->startPos = baseVar->startPos;
    vAcc->name = NULL;
    vAcc->baseVar = baseVar;
    vAcc->index = NULL;
    vAcc->fieldName = fieldName;
    vAcc->vData = NULL;
    return vAcc;
}

/* ************************************************************** */
/* ******************      expressions       ******************** */
/* ************************************************************** */

unsignedConstRec *makeUnsignedConstRec(tokenRec *val)
{
    unsignedConstRec *uConst;
    uConst = allocMem(sizeof(unsignedConstRec), "makeUnsignedConstRec");
    if (val->tType == INTCONST)
	uConst->kind = UINT;
    else if (val->tType == REALCONST)
	uConst->kind = UREAL;
    else if (val->tType == STRINGCONST)
	uConst->kind = USTRING;
    else if (val->tType == TRUESYM || val->tType == FALSESYM)
	uConst->kind = UBOOL;
    else
	internalError("bad call to makeUnsignedConstRec");
    uConst->val = val;
    return uConst;
}

exprRec *makeConstExprRec(unsignedConstRec *constVal)
{
    exprRec *theExpr;
    theExpr = allocMem(sizeof(exprRec), "makeConstExprRec");
    theExpr->kind = CONSTEXPR;
    theExpr->startPos = constVal->val->location.firstChar;
    theExpr->constVal = constVal;
    theExpr->theVar = NULL;
    theExpr->op = NULL;
    theExpr->left = NULL;
    theExpr->right = NULL;
    theExpr->name = NULL;
    theExpr->actualParams = NULL;
    theExpr->eData = NULL;
    return theExpr;
}

exprRec *makeVariableExprRec(varAccessRec *theVar)
{
    exprRec *theExpr;
    theExpr = allocMem(sizeof(exprRec), "makeVariableExprRec");
    theExpr->kind = VAREXPR;
    theExpr->startPos = theVar->startPos;
    theExpr->constVal = NULL;
    theExpr->theVar = theVar;
    theExpr->op = NULL;
    theExpr->left = NULL;
    theExpr->right = NULL;
    theExpr->name = NULL;
    theExpr->actualParams = NULL;
    theExpr->eData = NULL;
    return theExpr;
}

exprRec *makeUnaryOpExprRec(tokenRec *op, exprRec *left)
{
    exprRec *theExpr;
    theExpr = allocMem(sizeof(exprRec), "makeUnaryOpExprRec");
    theExpr->kind = UNARYOPEXPR;
    theExpr->startPos = op->location.firstChar;
    theExpr->constVal = NULL;
    theExpr->theVar = NULL;
    theExpr->op = op;
    theExpr->left = left;
    theExpr->right = NULL;
    theExpr->name = NULL;
    theExpr->actualParams = NULL;
    theExpr->eData = NULL;
    return theExpr;
}

exprRec *makeBinaryOpExprRec(tokenRec *op, exprRec *left, exprRec *right)
{
    exprRec *theExpr;
    theExpr = allocMem(sizeof(exprRec), "makeBinaryOpExprRec");
    theExpr->kind = BINARYOPEXPR;
    theExpr->startPos = left->startPos;
    theExpr->constVal = NULL;
    theExpr->theVar = NULL;
    theExpr->op = op;
    theExpr->left = left;
    theExpr->right = right;
    theExpr->name = NULL;
    theExpr->actualParams = NULL;
    theExpr->eData = NULL;
    return theExpr;
}

exprRec *makeFunCallExprRec(tokenRec *name, actualParamList *actualParams)
{
    exprRec *theExpr;
    theExpr = allocMem(sizeof(exprRec), "makeFunCallExprRec");
    theExpr->kind = FUNCALLEXPR;
    theExpr->startPos = name->location.firstChar;
    theExpr->constVal = NULL;
    theExpr->theVar = NULL;
    theExpr->op = NULL;
    theExpr->left = NULL;
    theExpr->right = NULL;
    theExpr->name = name;
    theExpr->actualParams = actualParams;
    theExpr->eData = NULL;
    return theExpr;
}

/* ************************************************************** */
/* *******************     statements       ********************* */
/* ************************************************************** */

actualParamList *makeActualParamList(exprRec *param, actualParamList *next)
{
    actualParamList *listRec;
    listRec = allocMem(sizeof(actualParamList), "makeActualParamList");
    listRec->param = param;
    listRec->next = next;
    return listRec;
}

stmntRec *makeNullStmntRec(void)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeNullStmntRec");
    stmnt->kind = NULLSTMNT;
    stmnt->left = NULL;
    stmnt->right = NULL;
    stmnt->stmnts = NULL;
    stmnt->testExpr = NULL;
    stmnt->thenPart = NULL;
    stmnt->elsePart = NULL;
    stmnt->body = NULL;
    stmnt->procName = NULL;
    stmnt->actualParams = NULL;
    stmnt->sData = NULL;
    return stmnt;
}

stmntRec *makeAssignmentStmntRec(varAccessRec *left, exprRec *right)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeAssignmenttmntRec");
    stmnt->kind = ASSIGNSTMNT;
    stmnt->left = left;
    stmnt->right = right;
    stmnt->stmnts = NULL;
    stmnt->testExpr = NULL;
    stmnt->thenPart = NULL;
    stmnt->elsePart = NULL;
    stmnt->body = NULL;
    stmnt->procName = NULL;
    stmnt->actualParams = NULL;
    stmnt->sData = NULL;
    return stmnt;
}

stmntRec *makeCompoundStmntRec(struct stmntList *stmnts)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeCompoundStmntRec");
    stmnt->kind = COMPOUNDSTMNT;
    stmnt->left = NULL;
    stmnt->right = NULL;
    stmnt->stmnts = stmnts;
    stmnt->testExpr = NULL;
    stmnt->thenPart = NULL;
    stmnt->elsePart = NULL;
    stmnt->body = NULL;
    stmnt->procName = NULL;
    stmnt->actualParams = NULL;
    stmnt->sData = NULL;
    return stmnt;
}

stmntRec *makeIfStmntRec(exprRec *testExpr,
			 stmntRec *thenPart,
			 stmntRec *elsePart)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeIfStmntRec");
    stmnt->kind = IFSTMNT;
    stmnt->left = NULL;
    stmnt->right = NULL;
    stmnt->stmnts = NULL;
    stmnt->testExpr = testExpr;
    stmnt->thenPart = thenPart;
    stmnt->elsePart = elsePart;
    stmnt->body = NULL;
    stmnt->procName = NULL;
    stmnt->actualParams = NULL;
    stmnt->sData = NULL;
    return stmnt;
}

stmntRec *makeWhileStmntRec(exprRec *testExpr, stmntRec *body)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeWhileStmntRec");
    stmnt->kind = WHILESTMNT;
    stmnt->left = NULL;
    stmnt->right = NULL;
    stmnt->stmnts = NULL;
    stmnt->testExpr = testExpr;
    stmnt->thenPart = NULL;
    stmnt->elsePart = NULL;
    stmnt->body = body;
    stmnt->procName = NULL;
    stmnt->actualParams = NULL;
    stmnt->sData = NULL;
    return stmnt;
}

stmntRec *makeProcStmntRec(tokenRec *procName, actualParamList *actualParams)
{
    stmntRec *stmnt;
    stmnt = allocMem(sizeof(stmntRec), "makeProcStmntRec");
    stmnt->kind = PROCSTMNT;
    stmnt->left = NULL;
    stmnt->right = NULL;
    stmnt->stmnts = NULL;
    stmnt->testExpr = NULL;
    stmnt->thenPart = NULL;
    stmnt->elsePart = NULL;
    stmnt->body = NULL;
    stmnt->procName = procName;
    stmnt->actualParams = actualParams;
    stmnt->sData = NULL;
    return stmnt;
}

stmntList *makeStmntList(stmntRec *stmnt, stmntList *next)
{
    stmntList *listRec;
    listRec = allocMem(sizeof(stmntList), "makeStmntList");
    listRec->stmnt = stmnt;
    listRec->next = next;
    return listRec;
}

/* ************************************************************** */
/* ********************      blocks       *********************** */
/* ************************************************************** */

blockRec *makeBlockRec(typeDefList *typeDefs,
		       varDecList *varDecs,
		       procList *procDecs,
		       stmntRec *stmntPart)
{
    blockRec *b;
    b = allocMem(sizeof(blockRec), "makeBlockRec");
    b->typeDefs = typeDefs;
    b->varDecs = varDecs;
    b->procDecs = procDecs;
    b->stmntPart = stmntPart;
    return b;
}

/* ************************************************************** */
/* ********************      programs       ********************* */
/* ************************************************************** */

programRec *makeProgramRec(tokenRec *name, idListRec *params, blockRec *body)
{
    programRec *p;
    p = allocMem(sizeof(programRec), "makeProgramRec");
    p->progName = name;
    p->progParams = params;
    p->body = body;
    return p;
}
