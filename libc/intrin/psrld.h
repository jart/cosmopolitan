#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRLD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRLD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psrld(uint32_t[4], const uint32_t[4], unsigned char);
void psrldv(uint32_t[4], const uint32_t[4], const uint64_t[2]);

#define psrld(A, B, I) INTRIN_SSEVEX_X_I_(psrld, SSE2, "psrld", A, B, I)
#define psrldv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psrldv, SSE2, "psrld", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRLD_H_ */
