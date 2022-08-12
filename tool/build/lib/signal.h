#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_SIGNAL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_SIGNAL_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpRestore(struct Machine *);
void TerminateSignal(struct Machine *, int);
int DeliverSignal(struct Machine *, int, int);
int ConsumeSignal(struct Machine *);
void EnqueueSignal(struct Machine *, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_SIGNAL_H_ */
