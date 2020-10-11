#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ARGV_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ARGV_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void LoadArgv(struct Machine *, const char *, char **, char **);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ARGV_H_ */
