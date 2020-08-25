#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKHDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKHDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void punpckhdq(uint32_t[4], const uint32_t[4], const uint32_t[4]);

#define punpckhdq(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpckhdq, SSE2, "punpckhdq", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKHDQ_H_ */
