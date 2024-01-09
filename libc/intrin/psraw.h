#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void psraw(int16_t[8], const int16_t[8], unsigned char) libcesque;
void psrawv(int16_t[8], const int16_t[8], const uint64_t[2]) libcesque;

#define psraw(A, B, I) INTRIN_SSEVEX_X_I_(psraw, SSE2, "psraw", A, B, I)
#define psrawv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psrawv, SSE2, "psraw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_ */
