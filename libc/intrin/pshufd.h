#ifndef COSMOPOLITAN_LIBC_INTRIN_PSHUFD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSHUFD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pshufd(int32_t[4], const int32_t[4], uint8_t);

#define pshufd(A, B, I) INTRIN_SSEVEX_X_X_I_(pshufd, SSE2, "pshufd", A, B, I)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSHUFD_H_ */
