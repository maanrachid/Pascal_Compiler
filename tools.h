#ifndef _tools_h
#define _tools_h

void programError(char *msg, int location);


void errorInInputToCompiler(char *msg, int location);
  /*  Call errorInInputToCompiler to report an error in the program
   *  being compiled.
   */

void internalError(char *msg);
  /*  Call internalError when an apparently impossible state is reached,
   *  presumably because of an error in the compiler itself.
   */

void miscError(char *msg1, char *msg2);
  /*  Call miscError in other cases where compilation must quit (for
   *  example, when no more memory can be allocated).
   */

void *allocMem(int size, char *errLocation);
  /*  Make all calls to malloc through allocMem, in order to guarantee
   *  that heap overflow will always be detected.
   */

char *copyString(char *s);
  /*  Use copyString to produce dynamically allocated copies of
   *  strings, in order to avoid errors in amount of memory allocated.
   */

#endif
