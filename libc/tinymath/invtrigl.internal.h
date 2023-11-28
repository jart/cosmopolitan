#ifndef COSMOPOLITAN_LIBC_TINYMATH_INVTRIGL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_INVTRIGL_H_
COSMOPOLITAN_C_START_

/* shared by acosl, asinl and atan2l */
#define pio2_hi __pio2_hi
#define pio2_lo __pio2_lo

extern const long double pio2_hi, pio2_lo;

long double __invtrigl_R(long double z);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_INVTRIGL_H_ */
