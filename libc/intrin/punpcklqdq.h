#ifndef COSMOPOLITAN_LIBC_INTRIN_PUNPCKLQDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PUNPCKLQDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void punpcklqdq(uint64_t[2], const uint64_t[2], const uint64_t[2]);

#define punpcklqdq(A, B, C)                                                   \
  INTRIN_SSEVEX_X_X_X_(punpcklqdq, SSE2, "punpcklqdq", INTRIN_NONCOMMUTATIVE, \
                       A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PUNPCKLQDQ_H_ */
