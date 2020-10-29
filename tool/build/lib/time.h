#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_TIME_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_TIME_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpPause(struct Machine *, uint32_t);
void OpRdtsc(struct Machine *, uint32_t);
void OpRdtscp(struct Machine *, uint32_t);
void OpRdpid(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_TIME_H_ */
