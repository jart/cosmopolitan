#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddd(int32_t[4], const int32_t[4], const int32_t[4]);

#define paddd(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddd, SSE2, "paddd", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDD_H_ */
