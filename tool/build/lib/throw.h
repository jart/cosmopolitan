#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpUd(struct Machine *, uint32_t) wontreturn;
void HaltMachine(struct Machine *, int) wontreturn;
void ThrowDivideError(struct Machine *) wontreturn;
void ThrowSegmentationFault(struct Machine *, int64_t) wontreturn;
void ThrowProtectionFault(struct Machine *) wontreturn;
void OpHlt(struct Machine *, uint32_t) wontreturn;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_THROW_H_ */
