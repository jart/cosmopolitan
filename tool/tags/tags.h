#ifndef COSMOPOLITAN_TOOL_TAGS_TAGS_H_
#define COSMOPOLITAN_TOOL_TAGS_TAGS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Token {
  const char *s;
  size_t n;
};

struct Exprs {
  struct Exprs *n;
  long double x;
};

static void SyntaxError(void) noreturn;
static long double ParseExpr(struct Token);
static void ExprsFree(struct Exprs *);
static struct Exprs *ExprsAppend(struct Exprs *, long double);
static long double CallFunction(struct Token, struct Exprs *);
int GetKeyword(const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_TAGS_TAGS_H_ */
