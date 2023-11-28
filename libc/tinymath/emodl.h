#ifndef COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_
#include "libc/math.h"

/**
 * Returns Euclidean floating-point division remainder.
 *
 * @return (𝑥 mod 𝑦) ∈ [0.,𝑦)
 * @see fmodl()
 */
#define emodl(x, y)                              \
  ({                                             \
    long double __x = x;                         \
    long double __y = y;                         \
    __x - fabsl(__y) * floorl(__x / fabsl(__y)); \
  })

#endif /* COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_ */
