#ifndef COSMOPOLITAN_LIBC_INTRIN_PHADDSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PHADDSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

void phaddsw(short[8], const short[8], const short[8]);

#define phaddsw(A, B, C)                                                       \
  INTRIN_SSEVEX_X_X_X_(phaddsw, SSSE3, "phaddsw", INTRIN_NONCOMMUTATIVE, A, B, \
                       C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PHADDSW_H_ */
