#ifndef COSMOPOLITAN_LIBC_TINYMATH_LOG2F_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_LOG2F_DATA_INTERNAL_H_

#define LOG2F_TABLE_BITS 4
#define LOG2F_POLY_ORDER 4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern _Hide const struct log2f_data {
  struct {
    double invc, logc;
  } tab[1 << LOG2F_TABLE_BITS];
  double poly[LOG2F_POLY_ORDER];
} __log2f_data;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_LOG2F_DATA_INTERNAL_H_ */
