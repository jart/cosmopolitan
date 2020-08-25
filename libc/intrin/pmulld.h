#ifndef COSMOPOLITAN_LIBC_INTRIN_PMULLD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMULLD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pmulld(int32_t[4], const int32_t[4], const int32_t[4]);

#define pmulld(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmulld, SSE4_1, "pmulld", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMULLD_H_ */
