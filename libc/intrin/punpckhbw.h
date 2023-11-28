#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKHBW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKHBW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void punpckhbw(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define punpckhbw(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpckhbw, SSE2, "punpckhbw", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKHBW_H_ */
