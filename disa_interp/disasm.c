#include <stdio.h>
#include "opcodes.h"
#include "machine.h"

extern int code[];
extern either_type st[];

void disassemble(int codeSize, int stringSize, int gVars, int start)
{
  int pc, ireg;
  int op1, op2, op3, op4;
  int i;
  int lineStart;
  int casetableflags[CODEMEM];

  for (i = 0; i < CODEMEM; i++)
    casetableflags[i] = 0;

  printf("Starting address: %d\n", start);
  printf("Global variable space: %d\n", gVars);

  printf("Strings (%d words):\n", stringSize);
  lineStart = 1;
  for (i = 0; i < stringSize; i++) {
    if (lineStart) {
      printf("%4d:  ", i);
      lineStart = 0;
    }
    if (st[i].i == 0) {
      putchar('\n');
      lineStart = 1;
    } else {
      putchar(st[i].i);
    }
  }
  printf("End of strings\n");
  printf("Code (%d words):\n", codeSize);
  pc = 0;
  while (1) {
    if (pc == codeSize)
      break;
    else if (pc > codeSize) {
      fprintf(stderr, "Error in code file or disassembler\n");
      exit(1);
    }

    printf("%4d:  ", pc);

    if (casetableflags[pc]) {
      op1 = code[pc++];
      op2 = code[pc++];
      printf("Case table item %12d%6d\n", op1, op2);
      continue;
    }

    ireg = code[pc++];
    switch (ireg) {
    case LDADDR :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("LDADDR %d %d\n", op1, op2);
      break;

    case LDVAL :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("LDVAL %d %d\n", op1, op2);
      break;

    case LDINDIRECT :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("LDINDIRECT %d %d\n", op1, op2);
      break;
    case FIXDISPLAY :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("FIXDISPLAY %d %d\n", op1, op2);
      break;
    case OFFSET :
      op1 = code[pc++];
      printf("OFFSET %d\n", op1);
      break;
    case JMP :
      op1 = code[pc++];
      printf("JMP %d\n", op1);
      break;
    case JMPIFFALSE :
      op1 = code[pc++];
      printf("JMPIFFALSE %d\n", op1);
      break;
    case SWITCH :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("SWITCH %d %d\n", op1, op2);
      for (i = op1; i < op1 + 2 * op2; i++)
	casetableflags[i] = 1;
      break;
    case FOR1UP :
      op1 = code[pc++];
      printf("FOR1UP %d\n", op1);
      break;
    case FOR2UP :
      op1 = code[pc++];
      printf("FOR2UP %d\n", op1);
      break;
    case FOR1DOWN :
      op1 = code[pc++];
      printf("FOR1DOWN %d\n", op1);
      break;
    case FOR2DOWN :
      op1 = code[pc++];
      printf("FOR2DOWN %d\n", op1);
      break;
    case MARK :
      op1 = code[pc++];
      printf("MARK %d\n", op1);
      break;
    case CALL :
      op1 = code[pc++];
      op2 = code[pc++];
      op3 = code[pc++];
      op4 = code[pc++];
      printf("CALL %d %d %d %d\n", op1, op2, op3, op4);
      break;
    case INDEX1 :
      op1 = code[pc++];
      op2 = code[pc++];
      printf("INDEX1 %d %d\n", op1, op2);
      break;
    case INDEX :
      op1 = code[pc++];
      op2 = code[pc++];
      op3 = code[pc++];
      printf("INDEX %d %d %d\n", op1, op2, op3);
      break;

    case LOADBLOCK :
      op1 = code[pc++];
      printf("LOADBLOCK %d\n", op1);
      break;
    case COPYBLOCK :
      op1 = code[pc++];
      printf("COPYBLOCK %d\n", op1);
      break;
    case LOADIMMED :
      op1 = code[pc++];
      printf("LOADIMMED %d\n", op1);
      break;
    case FLOAT :
      op1 = code[pc++];
      printf("FLOAT %d\n", op1);
      break;
    case QUIT :
      printf("QUIT\n");
      break;
    case RETPROC :
      printf("RETPROC\n");
      break;
    case RETFCN :
      printf("RETFCN\n");
      break;
    case GETINDIRECT :
      printf("GETINDIRECT\n");
      break;
    case NOTOP :
      printf("NOTOP\n");
      break;
    case NEGINT :
      printf("NEGINT\n");
      break;
    case STORE :
      printf("STORE\n");
      break;
    case EQREAL :
      printf("EQREAL\n");
      break;

    case NEQREAL :
      printf("NEQREAL\n");
      break;
    case LTREAL :
      printf("LTREAL\n");
      break;
    case LEQREAL :
      printf("LEQREAL\n");
      break;
    case GTREAL :
      printf("GTREAL\n");
      break;
    case GEQREAL :
      printf("GEQREAL\n");
      break;
    case EQINT :
      printf("EQINT\n");
      break;
    case NEQINT :
      printf("NEQINT\n");
      break;
    case LTINT :
      printf("LTINT\n");
      break;
    case LEQINT :
      printf("LEQINT\n");
      break;
    case GTINT :
      printf("GTINT\n");
      break;
    case GEQINT :
      printf("GEQINT\n");
      break;
    case OROP :
      printf("OROP\n");
      break;
    case ADDINT :
      printf("ADDINT\n");
      break;
    case SUBINT :
      printf("SUBINT\n");
      break;
    case ADDREAL :
      printf("ADDREAL\n");
      break;
    case SUBREAL :
      printf("SUBREAL\n");
      break;
    case ANDOP :
      printf("ANDOP\n");
      break;
    case MULINT :
      printf("MULINT\n");
      break;
    case DIVINT :
      printf("DIVINT\n");
      break;
    case MODINT :
      printf("MODINT\n");
      break;
    case MULREAL :
      printf("MULREAL\n");
      break;
    case DIVREAL :
      printf("DIVREAL\n");
      break;
    case READLNOP :
      printf("READLNOP\n");
      break;
    case WRITELNOP :
      printf("WRITELNOP\n");
      break;
    case NEGREAL :
      printf("NEGREAL\n");
      break;


    case READCHAR :
      printf("READCHAR\n");
      break;
    case READINT :
      printf("READINT\n");
      break;
    case READREAL :
      printf("READREAL\n");
      break;
    case WRITEBOOL :
      printf("WRITEBOOL\n");
      break;
    case WRITECHAR :
      printf("WRITECHAR\n");
      break;
    case WRITEINT :
      printf("WRITEINT\n");
      break;
    case WRITEREAL2 :
      printf("WRITEREAL2\n");
      break;
    case WRITESTRING :
      printf("WRITESTRING\n");
      break;
    case WRITEREAL3 :
      printf("WRITEREAL3\n");
      break;
    case ABSIFCN :
      printf("ABSIFCN\n");
      break;
    case ABSRFCN :
      printf("ABSRFCN\n");
      break;
    case SQRIFCN :
      printf("SQRIFCN\n");
      break;
    case SQRRFCN :
      printf("SQRRFCN\n");
      break;
    case ODDFCN :
      printf("ODDFCN\n");
      break;

    case CHRFCN :
      printf("CHRFCN\n");
      break;
    case ORDFCN :
      printf("ORDFCN\n");
      break;
    case SUCCFCN :
      printf("SUCCFCN\n");
      break;
    case PREDFCN :
      printf("PREDFCN\n");
      break;
    case ROUNDFCN :
      printf("ROUNDFCN\n");
      break;
    case TRUNCFCN :
      printf("TRUNCFCN\n");
      break;
    case SINFCN :
      printf("SINFCN\n");
      break;
    case COSFCN :
      printf("COSFCN\n");
      break;
    case EXPFCN :
      printf("EXPFCN\n");
      break;
    case LNFCN :
      printf("LNFCN\n");
      break;
    case SQRTFCN :
      printf("SQRTFCN\n");
      break;
    case ARCTANFCN :
      printf("ARCTANFCN\n");
      break;
    case EOFFCN :
      printf("EOFFCN\n");
      break;
    case EOLNFCN :
      printf("EOLNFCN\n");
      break;
    }
  }
}
