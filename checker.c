#include <stdio.h>         /* Is this needed when printf removed? */
#include <stdlib.h>
#include <string.h>
#include "syntaxTree.h"
#include "check.h"
#include "typeinfo.h"
#include "symtab.h"
#include "tools.h"
#include "grammar.tab.h"
#include "checkerTools.h"
#include "stdObjects.h"
#include <errno.h>
/* *********************************************************************** */
/*                               Variables                                 */
/* *********************************************************************** */
procListRec *builtinProcs;
typeRec *integerType;
typeRec *realType;
typeRec *boolType;
typeRec *charType;
typeRec *stringType; 

static int currentLevel;

procListRec *userDefProcs;  /* List of all procs in program, including main */
			    /* The value of  userDefProcs  is returned      */
			    /* by  checkProgram                             */

/* *********************************************************************** */
/*                               Prototypes                                */
/* *********************************************************************** */

static void initChecker(void);

static typeRec *checkTypeDenoter(typeDenoter *td);
static typeRec *checkNameTypeDenoter(typeDenoter *td);
static void evalSignedIntRec(signedIntRec rec, int *pos, int *val);
static typeRec *checkArrayTypeDenoter(typeDenoter *td);

static void checkVarDec(varDecRec *vdr, int scopeStartTime,
			varListRec **pVLst);
static varListRec *checkVarDecList(varDecList *vdLst, int scopeStartTime);

static void checkProcDec(procDec *proc, int scopeStartTime);
static void checkProcDecList(procList *procs, int scopeStartTime);

static void checkVarAccess(varAccessRec *var);
static void checkEntireVarAccess(varAccessRec *var);
static void checkIndexedVarAccess(varAccessRec *var);

static void checkExpression(exprRec *er);
static void checkConstExpression(exprRec *er);
static void checkVarExpression(exprRec *er);
static void checkUnaryOpExpression(exprRec *er);
static void checkBinOpExpression(exprRec *er);

static void checkStatement(stmntRec *sr);
static void checkNullStatement(stmntRec *sr);
static void checkAssignmentStatement(stmntRec *sr);
static void checkCompoundStatement(stmntRec *sr);
static void checkIfStatement(stmntRec *sr);
static void checkWhileStatement(stmntRec *sr);

static void checkStmntList(stmntList *sLst);

static void checkBlock(blockRec *br, procRec *currProc, int scopeStartTime);

static varListRec* checkParams(formalParamList *pList,int scopeTime);
static void checkParam(formalParamSection *fpl,int scopeTime,varListRec** pVLst);
static typeRec *checkRecordTypeDenoter(typeDenoter *td);
static fieldInfoRec *checkFields(typeDenoter *td);
static void addToFieldList(fieldInfoRec **fir,fieldInfoRec **temp,
                             char *name,typeRec* tRec);

static void checkTypes(typeDefList *tdl,int scopeStartTime);
static void checkFieldVarAccess(varAccessRec *var);
static void checkFunCallExpression(exprRec *er);
static void checkProcedureStatement(stmntRec *sr);
static int getPosOfErr(exprRec *er);

/* *********************************************************************** */
/*                Procedure to call to start type checking                 */
/* *********************************************************************** */

procListRec *checkProgram(programRec *prog)
{
    procRec *mainProc;
    int startTime;
    initChecker();
    mainProc = makeUserDefProc("", currentLevel);
    addToProcList(mainProc, &userDefProcs);
    addScopeLevel();
    currentLevel++;
    startTime = getCurrentTime();
    checkBlock(prog->body, mainProc, startTime);
    return userDefProcs;
}

/* *********************************************************************** */
/*                             Initialization                              */
/* *********************************************************************** */

static void initChecker(void)
{
    userDefProcs = NULL;
    builtinProcs = NULL;
    currentLevel = OUTERSCOPELEVEL;
    initSymtab();
    initStdTypes();
    initStdProcs();
}

/* *********************************************************************** */
/*                        Type definitions                                 */
/* *********************************************************************** */

