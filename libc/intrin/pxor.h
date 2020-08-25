#ifndef COSMOPOLITAN_LIBC_INTRIN_PXOR_H_
#define COSMOPOLITAN_LIBC_INTRIN_PXOR_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pxor(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define pxor(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pxor, SSE2, "pxor", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PXOR_H_ */
