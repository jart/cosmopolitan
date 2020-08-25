#ifndef COSMOPOLITAN_LIBC_INTRIN_POR_H_
#define COSMOPOLITAN_LIBC_INTRIN_POR_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void por(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define por(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(por, SSE2, "por", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_POR_H_ */
