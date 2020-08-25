#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDUSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDUSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddusw(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define paddusw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddusw, SSE2, "paddusw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDUSW_H_ */
