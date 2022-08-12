#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_
#include "tool/build/lib/machine.h"

#define kFpuTagValid   0b00
#define kFpuTagZero    0b01
#define kFpuTagSpecial 0b10
#define kFpuTagEmpty   0b11

#define kFpuCwIm 0x0001 /* invalid operation mask */
#define kFpuCwDm 0x0002 /* denormal operand mask */
#define kFpuCwZm 0x0004 /* zero divide mask */
#define kFpuCwOm 0x0008 /* overflow mask */
#define kFpuCwUm 0x0010 /* underflow mask */
#define kFpuCwPm 0x0020 /* precision mask */
#define kFpuCwPc 0x0300 /* precision: 32,∅,64,80 */
#define kFpuCwRc 0x0c00 /* rounding: even,→-∞,→+∞,→0 */

#define kFpuSwIe 0x0001 /* invalid operation */
#define kFpuSwDe 0x0002 /* denormalized operand */
#define kFpuSwZe 0x0004 /* zero divide */
#define kFpuSwOe 0x0008 /* overflow */
#define kFpuSwUe 0x0010 /* underflow */
#define kFpuSwPe 0x0020 /* precision */
#define kFpuSwSf 0x0040 /* stack fault */
#define kFpuSwEs 0x0080 /* exception summary status */
#define kFpuSwC0 0x0100 /* condition 0 */
#define kFpuSwC1 0x0200 /* condition 1 */
#define kFpuSwC2 0x0400 /* condition 2 */
#define kFpuSwSp 0x3800 /* top stack */
#define kFpuSwC3 0x4000 /* condition 3 */
#define kFpuSwBf 0x8000 /* busy flag */

#define kMxcsrIe  0x0001 /* invalid operation flag */
#define kMxcsrDe  0x0002 /* denormal flag */
#define kMxcsrZe  0x0004 /* divide by zero flag */
#define kMxcsrOe  0x0008 /* overflow flag */
#define kMxcsrUe  0x0010 /* underflow flag */
#define kMxcsrPe  0x0020 /* precision flag */
#define kMxcsrDaz 0x0040 /* denormals are zeros */
#define kMxcsrIm  0x0080 /* invalid operation mask */
#define kMxcsrDm  0x0100 /* denormal mask */
#define kMxcsrZm  0x0200 /* divide by zero mask */
#define kMxcsrOm  0x0400 /* overflow mask */
#define kMxcsrUm  0x0800 /* underflow mask */
#define kMxcsrPm  0x1000 /* precision mask */
#define kMxcsrRc  0x6000 /* rounding control */
#define kMxcsrFtz 0x8000 /* flush to zero */

#define FpuSt(m, i) ((m)->fpu.st + (((i) + ((m->fpu.sw & kFpuSwSp) >> 11)) & 7))

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

double FpuPop(struct Machine *);
int FpuGetTag(struct Machine *, unsigned);
void FpuPush(struct Machine *, double);
void FpuSetTag(struct Machine *, unsigned, unsigned);
void OpFinit(struct Machine *);
void OpFpu(struct Machine *, uint32_t);
void OpFwait(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FPU_H_ */
