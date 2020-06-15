#ifndef COSMOPOLITAN_DSP_CORE_ITUROUND_H_
#define COSMOPOLITAN_DSP_CORE_ITUROUND_H_
#include "libc/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * An ITU recommended rounding function.
 *
 * 1. Negative numbers round toward zero
 * 2. Positive numbers round toward infinity
 *
 * @see round(), rint()
 */
static inline pureconst artificial long ituround(double x) {
  return floor(x + .5);
}

static inline pureconst artificial int ituroundf(float x) {
  return floorf(x + .5f);
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_ITUROUND_H_ */
