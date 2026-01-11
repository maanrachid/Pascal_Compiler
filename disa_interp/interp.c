#include <stdio.h>
#include <math.h>
#include "opcodes.h"
#include "machine.h"
#include "iolib.h"


extern either_type st[];
extern int code[];


void interpret(int stringSize, int gVars, int start)
{
  int pc, ireg, top, base;
  int op1, op2, op3, op4;
  int addr1, addr2;
  int tempindex;
  int display[MAXLEVEL];
  status ps;
  int tempch, tempint;
  float tempreal;
  int addr;
  int scanresult;
  int nextcaseindx, caseindxlimit, caseexpr;

  pc = start;
  base = stringSize;
  display[1] = base;
  top = base + 4 + gVars;
  ps = RUN;
  do {
    ireg = code[pc++];
    switch (ireg) {
    case LDADDR :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	op1 = code[pc++];
	op2 = code[pc++];
	st[top].i = display[op1] + op2;
      }
      break;
    case LDVAL :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	op1 = code[pc++];
	op2 = code[pc++];
	st[top] = st[display[op1] + op2];
      }
      break;
    case LDINDIRECT :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	op1 = code[pc++];
	op2 = code[pc++];
	st[top] = st[st[display[op1] + op2].i];
      }
      break;
    case FIXDISPLAY :
      op1 = code[pc++];
      op2 = code[pc++];
      tempint = base;
      for (tempindex = op2; tempindex > op1; tempindex--) {
	display[tempindex] = tempint;
	tempint = st[tempint + 2].i;
      }



		
      break;
    case OFFSET :
      st[top].i += code[pc++];
      break;
    case JMP :
      pc = code[pc];
      break;
    case JMPIFFALSE :
      if (st[top--].i == 0)
	pc = code[pc];
      else
	pc += 1;
      break;
    case SWITCH :
      op1 = code[pc++];
      op2 = code[pc++];
      caseexpr = st[top--].i;
      nextcaseindx = op1;
      caseindxlimit = nextcaseindx + 2*op2;
      while (1) {
	if (nextcaseindx >= caseindxlimit) {
	  ps = CASEERROR;
	  break;
	} else if (caseexpr == code[nextcaseindx]) {
	  pc = code[nextcaseindx + 1];
	  break;
	} else
	  nextcaseindx += 2;
      }
      break;
    case FOR1UP :
      op1 = code[pc++];
      if (st[top - 1].i <= st[top].i)
	st[st[top - 2].i].i = st[top - 1].i;
      else {
	top -= 3;
	pc = op1;
      }
      break;
    case FOR2UP :
      op1 = code[pc++];
      if (st[st[top - 2].i].i < st[top].i) {
	st[st[top - 2].i].i += 1;
	pc = op1;
      } else
	top -= 3;
      break;
    case FOR1DOWN :
      op1 = code[pc++];
      if (st[top - 1].i >= st[top].i)
	st[st[top - 2].i].i = st[top - 1].i;
      else {
	top -= 3;
	pc = op1;
      }
      break;
    case FOR2DOWN :
      op1 = code[pc++];
      if (st[st[top - 2].i].i > st[top].i) {
	st[st[top - 2].i].i -= 1;
	pc = op1;
      } else
	top -= 3;
      break;


    case MARK :
      op1 = code[pc++];
      if (top + 5 + op1 >= STACKMAX)
	ps = STACKOVERFLOW;
      else
	top += 5;
      break;
    case CALL :
      op1 = code[pc++];    /* Starting address */
      op2 = code[pc++];    /* Parameter space */
      op3 = code[pc++];    /* Variable space */
      op4 = code[pc++];    /* Level at which proc name defined */
      tempint = base;
      base = top - op2 - 4;
      top = top + op3;
      display[op4 + 1] = base;
      st[base + 2].i = display[op4];    /* Install static link */
      st[base + 3].i = tempint;         /* Install dynamic link */
      st[base + 1].i = pc;              /* Install return address */
      pc = op1;
      break;
    case INDEX1 :
      op1 = code[pc++];
      op2 = code[pc++];
      if ((st[top].i < op1) || (st[top].i > op2))
	ps = SUBSCRIPTERROR;
      else {
	st[top - 1].i += st[top].i - op1;
	top -= 1;
      }
      break;
    case INDEX :
      op1 = code[pc++];
      op2 = code[pc++];
      op3 = code[pc++];
      if ((st[top].i < op1) || (st[top].i > op2))
	ps = SUBSCRIPTERROR;
      else {
	st[top - 1].i += (st[top].i - op1) * op3;
	top -= 1;
      }
      break;
    case LOADBLOCK :
      op1 = code[pc++];
      if (top + op1 - 1 >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	addr1 = st[top--].i;
	for (tempindex = 0; tempindex < op1; tempindex++)
	  st[++top] = st[addr1++];
      }
      break;
    case COPYBLOCK :
      op1 = code[pc++];
      addr1 = st[top].i;
      addr2 = st[top - 1].i;
      for (tempindex = 0; tempindex < op1; tempindex++)
	st[addr2++] = st[addr1++];
      top -= 2;
      break;
    case LOADIMMED :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	st[top].i = code[pc++];
      }
      break;
    case FLOAT :
      st[top - code[pc]].r = st[top - code[pc]].i;
      pc += 1;
      break;
    case QUIT :
      ps = STOP;
      break;
    case RETPROC :
      top = base - 1;
      pc = st[base + 1].i;
      base = st[base + 3].i;
      break;
    case RETFCN :
      top = base; /* Leave function value on the stack */
      pc = st[base + 1].i;
      base = st[base + 3].i;
      break;
    case GETINDIRECT :
      st[top] = st[st[top].i];
      break;
    case NOTOP :
      st[top].i = ! st[top].i;
      break;
    case NEGINT :
      st[top].i = - st[top].i;
      break;
    case STORE :
      st[st[top - 1].i] = st[top];
      top -= 2;
      break;
    case EQREAL :
      st[top - 1].i = st[top - 1].r == st[top].r;
      top -= 1;
      break;
    case NEQREAL :
      st[top - 1].i = st[top - 1].r != st[top].r;
      top -= 1;
      break;
    case LTREAL :
      st[top - 1].i = st[top - 1].r < st[top].r;
      top -= 1;
      break;
    case LEQREAL :
      st[top - 1].i = st[top - 1].r <= st[top].r;
      top -= 1;
      break;
    case GTREAL :
      st[top - 1].i = st[top - 1].r > st[top].r;
      top -= 1;
      break;
    case GEQREAL :
      st[top - 1].i = st[top - 1].r >= st[top].r;
      top -= 1;
      break;
    case EQINT :
      st[top - 1].i = st[top - 1].i == st[top].i;
      top -= 1;
      break;
    case NEQINT :
      st[top - 1].i = st[top - 1].i != st[top].i;
      top -= 1;
      break;
    case LTINT :

      st[top - 1].i = st[top - 1].i < st[top].i;
      top -= 1;
      break;
    case LEQINT :
      st[top - 1].i = st[top - 1].i <= st[top].i;
      top -= 1;
      break;
    case GTINT :
      st[top - 1].i = st[top - 1].i > st[top].i;
      top -= 1;
      break;
    case GEQINT :
      st[top - 1].i = st[top - 1].i >= st[top].i;
      top -= 1;
      break;
    case OROP :
      st[top - 1].i = st[top - 1].i || st[top].i;
      top -= 1;
      break;
    case ADDINT :
      st[top - 1].i += st[top].i;
      top -= 1;
      break;
    case SUBINT :
      st[top - 1].i -= st[top].i;
      top -= 1;
      break;
    case ADDREAL :
      st[top - 1].r += st[top].r;
      top -= 1;
      break;
    case SUBREAL :
      st[top - 1].r -= st[top].r;
      top -= 1;
      break;
    case ANDOP :
      st[top - 1].i = st[top - 1].i && st[top].i;
      top -= 1;
      break;
    case MULINT :
      st[top - 1].i *= st[top].i;
      top -= 1;
      break;
    case DIVINT :
      if (st[top].i == 0)
	ps = ARITHERROR;
      else {
	st[top - 1].i /= st[top].i;
	top -= 1;
      }
      break;
    case MODINT :
      if (st[top].i == 0)
	ps = ARITHERROR;
      else {
	st[top - 1].i %= st[top].i;
	top -= 1;
      }
      break;
    case MULREAL :
      st[top - 1].r *= st[top].r;
      top -= 1;
      break;
    case DIVREAL :
      if (st[top].r == 0.0)

	ps = ARITHERROR;
      else {
	st[top - 1].r /= st[top].r;
	top -= 1;
      }
      break;
    case READLNOP :
      while (1) {
	tempch = getchar();
	if (tempch == EOF) {
	  ps = IOERROR;
	  break;
	} else if (tempch == '\n')
	  break;
      }
      break;
    case WRITELNOP :
      putchar('\n');
      break;
    case NEGREAL :
      st[top].r = - st[top].r;
      break;
    case READCHAR :
      addr = st[top--].i;
      tempch = getchar();
      if (tempch == EOF)
	ps = IOERROR;
      else if (tempch == '\n')
	st[addr].i = ' ';
      else
	st[addr].i = tempch;
      break;
    case READINT :
      addr = st[top--].i;
      scanresult = scanf("%d", &tempint);
      if (scanresult != 1)
	ps = IOERROR;
      else
	st[addr].i = tempint;
      break;
    case READREAL :
      addr = st[top--].i;
      scanresult = scanf("%f", &tempreal);
      if (scanresult != 1)
	ps = IOERROR;
      else
	st[addr].r = tempreal;
      break;
    case WRITEBOOL :
      ps = write_bool(st[top - 1].i, st[top].i);
      top -= 2;
      break;
    case WRITECHAR :
      ps = write_char(st[top - 1].i, st[top].i);
      top -= 2;
      break;
    case WRITEINT :
      ps = write_int(st[top - 1].i, st[top].i);
      top -= 2;
      break;
    case WRITEREAL2 :
      ps = write_real2(st[top - 1].r, st[top].i);
      top -= 2;
      break;
    case WRITESTRING :
      ps = write_string(st[top - 2].i, st[top - 1].i, st[top].i);
      top -= 3;
      break;
    case WRITEREAL3 :
      ps = write_real3(st[top - 2].r, st[top - 1].i, st[top].i);
      top -= 3;
      break;
    case ABSIFCN :
      if (st[top].i < 0)
	st[top].i = - st[top].i;
      break;
    case ABSRFCN :
      if (st[top].r < 0)
	st[top].r = - st[top].r;
      break;
    case SQRIFCN :
      st[top].i = st[top].i * st[top].i;
      break;
    case SQRRFCN :
      st[top].r = st[top].r * st[top].r;
      break;
    case ODDFCN :
      st[top].i = st[top].i & 1;
      break;
    case CHRFCN :
      if ((st[top].i < 0) || (st[top].i > 127))
	ps = VALUEERROR;
      /* otherwise value OK without conversion */
      break;
    case ORDFCN :
      /* nothing to do */
      break;
    case SUCCFCN :
      if (st[top - 1].i >= st[top].i)
	ps = VALUEERROR;
      else {
	st[top - 1].i += 1;
	top -= 1;
      }
      break;
    case PREDFCN :
      if (st[top - 1].i <= st[top].i)
	ps = VALUEERROR;
      else {
	st[top - 1].i -= 1;
	top -= 1;
      }
      break;
    case ROUNDFCN :
      if (st[top].r >= 0)
	st[top].i = st[top].r + 0.5;
      else
	st[top].i = st[top].r - 0.5;
      break;
    case TRUNCFCN :
      st[top].i = st[top].r;
      break;
    case SINFCN :
      st[top].r = sin(st[top].r);
      break;
    case COSFCN :
      st[top].r = cos(st[top].r);
      break;
    case EXPFCN :
      st[top].r = exp(st[top].r);


      break;
    case LNFCN :
      st[top].r = log(st[top].r);
      break;
    case SQRTFCN :
      st[top].r = sqrt(st[top].r);
      break;
    case ARCTANFCN :
      st[top].r = atan(st[top].r);
      break;
    case EOFFCN :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	tempch = getchar();
	if (tempch == EOF)
	  st[top].i = 1;
	else {
	  st[top].i = 0;
	  ungetc(tempch, stdin);
	}
      }
      break;
    case EOLNFCN :
      if (++top >= STACKMAX)
	ps = STACKOVERFLOW;
      else {
	tempch = getchar();
	if (tempch == EOF)
	  ps = IOERROR;
	else if (tempch == '\n') {
	  st[top].i = 1;
	  ungetc(tempch, stdin);
	} else {
	  st[top].i = 0;
	  ungetc(tempch, stdin);
	}
      }
      break;
    }
  } while (ps == RUN);
  if (ps != STOP)
    printf("Error code %d\n", ps);
}
