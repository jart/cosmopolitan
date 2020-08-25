#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpUd(struct Machine *) noreturn;
void HaltMachine(struct Machine *, int) noreturn;
void ThrowDivideError(struct Machine *) noreturn;
void ThrowSegmentationFault(struct Machine *, int64_t) noreturn;
void ThrowProtectionFault(struct Machine *) noreturn;
void OpHlt(struct Machine *) noreturn;
void OpInterrupt(struct Machine *, int) noreturn;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_ */
