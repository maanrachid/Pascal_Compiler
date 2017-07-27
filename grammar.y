%{/*   */
#include <stdio.h>
#include "scan.h"
#include "parse.h"
#include "tools.h"
#include "syntaxTree.h"

extern int charsRead;
programRec *theprogram;
/**/

%}

%union {
  tokenRec *val;
  programRec *prog;
  idListRec *idlist;
  indexRangeRec *inrgerec;
  blockRec *blrec;
  typeDefList *tdeflst;
  typeDefRec *tdefrec;
  typeDenoter *tdenoter;
  signedIntRec *sintrec;
  recordSectionRec *recsecrec;
  varDecRec *vardecrec;
  varDecList *vardeclst;
  formalParamSection *frmprmsec;
  formalParamList *frmprmlst;
  procHeading *prochead;
  procDec *procdec;
  procList *proclst;
  exprRec *expr;
  actualParamList *actparlst;
  stmntRec *stat;
  varAccessRec *varaccrec;
  fieldList *fldlst;
  stmntList *stmlst;
} 



%token PLUS MINUS TIMES SLASH LPAREN RPAREN LBRACKET RBRACKET
%token EQ NEQ LT LEQ GT GEQ
%token COLON ASSIGNOP PERIOD DOTS COMMA SEMICOLON
%token ANDSYM ARRAYSYM BEGINSYM DIVSYM
%token DOSYM ELSESYM ENDSYM FORWARDSYM
%token IFSYM MODSYM NOTSYM OFSYM ORSYM PROCSYM
%token PROGSYM RECORDSYM THENSYM TYPESYM
%token VARSYM WHILESYM
%token IDENT
%token INTCONST REALCONST STRINGCONST
%token EOFTOKEN FALSESYM TRUESYM


%type <val> PLUS MINUS TIMES SLASH LPAREN RPAREN LBRACKET RBRACKET
%type <val> EQ NEQ LT LEQ GT GEQ
%type <val> COLON ASSIGNOP PERIOD DOTS COMMA SEMICOLON
%type <val> ANDSYM ARRAYSYM BEGINSYM DIVSYM
%type <val> DOSYM ELSESYM ENDSYM FORWARDSYM
%type <val> IFSYM MODSYM NOTSYM OFSYM ORSYM PROCSYM
%type <val> PROGSYM RECORDSYM THENSYM TYPESYM
%type <val> VARSYM WHILESYM
%type <val> IDENT
%type <val> INTCONST REALCONST STRINGCONST
%type <val> EOFTOKEN FALSESYM TRUESYM
%type <val> sign relational_operator multiplying_operator adding_operator
%type <fldlst> record_sections record_sections_help field_list 
%type <varaccrec> indexed_variable field_designator variable_access expressions
%type <varaccrec> component_variable
%type <stat> statement simple_statement compound_statement assignment_statement
%type <stat> structured_statment procedure_statement 
%type <stmlst> statement_sequence 
%type <expr> factor term expression simple_expression  
%type <expr> actual_parameter function_designator simple_expression_help
%type <actparlst> actual_parameters actual_parameter_list 
%type <proclst> procedure_declaration_part
%type <procdec> procedure_declaration
%type <prochead> procedure_heading
%type <frmprmlst> formal_parameter_sections formal_parameter_list
%type <frmprmsec> formal_parameter_section variable_parameter_specification 
%type <frmprmsec> value_parameter_specification 
%type <inrgerec> index_range
%type <vardeclst> variable_declarations variable_declaration_part
%type <vardecrec> variable_declaration
%type <sintrec> signed_integer
%type <tdefrec> type_definition
%type <tdeflst> type_definitions type_definition_part 
%type <blrec> block
%type <prog> program
%type <idlist> identifier_list
%type <tdenoter> type_denoter array_type record_type comma_indexranges 
%type <recsecrec> record_section



%%

main_thing:	program { theprogram = $1 ;YYACCEPT; }
        ;

identifier_list:  IDENT {$$ = makeIdListRec($1,NULL);} 
                | IDENT COMMA identifier_list {$$ =  makeIdListRec($1,$3);}
                  
        ;

