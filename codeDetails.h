#ifndef _codeDetails_h
#define _codeDetails_h

struct codeRec *makeNewLabel(void);

void emitLabel(struct codeRec *theLabel);
void emitMarkOrCall(int opcode, procRec *theProc);
void emitJump(int opcode, struct codeRec *target);
void emitInstr0(int opcode);
void emitInstr1(int opcode, int operand1);
void emitInstr2(int opcode, int operand1, int operand2);
void emitInstr3(int opcode, int operand1, int operand2, int operand3);

void initCodeList(void);
struct codeRec *getCodeList(void);

void calculateCodeAddrs(procListRec *procs);
void sendCodeToFile(procListRec *procs, FILE *outFile);

void initStringMem(void);
int getNextStringAddr(void);
void insertStringChar(int ch);

#endif
