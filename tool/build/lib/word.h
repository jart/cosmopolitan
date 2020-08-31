#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_WORD_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_WORD_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void SetMemoryShort(struct Machine *, int64_t, int16_t);
void SetMemoryInt(struct Machine *, int64_t, int32_t);
void SetMemoryLong(struct Machine *, int64_t, int64_t);
void SetMemoryFloat(struct Machine *, int64_t, float);
void SetMemoryDouble(struct Machine *, int64_t, double);
void SetMemoryLdbl(struct Machine *, int64_t, long double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_WORD_H_ */
