#ifndef COSMOPOLITAN_LIBC_TINYMATH_LOGF_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_LOGF_DATA_INTERNAL_H_

#define LOGF_TABLE_BITS 4
#define LOGF_POLY_ORDER 4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const struct logf_data {
  struct {
    double invc, logc;
  } tab[1 << LOGF_TABLE_BITS];
  double ln2;
  double poly[LOGF_POLY_ORDER - 1]; /* First order coefficient is 1.  */
} __logf_data;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_LOGF_DATA_INTERNAL_H_ */
