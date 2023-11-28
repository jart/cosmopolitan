#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKLBW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKLBW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void punpcklbw(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define punpcklbw(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpcklbw, SSE2, "punpcklbw", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKLBW_H_ */
