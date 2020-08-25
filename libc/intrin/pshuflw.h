#ifndef COSMOPOLITAN_LIBC_INTRIN_PSHUFLW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSHUFLW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pshuflw(int16_t[8], const int16_t[8], uint8_t);

#define pshuflw(A, B, I) INTRIN_SSEVEX_X_X_I_(pshuflw, SSE2, "pshuflw", A, B, I)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSHUFLW_H_ */