static typeRec *checkTypeDenoter(typeDenoter *td)
{
    switch (td->kind) {
	case NAMEDENOTER:
	    return checkNameTypeDenoter(td);
	case ARRAYDENOTER:
	    return checkArrayTypeDenoter(td);
	case RECORDDENOTER:
	  return checkRecordTypeDenoter(td);
	default:
	    internalError("bad call to checkTypeDenoter");
    }
}

static typeRec *checkNameTypeDenoter(typeDenoter *td)
{
    int ok;
    symtabInfo info;
    ok = searchSymtab(td->name->spell, &info);
    if ( ! ok )
	errorInInputToCompiler("undefined identifier",
			       td->name->location.firstChar);
    if (info.kind != TYPERECPOINTER)
	errorInInputToCompiler("type name expected",
			       td->name->location.firstChar);
    return info.item.t;
}

static void evalSignedIntRec(signedIntRec rec, int *pos, int *val)
{
    int unsignedVal;
    int z;
    double d;
    if (rec.sign == NULL)
	*pos = rec.valToken->location.firstChar;
    else
	*pos = rec.sign->location.firstChar;
    unsignedVal = atoi(rec.valToken->spell);
    if (rec.sign != NULL && rec.sign->tType == MINUS)
	*val = -unsignedVal;
    else
	*val = unsignedVal;
    
    z= strtol(rec.valToken->spell,NULL,10);
    if (errno==ERANGE)
	errorInInputToCompiler("out of range constant"
			       ,rec.valToken->location.firstChar);
   
    
    
}

static typeRec *checkArrayTypeDenoter(typeDenoter *td)
{
    int pos1, pos2, val1, val2;
    typeRec *compType;
    evalSignedIntRec(*td->range->lowBound, &pos1, &val1);
    evalSignedIntRec(*td->range->highBound, &pos2, &val2);
    if (val1 > val2)
	errorInInputToCompiler("lower limit larger than upper limit",
			       pos1);
    compType = checkTypeDenoter(td->componentType);
    return makeArrayType(val1, val2, compType);
}

static typeRec *checkRecordTypeDenoter(typeDenoter *td){
  typeRec *t;
  addScopeLevel();
  t= makeRecordType(checkFields(td));
  removeScopeLevel();
  return t;
}

static fieldInfoRec *checkFields(typeDenoter *td){
  fieldInfoRec *fir,*temp;
  typeRec *tRec;
  idListRec *ids;
  fieldList *fl;
  char *name;
  symtabInfo s;
  int time;
  int currtime;
  currtime = getCurrentTime();
  
  temp=fir=NULL;
  fl = td->theFields;
  while(fl!=NULL){
    tRec = checkTypeDenoter(fl->recSect->theType);
    ids = fl->recSect->names;
    while(ids!=NULL){
      name = ids->theIdent->spell;
      time = getLastLookupTime(name);
      if (time>currtime)
	errorInInputToCompiler("The name of the field has been used as a type",
			       ids->theIdent->location.firstChar); 



      if (definedInCurrentLevel(name)){
        errorInInputToCompiler("The name of the field has been used",
                               ids->theIdent->location.firstChar);
      }
      addToFieldList(&fir,&temp,name,tRec);
      insertInSymtab(name,s);
      ids= ids->next;
    }
    fl = fl->next;
  }

  temp=fir;

  return fir;
}

static void addToFieldList(fieldInfoRec **fir,fieldInfoRec **temp,
			     char *name,typeRec* tRec){
   if (*fir==NULL){
    *fir=makeFieldInfoRec(name,tRec,NULL);
    *temp=*fir;
  }else{
    (*temp)->next= makeFieldInfoRec(name,tRec,NULL);
    *temp = (*temp)->next;
  }
}

/* *********************************************************************** */
/*                       Variable declarations                             */
/* *********************************************************************** */

