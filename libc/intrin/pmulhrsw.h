#ifndef COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

void pmulhrsw(short a[8], const short b[8], const short c[8]);

#define pmulhrsw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pmulhrsw, SSSE3, "pmulhrsw", INTRIN_COMMUTATIVE, A, B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMULHRSW_H_ */
