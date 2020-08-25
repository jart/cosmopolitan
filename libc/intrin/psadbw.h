#ifndef COSMOPOLITAN_LIBC_INTRIN_PSADBW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSADBW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psadbw(uint64_t[2], const uint8_t[16], const uint8_t[16]);

#define psadbw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psadbw, SSE2, "psadbw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSADBW_H_ */
