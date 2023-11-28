#ifndef COSMOPOLITAN_LIBC_TINYMATH_LOG2_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_LOG2_DATA_INTERNAL_H_

#define LOG2_TABLE_BITS  6
#define LOG2_POLY_ORDER  7
#define LOG2_POLY1_ORDER 11

COSMOPOLITAN_C_START_

extern const struct log2_data {
  double invln2hi;
  double invln2lo;
  double poly[LOG2_POLY_ORDER - 1];
  double poly1[LOG2_POLY1_ORDER - 1];
  struct {
    double invc, logc;
  } tab[1 << LOG2_TABLE_BITS];
#if !__FP_FAST_FMA
  struct {
    double chi, clo;
  } tab2[1 << LOG2_TABLE_BITS];
#endif
} __log2_data;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_LOG2_DATA_INTERNAL_H_ */
