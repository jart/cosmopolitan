#ifndef COSMOPOLITAN_LIBC_INTRIN_PANDN_H_
#define COSMOPOLITAN_LIBC_INTRIN_PANDN_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pandn(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define pandn(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pandn, SSE2, "pandn", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PANDN_H_ */
