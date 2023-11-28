#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKHWD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKHWD_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void punpckhwd(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define punpckhwd(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpckhwd, SSE2, "punpckhwd", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKHWD_H_ */