static void checkVarDec(varDecRec *vdr, int scopeStartTime,
			varListRec **pVLst)
{
    idListRec *ids;
    char *name;
    varInfoRec *vr;
    typeRec *tRec;
    int time;
    ids = vdr->ids;
    tRec = checkTypeDenoter(vdr->td);
    while (ids != NULL) {
      name = ids->theIdent->spell;
      time = getLastLookupTime(name);
      if (time>scopeStartTime)
	errorInInputToCompiler("The name of the variable has been used as a type",
			       ids->theIdent->location.firstChar); 

      if (definedInCurrentLevel(name)){
	errorInInputToCompiler("The name of the variable has been used",
			       ids->theIdent->location.firstChar);
      }
      vr = makeLocalVarInfoRec(name, tRec, currentLevel);
      putVarIntoSymtab(name, vr);
      addToVarList(vr, pVLst);
      ids = ids->next;
    }
}

static varListRec *checkVarDecList(varDecList *vdLst, int scopeStartTime)
{
    varDecList *lst;
    varListRec *chkLst;
    lst = vdLst;
    chkLst = NULL;
    while (lst != NULL) {
	checkVarDec(lst->dec, scopeStartTime, &chkLst);
	lst = lst->next;
    }
    return chkLst;
}

static void checkTypes(typeDefList *tdl,int scopeStartTime){
  typeDefList *temp;
  typeRec *tRec;
  int time;
  temp = tdl;
  
  while(temp!=NULL){
    tRec=checkTypeDenoter(temp->def->theType);

    time = getLastLookupTime(temp->def->name->spell);
    if (time>scopeStartTime)
      errorInInputToCompiler("The name of the type has been used as a type",
			       temp->def->name->location.firstChar);
    
    if (definedInCurrentLevel(temp->def->name->spell)){
      errorInInputToCompiler("The name of the type has already been used",
			     temp->def->name->location.firstChar);
    }
    putTypeIntoSymtab(temp->def->name->spell,tRec);
    temp=temp->next;
  }

}

/* *********************************************************************** */
/*                       Procedure declarations                            */
/* *********************************************************************** */

static void checkProcDec(procDec *proc, int scopeStartTime)
{
    procRec *currProc;
    int startTime,ok;
    symtabInfo s;
    char *name;
    int time;

    currProc = makeUserDefProc(proc->heading->procName->spell, currentLevel);
    name = proc->heading->procName->spell;

    time = getLastLookupTime(name);
    if (time>scopeStartTime)
	errorInInputToCompiler("The name of the procedure has been used as a type",
			        proc->heading->procName->location.firstChar);

    if (definedInCurrentLevel(name)){
      errorInInputToCompiler("The name of the procedure is already used",
			      proc->heading->procName->location.firstChar);
    }
    putProcIntoSymtab(proc->heading->procName->spell, currProc);
    addToProcList(currProc, &userDefProcs);
    addScopeLevel();
    currentLevel++;
    startTime = getCurrentTime();
    currProc->params = checkParams(proc->heading->pList,startTime);
    if (proc->block==NULL)
      miscError("forwarded function are not yet implemented",NULL); 
    checkBlock(proc->block, currProc, startTime);
    currentLevel--;
    removeScopeLevel();
}

static varListRec* checkParams(formalParamList *pList,int scopeTime){
  formalParamList *lst;
  varListRec* chklst;
  lst= pList;
  chklst=NULL;
  while(lst!= NULL){
    checkParam(lst->pSect,scopeTime,&chklst);
    lst=lst->next;
  }
  return chklst;
}