signed_integer:   sign INTCONST {$$ = makeSignedIntRec($1,$2);}
| INTCONST { $$ = makeSignedIntRec(NULL,$1);}
;

index_range:      signed_integer DOTS signed_integer
{$$ =makeIndexRangeRec($1,$3);}   
        ;

type_denoter:     IDENT { $$ = makeNameTypeDenoter($1);}

| array_type 

| record_type 
        ;

comma_indexranges:  index_range RBRACKET OFSYM type_denoter { $$ =  makeArrayTypeDenoter($1,$4);}
| index_range COMMA comma_indexranges {$$= makeArrayTypeDenoter($1,$3);}
;

array_type:       ARRAYSYM LBRACKET comma_indexranges { $$ = $3;}
;


sign:		  PLUS
		| MINUS
;

record_section:    identifier_list COLON type_denoter
{$$ = makeRecordSectionRec($1,$3);}
        ;


record_sections:   record_section SEMICOLON {$$ = makeFieldList($1,NULL);}


| record_section SEMICOLON record_sections {$$ = makeFieldList($1,$3);} 
        ;


record_sections_help: record_section {$$ = makeFieldList($1,NULL);} 
| record_section SEMICOLON  record_sections_help {$$ = makeFieldList($1,$3);}
;

field_list:        { $$ = NULL;} | record_sections_help
                   | record_sections 
;

record_type:       RECORDSYM field_list ENDSYM
{ $$ =makeRecordTypeDenoter($2);}
         ;


type_definition:   IDENT EQ type_denoter { $$ = makeTypeDefRec($1,$3);}
        ;

type_definitions:    type_definition SEMICOLON 
{$$ = makeTypeDefList($1,NULL);}

| type_definition SEMICOLON type_definitions
{$$ = makeTypeDefList($1,$3);}
;



type_definition_part:   { $$ = NULL;} | TYPESYM  type_definitions {$$ = $2;}
;


variable_declaration:   identifier_list COLON type_denoter
{ $$ = makeVarDecRec($1,$3);}
;

variable_declarations:  variable_declaration SEMICOLON { $$ =makeVarDecList($1,NULL); }
| variable_declaration SEMICOLON  variable_declarations { $$ =makeVarDecList($1,$3);}
                 ;

variable_declaration_part:   { $$ = NULL;} | VARSYM  variable_declarations {$$ =$2;}
                 ;

value_parameter_specification:  identifier_list COLON IDENT
{$$ = makeFormalParamSection(0,$1,makeNameTypeDenoter($3));}
;

variable_parameter_specification:  VARSYM identifier_list COLON IDENT
{$$ = makeFormalParamSection(1,$2,makeNameTypeDenoter($4));}
;

formal_parameter_section:    value_parameter_specification
                             | variable_parameter_specification
;

formal_parameter_sections:   formal_parameter_section 
{ $$ = makeFormalParamList ($1,NULL);}


| formal_parameter_section SEMICOLON formal_parameter_sections 
{ $$ = makeFormalParamList ($1,$3);}
;

formal_parameter_list:       LPAREN formal_parameter_sections RPAREN
{$$ =$2;}
;

procedure_heading:           PROCSYM IDENT formal_parameter_list
{ $$ = makeProcHeading($2, $3);}
                             |  PROCSYM IDENT
{$$ = makeProcHeading($2,NULL);}
;

block:                       type_definition_part
                                 variable_declaration_part
                                   procedure_declaration_part
                                    compound_statement
                              { $$ = makeBlockRec($1,$2,$3,$4);}
;


procedure_declaration:       procedure_heading SEMICOLON block 
{ $$ = makeProcDec($1,$3);}  
                             | procedure_heading SEMICOLON FORWARDSYM
{ $$ = makeProcDec($1,NULL);}
;



procedure_declaration_part:   { $$ = NULL;} | procedure_declaration SEMICOLON 
                               procedure_declaration_part
{ $$ = makeProcList($1,$3);}
;

expressions:                 variable_access LBRACKET expression 
{ $$ = makeIndexedVarAccessRec($1,$3);}
| expressions COMMA expression
{ $$ = makeIndexedVarAccessRec($1,$3);}
;

