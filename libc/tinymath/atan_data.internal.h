#ifndef COSMOPOLITAN_LIBC_TINYMATH_ATAN_DATA_H_
#define COSMOPOLITAN_LIBC_TINYMATH_ATAN_DATA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define ATAN_POLY_NCOEFFS 20
extern const struct atan_poly_data {
  double poly[ATAN_POLY_NCOEFFS];
} __atan_poly_data;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_ATAN_DATA_H_ */
