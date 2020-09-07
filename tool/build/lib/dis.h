#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_
#include "third_party/xed/x86.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Dis {
  struct DisOps {
    size_t i, n;
    struct DisOp {
      int64_t addr;
      uint8_t size;
      bool active;
      char *s;
    } * p;
  } ops;
  struct DisLoads {
    size_t i, n;
    struct DisLoad {
      int64_t addr;
      uint64_t size;
      bool istext;
    } * p;
  } loads;
  struct DisSyms {
    size_t i, n;
    struct DisSym {
      int64_t addr;
      int rank;
      int unique;
      int size;
      int name;
      bool iscode;
      bool isabs;
    } * p;
    const char *stab;
  } syms;
  struct DisEdges {
    size_t i, n;
    struct DisEdge {
      int64_t src;
      int64_t dst;
    } * p;
  } edges;
  struct XedDecodedInst xedd[1];
  char buf[512];
};

struct DisBuilder {
  struct Dis *dis;
  struct XedDecodedInst *xedd;
  int64_t addr;
};

struct DisHigh {
  uint8_t keyword;
  uint8_t reg;
  uint8_t literal;
  uint8_t label;
  uint8_t comment;
};

extern struct DisHigh *g_dis_high;

long Dis(struct Dis *, struct Machine *, int64_t, int);
long DisFind(struct Dis *, int64_t);
void DisFree(struct Dis *);
void DisFreeOp(struct DisOp *);
void DisFreeOps(struct DisOps *);
void DisLoadElf(struct Dis *, struct Elf *);
long DisFindSym(struct Dis *, int64_t);
long DisFindSymByName(struct Dis *, const char *);
bool DisIsText(struct Dis *, int64_t);
bool DisIsProg(struct Dis *, int64_t);
const char *DisSpec(struct XedDecodedInst *, char *);
char *DisInst(struct DisBuilder, char *, const char *);
char *DisArg(struct DisBuilder, char *, const char *);
char *DisHigh(char *, int);
const char *DisGetLine(struct Dis *, struct Machine *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_ */
