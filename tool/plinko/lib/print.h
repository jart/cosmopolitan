#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_PRINT_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_PRINT_H_
COSMOPOLITAN_C_START_

int Print(int, int);
int PrettyPrint(int, int, int);

bool ShouldForceDot(int);
bool ShouldConcealClosure(int);

int EnterPrint(int);
int PrintArgs(int, int, int, int);
int PrintAtom(int, int);
int PrintChar(int, int);
int PrintDepth(int, int);
int PrintDot(int);
int PrintIndent(int, int);
int PrintInt(int, long, int, char, char, int, bool);
int PrintListDot(int, int);
int PrintSpace(int);
int PrintZeroes(int, int);
void GetName(int *);
void LeavePrint(int);
void PrintHeap(int);
void PrintNewline(int);
void PrintTree(int, int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_PRINT_H_ */
