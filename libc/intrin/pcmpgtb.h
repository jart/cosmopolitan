#ifndef COSMOPOLITAN_LIBC_INTRIN_PCMPGTB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PCMPGTB_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void pcmpgtb(int8_t[16], const int8_t[16], const int8_t[16]);

#define pcmpgtb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pcmpgtb, SSE2, "pcmpgtb", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PCMPGTB_H_ */
