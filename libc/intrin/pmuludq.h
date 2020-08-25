#ifndef COSMOPOLITAN_LIBC_INTRIN_PMULUDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMULUDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pmuludq(uint64_t[2], const uint32_t[4], const uint32_t[4]);

#define pmuludq(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmuludq, SSE2, "pmuludq", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMULUDQ_H_ */
