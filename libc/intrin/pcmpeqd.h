#ifndef COSMOPOLITAN_LIBC_INTRIN_PCMPEQD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PCMPEQD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pcmpeqd(int32_t[4], const int32_t[4], const int32_t[4]);

#define pcmpeqd(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pcmpeqd, SSE2, "pcmpeqd", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PCMPEQD_H_ */
