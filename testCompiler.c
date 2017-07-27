#include <stdio.h>
#include <stdlib.h>
#include "scan.h"
#include "syntaxTree.h"
#include "parse.h"
#include "check.h"
#include "codegen.h"

void announceNoErrors(void);

main()
{
    programRec *p;
    procListRec *c;

    p = parseProgram();
    c = checkProgram(p);

    announceNoErrors();

    generateCode(c, stdout);

    exit(0);  /*  Included as place to set breakpoint for viewing trees.  */
}

void announceNoErrors(void)
{
    fprintf(stderr, "-1 \"No errors detected\"\n");
}
