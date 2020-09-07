#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_BITSCAN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_BITSCAN_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef uint64_t (*bitscan_f)(struct Machine *, uint32_t, uint64_t);

uint64_t AluBsr(struct Machine *, uint32_t, uint64_t);
uint64_t AluBsf(struct Machine *, uint32_t, uint64_t);
uint64_t AluPopcnt(struct Machine *, uint32_t, uint64_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_BITSCAN_H_ */
