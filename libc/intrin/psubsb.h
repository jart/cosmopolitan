#ifndef COSMOPOLITAN_LIBC_INTRIN_PSUBSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSUBSB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psubsb(int8_t[16], const int8_t[16], const int8_t[16]);

#define psubsb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psubsb, SSE2, "psubsb", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSUBSB_H_ */
