#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRAD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRAD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psrad(int32_t[4], const int32_t[4], unsigned char);
void psradv(int32_t[4], const int32_t[4], const uint64_t[2]);

#define psrad(A, B, I) INTRIN_SSEVEX_X_I_(psrad, SSE2, "psrad", A, B, I)
#define psradv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psradv, SSE2, "psrad", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRAD_H_ */
