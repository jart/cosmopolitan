#ifndef COSMOPOLITAN_LIBC_INTRIN_PHADDW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PHADDW_H_
#include "libc/intrin/macros.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

void phaddw(short[8], const short[8], const short[8]);

#define phaddw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(phaddw, SSSE3, "phaddw", INTRIN_NONCOMMUTATIVE, A, B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PHADDW_H_ */
