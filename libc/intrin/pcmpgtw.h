#ifndef COSMOPOLITAN_LIBC_INTRIN_PCMPGTW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PCMPGTW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pcmpgtw(int16_t[8], const int16_t[8], const int16_t[8]);

#define pcmpgtw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pcmpgtw, SSE2, "pcmpgtw", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PCMPGTW_H_ */