static void checkParam(formalParamSection *fpl,int scopeTime,varListRec** pVLst){
    idListRec *ids;
    char *name;
    varInfoRec *vr;
    typeRec *tRec;
    int time;

    ids = fpl->ids;
    tRec = checkTypeDenoter(fpl->td);
    if (fpl->isVarParamSpec){
      while (ids != NULL) {
	name = ids->theIdent->spell;
	time = getLastLookupTime(name);
	if (time>scopeTime)
	  errorInInputToCompiler("The name of the parameter has been used as a type",
				 ids->theIdent->location.firstChar); 

	if (definedInCurrentLevel(name)){
	  errorInInputToCompiler("The name of the parameter has already been used",
				 ids->theIdent->location.firstChar);
	}
	vr = makeVarParamInfoRec(name, tRec, currentLevel);
	putVarIntoSymtab(name, vr);
	addToVarList(vr, pVLst);
	ids = ids->next;
      }
    }else {
      while (ids != NULL) {
	name = ids->theIdent->spell;
	time = getLastLookupTime(name);
	if (time>scopeTime)
	  errorInInputToCompiler("The name of the parameter has been used as a type",
				 ids->theIdent->location.firstChar);


	if (definedInCurrentLevel(name)){
	  errorInInputToCompiler("The name of the parameter has already been used",
				 ids->theIdent->location.firstChar);
	}
	vr = makeValueParamInfoRec(name, tRec, currentLevel);
	putVarIntoSymtab(name, vr);
	addToVarList(vr, pVLst);
	ids = ids->next;
      }
    }
}



static void checkProcDecList(procList *procs, int scopeStartTime)
{
    procList *lst;
    lst = procs;
    while (lst != NULL) {
	checkProcDec(lst->dec, scopeStartTime);
	lst = lst->next;
    }
}

/* *********************************************************************** */
/*                         Variable accesses                               */
/* *********************************************************************** */

static void checkVarAccess(varAccessRec *var)
{
    switch (var->kind) {
	case ENTIREVAR:
	    checkEntireVarAccess(var);
	    break;
	case INDEXEDVAR:
	    checkIndexedVarAccess(var);
	    break;
	case FIELDVAR:
	  checkFieldVarAccess(var);
	    break;
	default:
	    internalError("bad call to checkVarAccess");
	    break;
    }
}

static void checkEntireVarAccess(varAccessRec *var)
{
    int ok;
    symtabInfo info;
    ok = searchSymtab(var->name->spell, &info);
    if ( ! ok )
	errorInInputToCompiler("undefined identifier",
			       var->name->location.firstChar);
    if (info.kind != VARRECPOINTER)
	errorInInputToCompiler("variable name expected",
			       var->name->location.firstChar);
    var->vData = makeVarData(info.item.v->vType, info.item.v);
}

static void checkIndexedVarAccess(varAccessRec *var)
{
    checkVarAccess(var->baseVar);
    if (var->baseVar->vData->t->tt != ARRAYTYPE)
	errorInInputToCompiler("array variable needed for indexing",
			       var->baseVar->startPos);
    checkExpression(var->index);
    if (var->index->eData->t != integerType)
	errorInInputToCompiler("integer expression needed as index",
			       var->index->startPos);
    var->vData = makeVarData(var->baseVar->vData->t->componentType, NULL);
}

static void checkFieldVarAccess(varAccessRec *var)
{
  int found;
  fieldInfoRec *temp;
  checkVarAccess(var->baseVar);
  if (var->baseVar->vData->t->tt != RECORDTYPE)
    errorInInputToCompiler("record type variable needed for using fields",
			   var->baseVar->startPos);

  found=0;
  temp = var->baseVar->vData->t->fi;
  while(temp!=NULL){
    if (!(strcmp(temp->fieldName,var->fieldName->spell))){
      found=1;
      break;
    }
    temp = temp->next;
  }
  if (!found)
    errorInInputToCompiler("Not a field in the record",
                           var->baseVar->startPos);

  var->vData = makeVarData(temp->fieldType,NULL);
}
/* *********************************************************************** */
/*                            Expressions                                  */
/* *********************************************************************** */

static void checkExpression(exprRec *er)
{
    switch (er->kind) {
	case CONSTEXPR:
	    checkConstExpression(er);
	    break;
	case VAREXPR:
	    checkVarExpression(er);
	    break;
	case UNARYOPEXPR:
	    checkUnaryOpExpression(er);
	    break;
	case BINARYOPEXPR:
	    checkBinOpExpression(er);
	    break;
	case FUNCALLEXPR:
	    checkFunCallExpression(er);
	    break;
	default:
	    internalError("bad call to checkExpression");
	    break;
    }
}

