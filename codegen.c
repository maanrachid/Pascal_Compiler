#include <stdio.h>
#include <string.h>
#include "scan.h"
#include "syntaxTree.h"
#include "check.h"
#include "codegen.h"
#include "tools.h"
#include "codeDetails.h"
#include "opcodes.h"
#include "procCodes.h"
#include "grammar.tab.h"
#include "checkerTools.h"

#define FRAMEHEADERSIZE 5

void selectAllCode(procListRec *p);
void selectProcBodyCode(procRec *p);

void initCodegen(void);
void initStdTypeSizes(void);

void fixTypeSize(typeRec *t);
int fixFieldInfoRecs(fieldInfoRec *fi);

void setOffsetsInParamList(varListRec *params, int *pNextOffset);
void setOffsetsInVarList(varListRec *vars, int *pNextOffset);
void setAllVarAndParamOffsets(procListRec *p);

static void selectVarAccessCode(varAccessRec *var);
static void selectEntireVarAccessCode(varAccessRec *var);
static void selectIndexedVarAccessCode(varAccessRec *var);
static void selectFieldVarAccessCode(varAccessRec *var);

static void selectExpressionCode(exprRec *er);
static void selectConstExpressionCode(exprRec *er);
static void selectVarExpressionCode(exprRec *er);
static void selectUOpExpressionCode(exprRec *er);
static void selectBinOpExpressionCode(exprRec *er);
static void selectFunCallExpressionCode(exprRec *er);

static void selectActualParamListCode(actualParamList *apLst,
				      varListRec *fpLst);

static void selectBuiltinProcCode(int whichBuiltin);
static void handleWriteString(stmntRec *sr);

static void selectStatementCode(stmntRec *sr, procRec *inProc);
static void selectNullStatementCode(stmntRec *sr, procRec *inProc);
static void selectAssignStatementCode(stmntRec *sr, procRec *inProc);
static void selectCompoundStatementCode(stmntRec *sr, procRec *inProc);
static void selectIfStatementCode(stmntRec *sr, procRec *inProc);
static void selectWhileStatementCode(stmntRec *sr, procRec *inProc);
static void selectProcStatementCode(stmntRec *sr, procRec *inProc);
static void selectStatementListCode(stmntList *lst, procRec *inProc);

/* *********************************************************************** */
/*                             Top-level code                              */
/* *********************************************************************** */

void generateCode(procListRec *p, FILE *outFile)
{
    initCodegen();
    setAllVarAndParamOffsets(p);
    selectAllCode(p);
    calculateCodeAddrs(p);
    sendCodeToFile(p, outFile);
}

void selectAllCode(procListRec *p)
{
    procListRec *lst;
    lst = p;
    while (lst != NULL) {
	selectProcBodyCode(lst->theProc);
	lst = lst->next;
    }
}

void selectProcBodyCode(procRec *p)
{
    initCodeList();
    selectStatementCode(p->body, p);
    if (strcmp(p->name, "") == 0)
	emitInstr0(QUIT);
    else
	emitInstr0(RETPROC);
    p->code = getCodeList();
}

/* *********************************************************************** */
/*                             Initialization                              */
/* *********************************************************************** */

void initCodegen(void)
{
    initStdTypeSizes();
}

void initStdTypeSizes(void)
{
    integerType->size = 1;
    realType->size = 1;
    boolType->size = 1;
    charType->size = 1;
    stringType->size = 1;     /*  perhaps never used??  */
}

/* *********************************************************************** */
/*                                 Types                                   */
/* *********************************************************************** */

void fixTypeSize(typeRec *t)
{
  fieldInfoRec *temp;
    if (t->size >= 0)
	return;
    if (t->tt==ARRAYTYPE){
      fixTypeSize(t->componentType);
      t->size=(t->highBound - t->lowBound+1) * t->componentType->size;
      return;
    }else if (t->tt==RECORDTYPE){
      temp = t->fi;
      t->size=0;
      while(temp!=NULL){
	temp->offset=t->size;
	t->size+= fixFieldInfoRecs(temp);
	temp=temp->next;
      }
      return;
    }

    //    internalError("fixTypeSize does not yet allow this call");
}

int fixFieldInfoRecs(fieldInfoRec *fi)
{
    fixTypeSize(fi->fieldType);
    fi->size = fi->fieldType->size;
    return fi->size;
}

