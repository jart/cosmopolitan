#ifndef COSMOPOLITAN_LIBC_INTRIN_PSUBW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSUBW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void(psubw)(int16_t[8], const int16_t[8], const int16_t[8]);

#define psubw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psubw, SSE2, "psubw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSUBW_H_ */
