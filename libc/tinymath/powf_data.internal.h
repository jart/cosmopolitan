#ifndef COSMOPOLITAN_LIBC_TINYMATH_POWF_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_POWF_DATA_INTERNAL_H_

#define POWF_LOG2_TABLE_BITS 4
#define POWF_LOG2_POLY_ORDER 5
#if TOINT_INTRINSICS
#define POWF_SCALE_BITS EXP2F_TABLE_BITS
#else
#define POWF_SCALE_BITS 0
#endif
#define POWF_SCALE ((double)(1 << POWF_SCALE_BITS))

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern _Hide const struct powf_log2_data {
  struct {
    double invc, logc;
  } tab[1 << POWF_LOG2_TABLE_BITS];
  double poly[POWF_LOG2_POLY_ORDER];
} __powf_log2_data;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_POWF_DATA_INTERNAL_H_ */
