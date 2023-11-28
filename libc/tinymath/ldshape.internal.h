#ifndef COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_
#include "libc/math.h"
COSMOPOLITAN_C_START_

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
union ldshape {
  long double f;
  struct {
    uint64_t m;
    uint16_t se;
  } i;
};
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384 && \
    __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
/* This is the m68k variant of 80-bit long double, and this definition only
 * works on archs where the alignment requirement of uint64_t is <= 4. */
union ldshape {
  long double f;
  struct {
    uint16_t se;
    uint16_t pad;
    uint64_t m;
  } i;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && \
    __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
union ldshape {
  long double f;
  struct {
    uint64_t lo;
    uint32_t mid;
    uint16_t top;
    uint16_t se;
  } i;
  struct {
    uint64_t lo;
    uint64_t hi;
  } i2;
};
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384 && \
    __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
union ldshape {
  long double f;
  struct {
    uint16_t se;
    uint16_t top;
    uint32_t mid;
    uint64_t lo;
  } i;
  struct {
    uint64_t hi;
    uint64_t lo;
  } i2;
};
#else
#error Unsupported long double representation
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_ */
