%{/* */
#include <stdio.h>
#include <stdlib.h>



#include "scan.h"
#include "parse.h"
#include "syntaxTree.h"
#include "grammar.tab.h"
#include "tools.h"

int charsRead;     
int danger;      /* a flag for a space between a number and a keyword */ 
int cmt;	/*counter for levels in the comment */
static tokenRec *newToken(tokenType ty, char *spell, int endPos, int leng);
static void checkdanger();

/*
union {

	tokenRec *val;
} yylval;
*/

%}

LETTER	[a-z]|[A-Z]
DIGIT   [0-9]
OTHERS	[!-&]|[(-[]|[]-~]




%x	CMNT


%%


[ \t]		{ charsRead++; danger=0;}

("(*"|"{")	{BEGIN(CMNT);cmt++;danger=0;
			charsRead+=yyleng;}

<CMNT>"}"|"*)"	{cmt--;charsRead+=yyleng; 
				if (cmt==0)
				BEGIN(INITIAL);}
<CMNT>("(*"|"{") {cmt++;charsRead+=yyleng;}
<CMNT><<EOF>>	{errorInInputToCompiler("Unterminated comment", charsRead);}
<CMNT>\n	charsRead++;
<CMNT>.		charsRead++;


"+"		{ charsRead++;danger=0;
                  yylval.val = newToken(PLUS, NULL, charsRead - 1, yyleng);
		  return PLUS;}

"*"             { charsRead++;danger=0;
                  yylval.val = newToken(TIMES, NULL, charsRead - 1, yyleng);
                  return TIMES;}

\/              { charsRead++;danger=0;
                  yylval.val = newToken(SLASH, NULL, charsRead - 1, yyleng);
                  return SLASH; }

\[             { charsRead++;danger=0;
                  yylval.val = newToken(LBRACKET, NULL, charsRead - 1, yyleng);
                  return LBRACKET;}

\]             { charsRead++;danger=0;
                  yylval.val = newToken(RBRACKET, NULL, charsRead - 1, yyleng);
                  return RBRACKET; }


\(              { charsRead++;danger=0;
	    	  yylval.val = newToken(LPAREN, NULL, charsRead - 1, yyleng);
		  return LPAREN; }

\)		{ charsRead++;danger=0;
                  yylval.val = newToken(RPAREN, NULL, charsRead - 1, yyleng);
                  return RPAREN; }	

":="              { charsRead++;danger=0;
                  yylval.val = newToken(ASSIGNOP, NULL, charsRead - 1, yyleng);
                  return ASSIGNOP; }

":"              { charsRead++;danger=0;
                  yylval.val = newToken(COLON, NULL, charsRead - 1, yyleng);
                  return COLON; }

","              { charsRead++;danger=0;
                  yylval.val = newToken(COMMA, NULL, charsRead - 1, yyleng);
                  return COMMA; }

"."              { charsRead++;danger=0;
                  yylval.val = newToken(PERIOD, NULL, charsRead - 1, yyleng);
                  return PERIOD; }

";"              { charsRead++;danger=0;
                  yylval.val = newToken(SEMICOLON, NULL, charsRead - 1, yyleng);
                  return SEMICOLON; }

"<"              { charsRead++;danger=0;
                  yylval.val = newToken(LT, NULL, charsRead - 1, yyleng);
                  return LT; }

">"              { charsRead++;danger=0;
                  yylval.val = newToken(GT, NULL, charsRead - 1, yyleng);
                  return GT; }

-		{ charsRead++;danger=0;
		  yylval.val = newToken(MINUS, NULL, charsRead - 1, yyleng);
		  return MINUS; }

".."            { charsRead += yyleng;danger=0;
                  yylval.val = newToken(DOTS, NULL, charsRead - 1, yyleng);
                  return DOTS; }

">="            { charsRead += yyleng;danger=0;
                  yylval.val = newToken(GEQ, NULL, charsRead - 1, yyleng);
                  return GEQ; }

"and"		 { checkdanger();
		  charsRead += yyleng;
                  yylval.val = newToken(ANDSYM, NULL, charsRead - 1, yyleng);
                  return ANDSYM; }

do              { checkdanger();
		  charsRead += yyleng;
                  yylval.val = newToken(DOSYM, NULL, charsRead - 1, yyleng);
                  return DOSYM; }

else 		 { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(ELSESYM, NULL, charsRead - 1, yyleng);
                  return ELSESYM; }

true		 { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(TRUESYM, NULL, charsRead - 1, yyleng);
                  return TRUESYM; }

false		{ checkdanger();charsRead += yyleng;
                  yylval.val = newToken(FALSESYM, NULL, charsRead - 1, yyleng);
                  return FALSESYM; }

end		 { checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(ENDSYM, NULL, charsRead - 1, yyleng);
                  return ENDSYM; }

forward		 { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(FORWARDSYM, NULL, charsRead - 1, yyleng);
                  return FORWARDSYM; }

if               { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(IFSYM, NULL, charsRead - 1, yyleng);
                  return IFSYM; }

not               { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(NOTSYM, NULL, charsRead - 1, yyleng);
                  return NOTSYM; }

mod               { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(MODSYM, NULL, charsRead - 1, yyleng);
                  return MODSYM; }

of               { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(OFSYM, NULL, charsRead - 1, yyleng);
                  return OFSYM; }

or               {checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(ORSYM, NULL, charsRead - 1, yyleng);
                  return ORSYM; }

procedure         {checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(PROCSYM, NULL, charsRead - 1, yyleng);
                  return PROCSYM; }

program           {checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(PROGSYM, NULL, charsRead - 1, yyleng);
                  return PROGSYM; }

record           { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(RECORDSYM, NULL, charsRead - 1, yyleng);
                  return RECORDSYM; }

type             { checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(TYPESYM, NULL, charsRead - 1, yyleng);
                  return TYPESYM; }

var              { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(VARSYM, NULL, charsRead - 1, yyleng);
                  return VARSYM; }

while		 { checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(WHILESYM, NULL, charsRead - 1, yyleng);
                  return WHILESYM; }

array            { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(ARRAYSYM, NULL, charsRead - 1, yyleng);
                  return ARRAYSYM; }

begin            { checkdanger();charsRead += yyleng;
                  yylval.val = newToken(BEGINSYM, NULL, charsRead - 1, yyleng);
                  return BEGINSYM; }

div            { checkdanger(); charsRead += yyleng;
                  yylval.val = newToken(DIVSYM, NULL, charsRead - 1, yyleng);
                  return DIVSYM; }


"<="            { charsRead += yyleng; danger=0;
                  yylval.val = newToken(LEQ, NULL, charsRead - 1, yyleng);
                  return LEQ; }

"="		{ charsRead += yyleng; danger=0;
                  yylval.val = newToken(EQ, NULL, charsRead - 1, yyleng);
                  return EQ; }

"<>"            { charsRead += yyleng; danger=0;
                  yylval.val = newToken(NEQ, NULL, charsRead - 1, yyleng);
                  return NEQ; }


then		{ checkdanger(); charsRead += yyleng;
		  yylval.val = newToken(THENSYM, NULL, charsRead - 1, yyleng);
		  return THENSYM; }

{DIGIT}+ 	{checkdanger(); charsRead += yyleng;
		 	yylval.val = newToken(INTCONST, copyString(yytext),
                                charsRead - 1, yyleng);
                  	return INTCONST; }		

{DIGIT}+("."{DIGIT}+)?(("E"|"e")("+"|-)?{DIGIT}+)?		{checkdanger();
			charsRead += yyleng;
                        yylval.val = newToken(REALCONST, copyString(yytext),
                                charsRead - 1, yyleng);
                        return REALCONST; }


\'({OTHERS}|"''"|"\\\\"|"\\n"|"\\t"|" ")+\'	{ char *str;int i;int counter;
						counter=0;
						danger=0; 
						charsRead += yyleng;
						str=malloc(yyleng);
						for(i=1;i<yyleng-1;i++){
							if (yytext[i]=='\''){
							str[counter++]='\'';
							i++;
						}else if ((yytext[i]=='\\') 
						&&(yytext[i+1]=='\\')){
						 str[counter++]='\\';
							i++;									}else if ((yytext[i]=='\\') &&(yytext[i+1]=='t')){
						 str[counter++]='\t';
							i++;
						}else if ((yytext[i]=='\\') 
						&&(yytext[i+1]=='n')){
						 str[counter++]='\n';
						i++;
						}else 
						 str[counter++]=yytext[i];
						}
						str[counter]='\0';
						
                       				yylval.val = newToken(STRINGCONST, copyString(str),
                            			charsRead - 1, yyleng);
						free(str);
                        			return STRINGCONST; 
						}


