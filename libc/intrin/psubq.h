#ifndef COSMOPOLITAN_LIBC_INTRIN_PSUBQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSUBQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psubq(int64_t[2], const int64_t[2], const int64_t[2]);

#define psubq(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psubq, SSE2, "psubq", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSUBQ_H_ */