indexed_variable:           expressions RBRACKET {$$ = $1;}                              
 

;

field_designator:           variable_access PERIOD IDENT
{$$ = makeFieldVarAccessRec($1,$3);}
;

component_variable:         indexed_variable
                            | field_designator
;

variable_access:            IDENT { $$ = makeEntireVarAccessRec($1);}
                            | component_variable 
;

factor:                     variable_access { $$ = makeVariableExprRec($1);}
|INTCONST { $$ = makeConstExprRec(makeUnsignedConstRec($1));}  
| REALCONST  { $$ = makeConstExprRec(makeUnsignedConstRec($1));} 
| STRINGCONST  { $$ = makeConstExprRec(makeUnsignedConstRec($1));} 
|TRUESYM  { $$ = makeConstExprRec(makeUnsignedConstRec($1));} 
| FALSESYM  { $$ = makeConstExprRec(makeUnsignedConstRec($1));} 
| function_designator 
|LPAREN  expression RPAREN {$$ = $2; $$->startPos=charsRead-1;}
|NOTSYM factor {$$ = makeUnaryOpExprRec($1,$2);} 
;

actual_parameter:           expression 
;

actual_parameters:          actual_parameter { $$ = makeActualParamList($1,NULL);} 
| actual_parameter COMMA actual_parameters
{ $$ = makeActualParamList($1,$3);}
;


actual_parameter_list:      LPAREN  actual_parameters RPAREN { $$= $2;}
;

function_designator:        IDENT actual_parameter_list 
{$$ = makeFunCallExprRec($1,$2);}
                            | IDENT LPAREN RPAREN
{$$ = makeFunCallExprRec($1,NULL);}
;

multiplying_operator:       TIMES | SLASH | DIVSYM | MODSYM | ANDSYM
;


term:                   factor | term multiplying_operator factor
{$$ =makeBinaryOpExprRec($2,$1,$3);}
;

adding_operator:        sign | ORSYM
;

simple_expression_help:  term | 
                         simple_expression_help adding_operator term 
{$$ = makeBinaryOpExprRec($2,$1,$3);}

;

simple_expression:        sign simple_expression_help 
{$$ = makeUnaryOpExprRec($1,$2);} 
                          | simple_expression_help                          
;

relational_operator:      EQ | NEQ | LT | GT| LEQ | GEQ
;

expression:               simple_expression
                          | simple_expression relational_operator simple_expression
{ $$ = makeBinaryOpExprRec($2,$1,$3);}
;

assignment_statement:     variable_access ASSIGNOP expression
{ $$ = makeAssignmentStmntRec($1,$3);}
;

procedure_statement:     IDENT { $$ =makeProcStmntRec($1,NULL);}
| IDENT actual_parameter_list { $$ =makeProcStmntRec($1,$2);}

;

statement_sequence:      statement {$$ = makeStmntList($1,NULL);}
|  statement SEMICOLON statement_sequence 
{ $$ = makeStmntList($1,$3);}
;

compound_statement:      BEGINSYM statement_sequence ENDSYM
{ $$ = makeCompoundStmntRec($2);}
;

simple_statement:         { $$ = makeNullStmntRec();} |assignment_statement| procedure_statement
;

structured_statment:     compound_statement 
                         | IFSYM expression THENSYM statement ELSESYM statement
{ $$ = makeIfStmntRec($2,$4,$6);}
                         | IFSYM expression THENSYM statement
{ $$ = makeIfStmntRec($2,$4,makeNullStmntRec());}
                         | WHILESYM expression DOSYM statement
{ $$ = makeWhileStmntRec($2,$4);}
;

statement:               simple_statement | structured_statment
;


program:                PROGSYM IDENT LPAREN identifier_list RPAREN 
                        SEMICOLON block PERIOD 
                        { $$ = makeProgramRec($2 , $4, $7);}
; 

 


%%




int yyerror(char *s)
{
  programError(s, charsRead);
}

void initParser(void)
{
}

programRec *parseProgram(void)
{
  yyparse();
  return theprogram;
}

