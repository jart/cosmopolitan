#ifndef COSMOPOLITAN_LIBC_INTRIN_PSIGNB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSIGNB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psignb(int8_t[16], const int8_t[16], const int8_t[16]);

#define psignb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psignb, SSSE3, "psignb", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSIGNB_H_ */
