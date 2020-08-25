#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRLW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRLW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psrlw(uint16_t[8], const uint16_t[8], unsigned char);
void psrlwv(uint16_t[8], const uint16_t[8], const uint64_t[2]);

#define psrlw(A, B, I) INTRIN_SSEVEX_X_I_(psrlw, SSE2, "psrlw", A, B, I)
#define psrlwv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psrlwv, SSE2, "psrlw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRLW_H_ */
