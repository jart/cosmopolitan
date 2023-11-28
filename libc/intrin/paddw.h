#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void paddw(int16_t[8], const int16_t[8], const int16_t[8]);

#define paddw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddw, SSE2, "paddw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDW_H_ */
