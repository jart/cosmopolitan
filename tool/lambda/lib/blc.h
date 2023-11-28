#ifndef COSMOPOLITAN_TOOL_LAMBDA_LIB_BLC_H_
#define COSMOPOLITAN_TOOL_LAMBDA_LIB_BLC_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

#define BUILTINS 4
#define LOC      12
#define TERMS    5000000

#define IOP 0  // code for gro, wr0, wr1, put
#define VAR 1  // code for variable lookup
#define APP 2  // code for applications
#define ABS 3  // code for abstractions

struct Parse {
  int n;
  int i;
};

struct Closure {
  struct Closure *next;
  struct Closure *envp;
  int refs;
  int term;
};

extern char vlog;
extern char slog;
extern char alog;
extern char rlog;
extern char safer;
extern char style;
extern char binary;
extern char noname;
extern char asciiname;
extern int ci;
extern int co;
extern long ip;
extern long end;
extern int heap;
extern FILE *logh;
extern int mem[TERMS];
extern struct Closure root;
extern struct Closure *envp;
extern struct Closure *frep;
extern struct Closure *contp;

char GetBit(FILE *);
char NeedBit(FILE *);
struct Parse Parse(int, FILE *);
void Dump(int, int, FILE *);
void Error(int, const char *, ...);
void PrintLambda(int, int, int, int, FILE *);
void PrintBinary(int, int, int, FILE *);
void PrintDebruijn(int, int, int, FILE *);
void PrintMachineState(FILE *);
void PrintExpressions(FILE *, char, char);
void Print(int, int, int, FILE *);
void PrintVar(int, FILE *);
void *Calloc(size_t, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_LAMBDA_LIB_BLC_H_ */
