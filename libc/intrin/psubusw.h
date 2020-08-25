#ifndef COSMOPOLITAN_LIBC_INTRIN_PSUBUSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSUBUSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psubusw(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define psubusw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psubusw, SSE2, "psubusw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSUBUSW_H_ */
