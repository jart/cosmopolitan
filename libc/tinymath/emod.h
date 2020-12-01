#ifndef COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_
#include "libc/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Returns Euclidean floating-point division remainder.
 *
 * @return (𝑥 mod 𝑦) ∈ [0.,𝑦)
 * @see fmod()
 */
static inline double emod(double x, double y) {
  return x - fabs(y) * floor(x / fabs(y));
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_ */