static void checkConstExpression(exprRec *er)
{
  int z;
  double x;
    switch (er->constVal->val->tType) {
	case INTCONST:
	  z=strtol(er->constVal->val->spell,NULL,10);
	  if (errno==ERANGE)
	    errorInInputToCompiler("out of range constant",
				   er->constVal->val->location.firstChar);
				   
	  er->eData = makeIntConstExprData(atoi(er->constVal->val->spell));
	  break;
	case REALCONST:
	  x=strtol(er->constVal->val->spell,NULL,10);
          if (errno==ERANGE)
            errorInInputToCompiler("out of range constant",
                                   er->constVal->val->location.firstChar);
	    er->eData = makeRealConstExprData(atof(er->constVal->val->spell));
	    break;
	case TRUESYM:
	    er->eData = makeBooleanConstExprData(1);
	    break;
	case FALSESYM:
	    er->eData = makeBooleanConstExprData(0);
	    break;
	case STRINGCONST:
	    er->eData = makeStrConstExprData(er->constVal->val->spell);
	    break;
	default:
	    internalError("bad call to checkConstExpression");
	    break;
    }
}

static void checkVarExpression(exprRec *er)
{
    checkVarAccess(er->theVar);
    er->eData = makeOtherExprData(er->theVar->vData->t);
}

static void checkUnaryOpExpression(exprRec *er)
{
    typeRec *t1;
    checkExpression(er->left);
    t1 = er->left->eData->t;
    switch (er->op->tType) {
	case PLUS:
	case MINUS:
	    if (t1 == integerType || t1 == realType)
		er->eData = makeOtherExprData(t1);
	    else
		errorInInputToCompiler("operator expects numeric operand",
				       er->op->location.firstChar);
	    break;
	case NOTSYM:
	    if (t1 == boolType)
		er->eData = makeOtherExprData(t1);
	    else
		errorInInputToCompiler("operator expects boolean operand",
				       er->op->location.firstChar);
	    break;
	default:
	    internalError("bad call to checkUnaryOpExpression");
	    break;
    }
}

static void checkBinOpExpression(exprRec *er)
{
    typeRec *t1, *t2;
    checkExpression(er->left);
    checkExpression(er->right);
    t1 = er->left->eData->t;
    t2 = er->right->eData->t;
    switch (er->op->tType) {
        case TIMES:
        case MINUS:
	case PLUS:
	    if ((t1==t2) && (t1 == integerType || 
			     t1 == realType))
	      er->eData = makeOtherExprData(t1);
	    else if (t1 == realType && t2 == integerType)
	      er->eData = makeOtherExprData(t1);
	    else if (t2 == realType && t1 == integerType)
	      er->eData = makeOtherExprData(t2);
	    else
		errorInInputToCompiler("operator with bad operand type(s)",
				       er->op->location.firstChar);
	    break;
        case SLASH:
	  if ((t1 == integerType || t1 == realType) && 
	      (t2 == integerType || t2 == realType))
	    er->eData = makeOtherExprData(realType);
	  else 
	    errorInInputToCompiler("operator with bad operand type(s)",
				      er->op->location.firstChar);
	  break;
        case DIVSYM:
        case MODSYM:
	  if (t1 == integerType && t2 == integerType)
	    er->eData = makeOtherExprData(t1);
	  else
	    errorInInputToCompiler("operator with bad operand type(s)",
				      er->op->location.firstChar);
	  break;
        case ORSYM:
	case ANDSYM:
	    if (t1 == t2 && t1 == boolType)
		er->eData = makeOtherExprData(t1);
	    else
		errorInInputToCompiler("operator with bad operand type(s)",
				       er->op->location.firstChar);
	    break;
        case NEQ:
        case EQ:
        case LT:
        case GT:
        case GEQ:
        case LEQ:
	  if (((t1==t2) && (t1==boolType)) || 
	      ((t1==t2) && (t1==charType)) ||
	      (t2==charType && t1==stringType && er->left->eData->stringLeng==1)||
	      (t1==charType && t2==stringType && er->right->eData->stringLeng==1)||
	      (t1==stringType && er->left->eData->stringLeng==1 &&
	       t2==stringType && er->right->eData->stringLeng==1) ||
	      ((t1==integerType || t1== realType)&& 
	       (t2==integerType || t2== realType)))
	    er->eData = makeOtherExprData(boolType);
	  else
	      errorInInputToCompiler("operator with bad operand type(s)",
				     er->op->location.firstChar);
	  break;
	default:
	    internalError("bad call to checkBinOpExpression");
	    break;
    }
}

