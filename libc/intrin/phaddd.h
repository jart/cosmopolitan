#ifndef COSMOPOLITAN_LIBC_INTRIN_PHADDD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PHADDD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void phaddd(int32_t[4], const int32_t[4], const int32_t[4]);

#define phaddd(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(phaddd, SSSE3, "phaddd", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PHADDD_H_ */