\'({OTHERS}|"\\"|" ")*(\'|\n)			{int i;
						if ((yytext[yyleng-1]=='\n')||
						   (yytext[yyleng-1]==EOF))
						 errorInInputToCompiler("missing single quote at that location ", charsRead+yyleng-1);
						if (yyleng<3) errorInInputToCompiler("String should have at least 1 character", charsRead);
						for(i=1;i<yyleng-1;i++){
							if ((yytext[i]=='\\') && (yytext[i+1]!='\\') && (yytext[i+1]!='t') && (yytext[i+1]!='n')) 
							errorInInputToCompiler("backslash should be followed by a backslash or n or t", charsRead+i);
							else if ((yytext[i]=='\'')  && (yytext[i+1]!='\''))
								errorInInputToCompiler("a single quote should be followed by single quote", charsRead+i);
						}
						}		

\'			{errorInInputToCompiler("you should terminate the string starting here with a single quote ,not EOF",charsRead);}

{LETTER}({LETTER}|{DIGIT})*	{ checkdanger();charsRead += yyleng;
		  yylval.val = newToken(IDENT, copyString(yytext),
				charsRead - 1, yyleng);
		  return IDENT; }

.		{ errorInInputToCompiler("Bad input character", charsRead);}
\n		{ danger=0;charsRead++; }

<<EOF>>		{ charsRead++;
		  yylval.val = newToken(EOFTOKEN, NULL, charsRead - 1, yyleng);
		  return EOFTOKEN; }

%%

static tokenRec *newToken(tokenType ty, char *spell, int endPos, int leng)
{
    tokenRec *pt;
    pt = allocMem(sizeof(tokenRec), "newToken");
    pt->tType = ty;
    pt->spell = spell;
    pt->location.firstChar = endPos - leng + 1;
    pt->location.lastChar = endPos;
}

static void checkdanger(){
	if (danger)
           errorInInputToCompiler("Consecutive identifiers,reserved words and numbers must be seperated by white space or comment", charsRead);
        else
           danger=1;

}

tokenRec *gTok(void)
{
	int junk;

	junk = yylex();
	return yylval.val;
}

void initScan(FILE *inFileParam)
{
	charsRead = 0;
	danger=0;
	cmt=0;
}
