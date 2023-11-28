#ifndef COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_
#include "libc/math.h"

/**
 * Returns Euclidean floating-point division remainder.
 *
 * @return (𝑥 mod 𝑦) ∈ [0.,𝑦)
 * @see fmod()
 */
#define emod(x, y)                            \
  ({                                          \
    double __x = x;                           \
    double __y = y;                           \
    __x - fabs(__y) * floor(__x / fabs(__y)); \
  })

#endif /* COSMOPOLITAN_LIBC_TINYMATH_EMOD_H_ */
