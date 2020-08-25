#ifndef COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void packuswb(uint8_t[16], const int16_t[8], const int16_t[8]);

#define packuswb(A, B, C)                                                    \
  INTRIN_SSEVEX_X_X_X_(packuswb, SSE2, "packuswb", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_ */
