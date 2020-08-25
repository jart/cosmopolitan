#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRLQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRLQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psrlq(uint64_t[2], const uint64_t[2], unsigned char);
void psrlqv(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define psrlq(A, B, I) INTRIN_SSEVEX_X_I_(psrlq, SSE2, "psrlq", A, B, I)
#define psrlqv(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psrlqv, SSE2, "psrlq", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRLQ_H_ */
