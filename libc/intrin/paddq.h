#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddq(int64_t[2], const int64_t[2], const int64_t[2]);

#define paddq(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddq, SSE2, "paddq", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDQ_H_ */
