#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_BCD_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_BCD_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpDas(struct Machine *, uint32_t);
void OpAaa(struct Machine *, uint32_t);
void OpAas(struct Machine *, uint32_t);
void OpAam(struct Machine *, uint32_t);
void OpAad(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_BCD_H_ */
