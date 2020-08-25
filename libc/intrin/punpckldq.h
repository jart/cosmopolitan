#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKLDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKLDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void punpckldq(uint32_t[4], const uint32_t[4], const uint32_t[4]);

#define punpckldq(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpckldq, SSE2, "punpckldq", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKLDQ_H_ */
