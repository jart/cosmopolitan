#ifndef COSMOPOLITAN_LIBC_INTRIN_PACKSSWB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PACKSSWB_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void packsswb(int8_t[16], const int16_t[8], const int16_t[8]);

#define packsswb(A, B, C)                                                    \
  INTRIN_SSEVEX_X_X_X_(packsswb, SSE2, "packsswb", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PACKSSWB_H_ */
