#include <stdlib.h>
#include <string.h>
typedef enum {RUN, STOP, ARITHERROR, IOERROR, VALUEERROR, CASEERROR,
              SUBSCRIPTERROR, STACKOVERFLOW} status;

typedef union {
    int i;
    float r;
} either_type;

#define CODEMEM 1000
#define STACKMAX 2000
#define MAXLEVEL 10

void interpret(int stringSize, int gVars, int start);
void disassemble(int codeSize, int stringSize, int gVars, int start);
