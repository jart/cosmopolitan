#ifndef COSMOPOLITAN_DSP_CORE_ITUROUND_H_
#define COSMOPOLITAN_DSP_CORE_ITUROUND_H_
#include "libc/math.h"

/**
 * An ITU recommended rounding function.
 *
 * 1. Negative numbers round toward zero
 * 2. Positive numbers round toward infinity
 *
 * @see round(), rint()
 */
__funline long ituround(double x) {
  return floor(x + .5);
}

__funline int ituroundf(float x) {
  return floorf(x + .5f);
}

#endif /* COSMOPOLITAN_DSP_CORE_ITUROUND_H_ */
