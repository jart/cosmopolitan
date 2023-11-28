#ifndef COSMOPOLITAN_LIBC_TINYMATH_POW_DATA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_POW_DATA_INTERNAL_H_

#define POW_LOG_TABLE_BITS 7
#define POW_LOG_POLY_ORDER 8

COSMOPOLITAN_C_START_

extern const struct pow_log_data {
  double ln2hi;
  double ln2lo;
  double poly[POW_LOG_POLY_ORDER - 1]; /* First coefficient is 1.  */
  /* Note: the pad field is unused, but allows slightly faster indexing.  */
  struct {
    double invc, pad, logc, logctail;
  } tab[1 << POW_LOG_TABLE_BITS];
} __pow_log_data;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_POW_DATA_INTERNAL_H_ */
