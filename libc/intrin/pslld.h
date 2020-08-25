#ifndef COSMOPOLITAN_LIBC_INTRIN_PSLLD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSLLD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pslld(uint32_t[4], const uint32_t[4], unsigned char);
void pslldv(uint32_t[4], const uint32_t[4], const uint64_t[2]);

#define pslld(A, B, I) INTRIN_SSEVEX_X_I_(pslld, SSE2, "pslld", A, B, I)
#define pslldv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pslldv, SSE2, "pslld", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSLLD_H_ */
