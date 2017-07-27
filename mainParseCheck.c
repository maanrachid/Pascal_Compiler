#include <stdio.h>
#include "scan.h"
#include "parse.h"

main()
{
  initParser();
  parseProgram();

  fprintf(stderr, "-1 \"No errors found\"\n");
}
