#ifndef COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void pmulhrsw(int16_t a[8], const int16_t b[8], const int16_t c[8]);

#define pmulhrsw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmulhrsw, SSSE3, "pmulhrsw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_ */
