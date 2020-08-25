#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDSB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddsb(int8_t[16], const int8_t[16], const int8_t[16]);

#define paddsb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddsb, SSE2, "paddsb", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDSB_H_ */
