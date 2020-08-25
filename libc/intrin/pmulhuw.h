#ifndef COSMOPOLITAN_LIBC_INTRIN_PMULHUW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMULHUW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pmulhuw(uint16_t[8], const uint16_t[8], const uint16_t[8]);

#define pmulhuw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmulhuw, SSE2, "pmulhuw", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMULHUW_H_ */
