#ifndef COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_
#include "libc/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Returns Euclidean floating-point division remainder.
 *
 * @return (𝑥 mod 𝑦) ∈ [0.,𝑦)
 * @see fmodl()
 */
static inline long double emodl(long double x, long double y) {
  return x - fabsl(y) * floorl(x / fabsl(y));
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_EMODL_H_ */
