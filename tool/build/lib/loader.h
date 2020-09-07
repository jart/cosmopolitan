#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_LOADER_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_LOADER_H_
#include "libc/elf/struct/ehdr.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Elf {
  const char *prog;
  Elf64_Ehdr *ehdr;
  size_t size;
  int64_t base;
  char *map;
  size_t mapsize;
};

void LoadProgram(struct Machine *, const char *, char **, char **,
                 struct Elf *);
void LoadDebugSymbols(struct Elf *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_LOADER_H_ */
