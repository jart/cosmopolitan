#ifndef COSMOPOLITAN_LIBC_TINYMATH_HORNER_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_HORNER_INTERNAL_H_
#include "libc/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if WANT_VMATH
#define FMA(x, y, z) vfmaq_f64(z, x, y)
#else
#define FMA fma
#endif

#include "libc/tinymath/horner_wrap.internal.h"

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_HORNER_INTERNAL_H_ */
