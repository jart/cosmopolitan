#ifndef COSMOPOLITAN_LIBC_INTRIN_PSLLW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSLLW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psllw(uint16_t[8], const uint16_t[8], unsigned char);
void psllwv(uint16_t[8], const uint16_t[8], const uint64_t[2]);

#define psllw(A, B, I) INTRIN_SSEVEX_X_I_(psllw, SSE2, "psllw", A, B, I)
#define psllwv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psllwv, SSE2, "psllw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSLLW_H_ */
