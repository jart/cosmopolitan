#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_
#include "third_party/xed/x86.h"
#include "tool/build/lib/loader.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define DIS_MAX_SYMBOL_LENGTH 32

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
      int unique;
      int size;
      int name;
      char rank;
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
  struct Machine *m; /* for the segment registers */
  uint64_t addr;     /* current effective address */
  char buf[512];
};

extern bool g_disisprog_disable;

long Dis(struct Dis *, struct Machine *, uint64_t, uint64_t, int);
long DisFind(struct Dis *, int64_t);
void DisFree(struct Dis *);
void DisFreeOp(struct DisOp *);
void DisFreeOps(struct DisOps *);
void DisLoadElf(struct Dis *, struct Elf *);
long DisFindSym(struct Dis *, int64_t);
long DisFindSymByName(struct Dis *, const char *);
bool DisIsText(struct Dis *, int64_t);
bool DisIsProg(struct Dis *, int64_t);
char *DisInst(struct Dis *, char *, const char *);
char *DisArg(struct Dis *, char *, const char *);
const char *DisSpec(struct XedDecodedInst *, char *);
const char *DisGetLine(struct Dis *, struct Machine *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_DIS_H_ */
