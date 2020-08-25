#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_CPUID_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_CPUID_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#include "tool/build/lib/cvt.h"

void OpCpuid(struct Machine *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_CPUID_H_ */
