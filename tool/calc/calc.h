#ifndef COSMOPOLITAN_TOOL_CALC_CALC_H_
#define COSMOPOLITAN_TOOL_CALC_CALC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Token {
  const char *s;
  size_t n;
};

struct Numbers {
  struct Numbers *n;
  long double x;
};

static void SyntaxError(void) wontreturn;
static long double ParseNumber(struct Token);
static void NumbersFree(struct Numbers *);
static struct Numbers *NumbersAppend(struct Numbers *, long double);
static long double CallFunction(struct Token, struct Numbers *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_CALC_CALC_H_ */
