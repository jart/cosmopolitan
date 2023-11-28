#ifndef COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_
#include "libc/tinymath/internal.h"
COSMOPOLITAN_C_START_

#define EXTRACT_WORDS(hi, lo, d) \
  do {                           \
    uint64_t __u = asuint64(d);  \
    (hi) = __u >> 32;            \
    (lo) = (uint32_t)__u;        \
  } while (0)

#define GET_HIGH_WORD(hi, d)  \
  do {                        \
    (hi) = asuint64(d) >> 32; \
  } while (0)

#define GET_LOW_WORD(lo, d)       \
  do {                            \
    (lo) = (uint32_t)asuint64(d); \
  } while (0)

#define INSERT_WORDS(d, hi, lo)                              \
  do {                                                       \
    (d) = asdouble(((uint64_t)(hi) << 32) | (uint32_t)(lo)); \
  } while (0)

#define SET_HIGH_WORD(d, hi) INSERT_WORDS(d, hi, (uint32_t)asuint64(d))

#define SET_LOW_WORD(d, lo) INSERT_WORDS(d, asuint64(d) >> 32, lo)

#define GET_FLOAT_WORD(w, d) \
  do {                       \
    (w) = asuint(d);         \
  } while (0)

#define SET_FLOAT_WORD(d, w) \
  do {                       \
    (d) = asfloat(w);        \
  } while (0)

_Complex double __ldexp_cexp(_Complex double, int);
_Complex float __ldexp_cexpf(_Complex float, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_ */