static void checkFunCallExpression(exprRec *er){
  int ok;
  symtabInfo s;
  actualParamList *z;
  varListRec *n;
  typeRec *t1,*t2;

  ok = searchSymtab(er->name->spell,&s);
  if (!ok){
    errorInInputToCompiler("undefined Function",
			   er->name->location.firstChar);
  }

  if (s.kind != PROCRECPOINTER)
    errorInInputToCompiler("function name expected",
                           er->name->location.firstChar);

  
  if (s.item.p->pKind!=BUILTINFUNCTION)
    errorInInputToCompiler("This is a procedure not a function",
			   er->name->location.firstChar);

  z= er->actualParams;
  n= s.item.p->params;
  while (z!=NULL){
    if (n==NULL)
      errorInInputToCompiler("wrong number of arguments",
			     er->name->location.firstChar);
  
    if ((n->theVar->vKind==VARPARAM)&&
	(z->param->kind!=VAREXPR)){
	errorInInputToCompiler("a variable is required for this parameter"
			       ,getPosOfErr(z->param));

    }
  
    checkExpression(z->param);
    t1=z->param->eData->t;
    t2=n->theVar->vType;

    if (t1!=t2){
      if ((t1==stringType  && t2==charType && z->param->eData->stringLeng==1)
	  || (t1==integerType && t2==realType && n->theVar->vKind!=VARPARAM));
      else
      errorInInputToCompiler("Type error",getPosOfErr(z->param));
    }


    n=n->next;
    z=z->next;

  }

  if (n!=NULL)
    errorInInputToCompiler("wrong number of arguments",
			   er->name->location.firstChar);

  er->eData=makeFunCallExprData(s.item.p->returnType,s.item.p);

}

static int getPosOfErr(exprRec *er){
  varAccessRec *temp;
  switch (er->kind){
  case CONSTEXPR:
    return er->constVal->val->location.firstChar;

  case VAREXPR:
    if (er->theVar->kind == ENTIREVAR)
      return er->theVar->name->location.firstChar;
    else {
      temp = er->theVar->baseVar;
      while(temp->name==NULL) temp=temp->baseVar;
      return temp->name->location.firstChar;
    }

  case UNARYOPEXPR:
  case BINARYOPEXPR:
    return er->op->location.firstChar;

  case FUNCALLEXPR:
    return er->name->location.firstChar;
  }

}


/* *********************************************************************** */
/*                            Statements                                   */
/* *********************************************************************** */

static void checkStatement(stmntRec *sr)
{
    switch (sr->kind) {
	case NULLSTMNT:
	    checkNullStatement(sr);
	    break;
	case ASSIGNSTMNT:
	    checkAssignmentStatement(sr);
	    break;
	case COMPOUNDSTMNT:
	    checkCompoundStatement(sr);
	    break;
	case IFSTMNT:
	    checkIfStatement(sr);
	    break;
	case WHILESTMNT:
	    checkWhileStatement(sr);
	    break;
	case PROCSTMNT:
	    checkProcedureStatement(sr);
	    break;
	default:
	    internalError("bad call to checkStatement");
	    break;
    }
}