/* *********************************************************************** */
/*                  Offsets of variables and parameters                    */
/* *********************************************************************** */

void setOffsetsInParamList(varListRec *params, int *pNextOffset)
{
    varListRec *lst;
    varInfoRec *infoRec;
    typeRec *theType;
    int theSize;
    lst = params;
    while (lst != NULL) {
	infoRec = lst->theVar;
	theType = infoRec->vType;
	if (theType->size < 0)
	    fixTypeSize(theType);
	if (theType->size < 0)
	    internalError("error detected in setOffsetsInParamList");
	if (infoRec->vKind == VALUEPARAM)
	    theSize = theType->size;
	else if (infoRec->vKind == VARPARAM)
	    theSize = 1;
	else
	    internalError("bad record found by setOffsetsInParamList");
	infoRec->vOffset = *pNextOffset;
	infoRec->vSize = theSize;
	*pNextOffset += theSize;
	lst = lst->next;
    }
}

void setOffsetsInVarList(varListRec *vars, int *pNextOffset)
{
    varListRec *lst;
    varInfoRec *infoRec;
    typeRec *theType;
    int theSize;
    lst = vars;
    while (lst != NULL) {
	infoRec = lst->theVar;
	theType = infoRec->vType;
	if (theType->size < 0)
	    fixTypeSize(theType);
	if (theType->size < 0)
	    internalError("error detected in setOffsetsInVarList");
	if (infoRec->vKind == LOCALVAR)
	    theSize = theType->size;
	else
	    internalError("bad record found by setOffsetsInParamList");
	infoRec->vOffset = *pNextOffset;
	infoRec->vSize = theSize;
	*pNextOffset += theSize;
	lst = lst->next;
    }
}

void setAllVarAndParamOffsets(procListRec *p)
{
    procListRec *lst;
    procRec *theProc;
    int nextOffset, pSpace, vSpace;
    lst = p;
    while (lst != NULL) {
	theProc = lst->theProc;
	nextOffset = FRAMEHEADERSIZE;
	setOffsetsInParamList(theProc->params, &nextOffset);
	pSpace = nextOffset - FRAMEHEADERSIZE;
	setOffsetsInVarList(theProc->localVars, &nextOffset);
	vSpace = nextOffset - FRAMEHEADERSIZE - pSpace;
	theProc->paramSpace = pSpace;
	theProc->varSpace = vSpace;
	lst = lst->next;
    }
}

/* *********************************************************************** */
/*                            Variable accesses                            */
/* *********************************************************************** */

static void selectVarAccessCode(varAccessRec *var)
{
    switch (var->kind) {
	case ENTIREVAR:
	    selectEntireVarAccessCode(var);
	    break;
	case INDEXEDVAR:
	    selectIndexedVarAccessCode(var);
	    break;
	case FIELDVAR:
	    selectFieldVarAccessCode(var);
	    break;
	default:
	    internalError("bad call to selectVarAccessCode");
	    break;
    }
}

static void selectEntireVarAccessCode(varAccessRec *var)
{
    varInfoRec *vInfo;
    vInfo = var->vData->vi;
    switch (vInfo->vKind) {
	case LOCALVAR:
        case VALUEPARAM:
	    emitInstr2(LDADDR, vInfo->vLevel, vInfo->vOffset);
	    break;
       case VARPARAM:
	 emitInstr2(LDVAL , vInfo->vLevel, vInfo->vOffset);
	 break;

	default:
	    internalError("bad call to selectEntireVarAccessCode");
	    break;
    }
}

static void selectIndexedVarAccessCode(varAccessRec *var)
{
  varInfoRec *vInfo;
  selectVarAccessCode(var->baseVar);
  selectExpressionCode(var->index);


    if (var->baseVar->vData->t->componentType->size==1)
      emitInstr2(INDEX1,var->baseVar->vData->t->lowBound,
	       var->baseVar->vData->t->highBound);
    else 
      emitInstr3(INDEX,var->baseVar->vData->t->lowBound,
		 var->baseVar->vData->t->highBound,
		 var->baseVar->vData->t->componentType->size);

}

static void selectFieldVarAccessCode(varAccessRec *var)
{
  varInfoRec *vInfo;
  typeRec *t;
  fieldInfoRec *fi;
  selectVarAccessCode(var->baseVar);


    t= var->baseVar->vData->t;    
    fi = t->fi;
    
    while(fi!=NULL){
      if (strcmp(fi->fieldName,var->fieldName->spell)==0){
	emitInstr1(OFFSET,fi->offset);
	break;
      }
      fi=fi->next;
    }

}

