#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpCvt0f2a(struct Machine *, uint32_t);
void OpCvtt0f2c(struct Machine *, uint32_t);
void OpCvt0f2d(struct Machine *, uint32_t);
void OpCvt0f5a(struct Machine *, uint32_t);
void OpCvt0f5b(struct Machine *, uint32_t);
void OpCvt0fE6(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_ */
