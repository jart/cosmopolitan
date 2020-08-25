#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddsw(int16_t[8], const int16_t[8], const int16_t[8]);

#define paddsw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddsw, SSE2, "paddsw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_ */
