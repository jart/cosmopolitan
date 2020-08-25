#ifndef COSMOPOLITAN_LIBC_INTRIN_PAVGB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PAVGB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pavgb(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define pavgb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pavgb, SSE2, "pavgb", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PAVGB_H_ */