/* *********************************************************************** */
/*                              Expressions                                */
/* *********************************************************************** */

static void selectExpressionCode(exprRec *er)
{
    switch (er->kind) {
	case CONSTEXPR:
	    selectConstExpressionCode(er);
	    break;
	case VAREXPR:
	    selectVarExpressionCode(er);
	    break;
	case UNARYOPEXPR:
	    selectUOpExpressionCode(er);
	    break;
	case BINARYOPEXPR:
	    selectBinOpExpressionCode(er);
	    break;
	case FUNCALLEXPR:
	    selectFunCallExpressionCode(er);
	    break;
	default:
	    internalError("bad call to checkExpression");
	    break;
    }
}

union converter {
    float r;
    int i;
};

static void selectConstExpressionCode(exprRec *er)
{
    exprData *eData;
    union converter x;
    eData = er->eData;
    switch (eData->kind) {
    case INTCONSTKIND:
	    emitInstr1(LOADIMMED, eData->intConstVal);
	    break;
    case REALCONSTKIND:
      x.r = eData->realConstVal;
      emitInstr1(LOADIMMED, x.i);
      break;
	case STRCONSTKIND:
	    if (eData->stringLeng != 1)
		internalError("bad call to selectConstExpressionCode");
	    emitInstr1(LOADIMMED, eData->charConstVal);
	    break;
    case BOOLCONSTKIND:
     
      emitInstr1(LOADIMMED,eData->boolConstVal);
      break;
	default:
	    internalError("bad call to selectConstExpressionCode");
	    break;
    }
}

static void selectVarExpressionCode(exprRec *er)
{
  
    selectVarAccessCode(er->theVar);
    if (er->theVar->vData->t->size==1)
      emitInstr0(GETINDIRECT);
    
    
    /* ***********  What about variables that occupy several words?  *****/
}

static void selectUOpExpressionCode(exprRec *er)
{
  exprRec *theExp;
  tokenType op;
  theExp= er->left;
  op = er->op->tType;
  selectExpressionCode(theExp);
  switch(op){
  case MINUS:
    if (theExp->eData->t==integerType)
      emitInstr0(NEGINT);
    else
      emitInstr0(NEGREAL);
    break;
  case NOTSYM:
    emitInstr0(NOTOP);
    break;
  }

  
    
}

static void selectBinOpExpressionCode(exprRec *er)
{
    tokenType op;
    exprRec *leftExpr, *rightExpr;
    op = er->op->tType;
    leftExpr = er->left;
    rightExpr = er->right;

    selectExpressionCode(leftExpr);
    selectExpressionCode(rightExpr);
    switch (op) {
	case PLUS:
	  if (leftExpr->eData->t==integerType && rightExpr->eData->t==integerType){
	   
	    emitInstr0(ADDINT);
	  }
	  else if (leftExpr->eData->t==realType && rightExpr->eData->t==realType){
           
            emitInstr0(ADDREAL);
          }else {
	   
	    if (leftExpr->eData->t==integerType)
	      emitInstr1(FLOAT,0);
	   
	    if (rightExpr->eData->t==integerType)
              emitInstr1(FLOAT,0);
	    emitInstr0(ADDREAL);
	  }
	  break;
	  
    case MINUS:
      if (leftExpr->eData->t==integerType && rightExpr->eData->t==integerType)\
	{
	 
	  emitInstr0(SUBINT);
	}
      else if (leftExpr->eData->t==realType && rightExpr->eData->t==realType){

	emitInstr0(SUBREAL);
      }else {

	if (leftExpr->eData->t==integerType)
	  emitInstr1(FLOAT,0);

	if (rightExpr->eData->t==integerType)
	  emitInstr1(FLOAT,0);
	emitInstr0(SUBREAL);
      }
      break;
      case TIMES:
	if (leftExpr->eData->t==integerType && rightExpr->eData->t==integerType)\
	  {
	    emitInstr0(MULINT);
	  }
	else if (leftExpr->eData->t==realType && rightExpr->eData->t==realType){
	  emitInstr0(MULREAL);
	}
	else {

	  if (leftExpr->eData->t==integerType)
	    emitInstr1(FLOAT,0);

	  if (rightExpr->eData->t==integerType)
	    emitInstr1(FLOAT,0);
	  emitInstr0(MULREAL);
	}
	break;

    case SLASH:

      if (leftExpr->eData->t==integerType)
	emitInstr1(FLOAT,0);

      if (rightExpr->eData->t==integerType)
	emitInstr1(FLOAT,0);
      emitInstr0(DIVREAL);
      break;

    case MODSYM:

      emitInstr0(MODINT);
      break;

    case DIVSYM:

      emitInstr0(DIVINT);
      break;
      
    case ORSYM:

      emitInstr0(OROP);
      break;
    case ANDSYM:
      emitInstr0(ANDOP);
      break;
      
    case NEQ:
      if (leftExpr->eData->t==realType)
	emitInstr0(NEQREAL);
      else
	emitInstr0(NEQINT);
      break;
      
    case EQ:
      if (leftExpr->eData->t==realType)
        emitInstr0(EQREAL);
      else
        emitInstr0(EQINT);
      break;


    case LT:
      if (leftExpr->eData->t==realType)
        emitInstr0(LTREAL);
      else
        emitInstr0(LTINT);
      break;

    case GT:
      if (leftExpr->eData->t==realType)
        emitInstr0(GTREAL);
      else
        emitInstr0(GTINT);
      break;

    case GEQ:
      if (leftExpr->eData->t==realType)
        emitInstr0(GEQREAL);
      else
        emitInstr0(GEQINT);
      break;

    case LEQ:
      if (leftExpr->eData->t==realType)
        emitInstr0(LEQREAL);
      else
        emitInstr0(LEQINT);
      break;

	default:
	    internalError("bad call to selectBinOpExpressionCode");
	    break;
    }
}

