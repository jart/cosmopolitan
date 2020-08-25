#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_IOPORTS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_IOPORTS_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t OpIn(struct Machine *, uint16_t);
void OpOut(struct Machine *, uint16_t, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_IOPORTS_H_ */
