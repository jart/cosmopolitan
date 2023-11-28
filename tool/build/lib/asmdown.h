#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ASMDOWN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ASMDOWN_H_
#include "tool/build/lib/javadown.h"
COSMOPOLITAN_C_START_

struct Asmdown {
  struct AsmdownSymbols {
    size_t n;
    struct AsmdownSymbol {
      int line;
      char *name;
      bool is_alias;
      struct Javadown *javadown;
    } * p;
  } symbols;
};

struct Asmdown *ParseAsmdown(const char *, size_t);
void FreeAsmdown(struct Asmdown *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ASMDOWN_H_ */
