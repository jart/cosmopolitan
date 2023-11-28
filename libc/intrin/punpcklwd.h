#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void punpcklwd(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define punpcklwd(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpcklwd, SSE2, "punpcklwd", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_ */
