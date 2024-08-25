#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void psraw(int16_t[8], const int16_t[8], unsigned char) libcesque;

#define psraw(A, B, I) INTRIN_SSEVEX_X_I_(psraw, SSE2, "psraw", A, B, I)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_ */