static void checkProcedureStatement(stmntRec *sr){
  int ok;
  symtabInfo s;
  actualParamList *z;
  varListRec *n;
  typeRec *t1,*t2;

  ok = searchSymtab(sr->procName->spell,&s);
  if (!ok){
    errorInInputToCompiler("undefined Procedure",
                           sr->procName->location.firstChar);
  }

  if (s.kind != PROCRECPOINTER)
    errorInInputToCompiler("procedure name expected",
                           sr->procName->location.firstChar);

  if (s.item.p->pKind==BUILTINFUNCTION)
    errorInInputToCompiler("This is a function not a procedure",
                           sr->procName->location.firstChar);

  z= sr->actualParams;
  n= s.item.p->params;

  while (z!=NULL){
    if (n==NULL)
      errorInInputToCompiler("wrong number of arguments",
                             sr->procName->location.firstChar);

    if  ((n->theVar->vKind==VARPARAM)&& 
	 (z->param->kind==VAREXPR)){
      if (z->param->startPos!=z->param->theVar->startPos)
	 errorInInputToCompiler("a variable access is required for this parameter Not expression"
			     ,getPosOfErr(z->param));
    }

    if ((n->theVar->vKind==VARPARAM)&&
        (z->param->kind!=VAREXPR))
      errorInInputToCompiler("a variable is required for this parameter"
			     ,getPosOfErr(z->param));

    

    checkExpression(z->param);
    t1=z->param->eData->t;
    t2=n->theVar->vType;

    if (t1!=t2){
      if ((t1==stringType  && t2==charType && z->param->eData->stringLeng==1)
	   || (t1==integerType && t2==realType && n->theVar->vKind!=VARPARAM));
      else
	errorInInputToCompiler("Type error", getPosOfErr(z->param));
    }

    n=n->next;
    z=z->next;

  }

  if (n!=NULL)
    errorInInputToCompiler("wrong number of arguments",
                           sr->procName->location.firstChar);

  sr->sData= makeProcCallStmntData(s.item.p);

}

static void checkNullStatement(stmntRec *sr)
{
    /*  Nothing to do  */
}

static void checkAssignmentStatement(stmntRec *sr)
{
    typeRec *leftType, *rightType;
    exprData *eData;
    checkVarAccess(sr->left);
    checkExpression(sr->right);
    leftType = sr->left->vData->t;
    eData = sr->right->eData;
    rightType = eData->t;
    if (( leftType == rightType )||(leftType==charType && rightType==stringType && eData->stringLeng==1)
	|| (leftType==realType && rightType==integerType)) 
	; /* no error */
    else
	errorInInputToCompiler("type error in assignment statement",
			       sr->left->startPos);
}

static void checkCompoundStatement(stmntRec *sr)
{
    checkStmntList(sr->stmnts);
}

static void checkIfStatement(stmntRec *sr)
{

  checkExpression(sr->testExpr);
  if (sr->testExpr->eData->t!=boolType)
    errorInInputToCompiler("test expression should be boolean in if statement",
                           getPosOfErr(sr->testExpr));

  checkStatement(sr->thenPart);
  checkStatement(sr->elsePart);
}

static void checkWhileStatement(stmntRec *sr)
{
  checkExpression(sr->testExpr);
  if (sr->testExpr->eData->t!=boolType)
    errorInInputToCompiler("test expression should be boolean in while statement",
			   getPosOfErr(sr->testExpr));
  
  checkStatement(sr->body);
}


static void checkStmntList(stmntList *sLst)
{
    stmntList *lst;
    lst = sLst;
    while (lst != NULL) {
	checkStatement(lst->stmnt);
	lst = lst->next;
    }
}

/* *********************************************************************** */
/*                                Blocks                                   */
/* *********************************************************************** */

static void checkBlock(blockRec *br, procRec *currProc, int scopeStartTime)
{
    varListRec *vars;
    checkTypes(br->typeDefs,scopeStartTime);
    currProc->localVars = checkVarDecList(br->varDecs, scopeStartTime);

    checkProcDecList(br->procDecs, scopeStartTime);
    checkStatement(br->stmntPart);
    currProc->body = br->stmntPart;
}
