#ifndef COSMOPOLITAN_LIBC_INTRIN_PMINUB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMINUB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pminub(unsigned char[16], const unsigned char[16],
            const unsigned char[16]);

#define pminub(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pminub, SSE2, "pminub", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMINUB_H_ */