static void selectFunCallExpressionCode(exprRec *er)
{
  procRec *p;
  p=er->eData->f;
  selectActualParamListCode(er->actualParams, p->params);
  switch (p->whichBuiltin){
  case TRUNCfun:
    emitInstr0(TRUNCFCN);
    break;

  case SQRTfun:
    emitInstr0(SQRTFCN);
    break;

  case  EOFfun:
    emitInstr0(EOFFCN);
    break;
  case EOLNfun:
    emitInstr0(EOLNFCN);
    break;

  case CHRfun:
    emitInstr0(CHRFCN);
    break;

  case ORDfun:
    emitInstr0(ORDFCN);
    break;
    
  }
}

static void selectActualParamListCode(actualParamList *apLst,
				      varListRec *fpLst)
{
  
    while (fpLst != NULL) {
	switch (fpLst->theVar->vKind) {
	    case VALUEPARAM:
		selectExpressionCode(apLst->param);
		if (fpLst->theVar->vType==realType &&
		    apLst->param->eData->t==integerType)
		  emitInstr1(FLOAT,0);
		if (apLst->param->eData->t->size>1)
		  emitInstr1(LOADBLOCK,apLst->param->eData->t->size);

                /*****  Integers passed to real params? *******/
		break;
	case VARPARAM:
	  selectVarAccessCode(apLst->param->theVar);
	  break;
	    default:
		internalError("bad call to selectActualParamListCode");
		break;
	}
	fpLst = fpLst->next;
	apLst = apLst->next;
    }
          
   

}

/* *********************************************************************** */
/*                              Statements                                 */
/* *********************************************************************** */

static void selectStatementCode(stmntRec *sr, procRec *inProc)
{
    switch (sr->kind) {
	case NULLSTMNT:
	    selectNullStatementCode(sr, inProc);
	    break;
	case ASSIGNSTMNT:
	    selectAssignStatementCode(sr, inProc);
	    break;
	case COMPOUNDSTMNT:
	    selectCompoundStatementCode(sr, inProc);
	    break;
	case IFSTMNT:
	    selectIfStatementCode(sr, inProc);
	    break;
	case WHILESTMNT:
	    selectWhileStatementCode(sr, inProc);
	    break;
	case PROCSTMNT:
	    selectProcStatementCode(sr, inProc);
	    break;
	default:
	    internalError("bad call to selectStatementCode");
	    break;
    }
}

static void selectNullStatementCode(stmntRec *sr, procRec *inProc)
{
    /* Nothing to do */
}

