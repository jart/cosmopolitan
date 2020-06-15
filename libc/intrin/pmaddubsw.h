#ifndef COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

void pmaddubsw(short[8], const unsigned char[16], const signed char[16]);

#define pmaddubsw(W, B, C)                                                   \
  INTRIN_SSEVEX_X_X_X_(pmaddubsw, SSSE3, "pmaddubsw", INTRIN_NONCOMMUTATIVE, \
                       W, B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_ */
