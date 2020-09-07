#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#include "tool/build/lib/machine.h"

#define kFpuTagValid   0b00
#define kFpuTagZero    0b01
#define kFpuTagSpecial 0b10
#define kFpuTagEmpty   0b11

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpFpu(struct Machine *, uint32_t);
void OpFinit(struct Machine *);
void OpFwait(struct Machine *, uint32_t);
void FpuPush(struct Machine *, long double);
long double FpuPop(struct Machine *);
long double *FpuSt(struct Machine *, unsigned);
int FpuGetTag(struct Machine *, unsigned);
void FpuSetTag(struct Machine *, unsigned, unsigned);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_ */