static void selectAssignStatementCode(stmntRec *sr, procRec *inProc)
{
    selectVarAccessCode(sr->left);
    selectExpressionCode(sr->right);
    if (sr->left->vData->t==realType && sr->right->eData->t==integerType)
      emitInstr1(FLOAT,0);

    if (sr->left->vData->t->size==1)
      emitInstr0(STORE);
    else 
      emitInstr1(COPYBLOCK,sr->left->vData->t->size);
    /* ***** This certainly needs changes  ******************/
}

static void selectCompoundStatementCode(stmntRec *sr, procRec *inProc)
{
    selectStatementListCode(sr->stmnts, inProc);
}

static void selectIfStatementCode(stmntRec *sr, procRec *inProc)
{
  struct codeRec *c1,*c2;
  c1 = makeNewLabel();
  c2 =  makeNewLabel();
  selectExpressionCode(sr->testExpr);
  emitJump(JMPIFFALSE,c1);
  selectStatementCode(sr->thenPart,inProc);
  emitJump(JMP,c2);
  emitLabel(c1);
  selectStatementCode(sr->elsePart,inProc);
  emitLabel(c2);
}

static void selectWhileStatementCode(stmntRec *sr, procRec *inProc)
{
  struct codeRec *condition,*out;
  condition = makeNewLabel();
  out = makeNewLabel();
  emitLabel(condition);
  selectExpressionCode(sr->testExpr);
  emitJump(JMPIFFALSE,out);
  selectStatementCode(sr->body,inProc);
  emitJump(JMP,condition);
  emitLabel(out);
}

static void selectBuiltinProcCode(int whichBuiltin)
{
    switch (whichBuiltin) {
	case WRITELNproc:
	    emitInstr0(WRITELNOP);
	    break;
	case WRITEINTEGERproc:
	    emitInstr1(LOADIMMED, 10);
	    emitInstr0(WRITEINT);
	    break;
	case WRITEINTEGER2proc:
	    emitInstr0(WRITEINT);
	    break;
	case WRITEREALproc:
	    emitInstr1(LOADIMMED, 10);
	    emitInstr0(WRITEREAL2);
	    break;
	case WRITEREAL2proc:
	    emitInstr0(WRITEREAL2);
	    break;
	case WRITEREAL3proc:
	    emitInstr0(WRITEREAL3);
	    break;
	case WRITECHARproc:
	    emitInstr1(LOADIMMED, 1);
	    emitInstr0(WRITECHAR);
	    break;
	case READLNproc:
	    emitInstr0(READLNOP);
	    break;
	case READINTEGERproc:
	    emitInstr0(READINT);
	    break;
	case READREALproc:
	    emitInstr0(READREAL);
	    break;
	case READCHARproc:
	    emitInstr0(READCHAR);
	    break;
	default:
	    internalError("bad call to selectBuiltinProcCode");
	    break;
    }
}

static void handleWriteString(stmntRec *sr)
{
    exprRec *arg;
    char *spell;
    int strLocation, i;
    arg = sr->actualParams->param;
    spell = arg->constVal->val->spell;
    strLocation = getNextStringAddr();
    i = 0;
    do {
	insertStringChar(spell[i]);
    } while (spell[i++] != '\0');
    emitInstr1(LOADIMMED, strLocation);
    emitInstr1(LOADIMMED, 1);
    emitInstr1(LOADIMMED, i - 1);   /* String length */
    emitInstr0(WRITESTRING);
}

static void selectProcStatementCode(stmntRec *sr, procRec *inProc)
{
    procRec *p;
    
    varListRec *temp;
    p = sr->sData->p;
    

    if (p->pKind == BUILTINPROC) {
	if (p->whichBuiltin == WRITESTRINGproc)
	    handleWriteString(sr);
	else {
	    selectActualParamListCode(sr->actualParams, p->params);
	    selectBuiltinProcCode(p->whichBuiltin);
	}
    } else if (p->pKind == USERDEFINEDPROC) {
      emitMarkOrCall(MARK,p);
   
      selectActualParamListCode(sr->actualParams, p->params);
      emitMarkOrCall(CALL,p);
     
      if (p->level < inProc->level+1)
	emitInstr2(FIXDISPLAY,p->level,inProc->level+1);


    } else {
	internalError("bad call to selectProcStatementCode");
    }
}

static void selectStatementListCode(stmntList *lst, procRec *inProc)
{
    while (lst != NULL) {
	selectStatementCode(lst->stmnt, inProc);
	lst = lst->next;
    }


}
