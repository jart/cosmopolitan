#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void punpcklwd(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define punpcklwd(A, B, C)                                                     \
  INTRIN_SSEVEX_X_X_X_(punpcklwd, SSE2, "punpcklwd", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKLWD_H_ */
