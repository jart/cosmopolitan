#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#include "tool/build/lib/machine.h"

#define kFpuTagValid   0b00
#define kFpuTagZero    0b01
#define kFpuTagSpecial 0b10
#define kFpuTagEmpty   0b11

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpFpu(struct Machine *);
void OpFinit(struct Machine *);
void OpFwait(struct Machine *);
void FpuPush(struct Machine *, long double);
long double FpuPop(struct Machine *);

forceinline long double *FpuSt(struct Machine *m, unsigned i) {
  i += m->fpu.sp;
  i &= 0b111;
  return m->fpu.st + i;
}

forceinline int FpuGetTag(struct Machine *m, unsigned i) {
  unsigned t;
  t = m->fpu.tw;
  i += m->fpu.sp;
  i &= 0b111;
  i *= 2;
  t &= 0b11 << i;
  t >>= i;
  return t;
}

forceinline void FpuSetTag(struct Machine *m, unsigned i, unsigned t) {
  i += m->fpu.sp;
  t &= 0b11;
  i &= 0b111;
  i *= 2;
  m->fpu.tw &= ~(0b11 << i);
  m->fpu.tw |= t << i;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_ */
