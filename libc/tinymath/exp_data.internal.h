#ifndef COSMOPOLITAN_LIBC_TINYMATH_EXP_DATA_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EXP_DATA_H_

#define EXP_TABLE_BITS       7
#define EXP_POLY_ORDER       5
#define EXP_USE_TOINT_NARROW 0
#define EXP2_POLY_ORDER      5

COSMOPOLITAN_C_START_

extern const struct exp_data {
  double invln2N;
  double shift;
  double negln2hiN;
  double negln2loN;
  double poly[4]; /* Last four coefficients.  */
  double exp2_shift;
  double exp2_poly[EXP2_POLY_ORDER];
  uint64_t tab[2 * (1 << EXP_TABLE_BITS)];
} __exp_data;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_EXP_DATA_H_ */
