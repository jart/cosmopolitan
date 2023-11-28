#ifndef COSMOPOLITAN_LIBC_TINYMATH_EXP2F_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_EXP2F_DATA_INTERNAL_H_

#define EXP2F_TABLE_BITS 5
#define EXP2F_POLY_ORDER 3

COSMOPOLITAN_C_START_

extern const struct exp2f_data {
  uint64_t tab[1 << EXP2F_TABLE_BITS];
  double shift_scaled;
  double poly[EXP2F_POLY_ORDER];
  double shift;
  double invln2_scaled;
  double poly_scaled[EXP2F_POLY_ORDER];
} __exp2f_data;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_EXP2F_DATA_INTERNAL_H_ */
