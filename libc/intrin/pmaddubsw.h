#ifndef COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

void pmaddubsw(int16_t[8], const uint8_t[16], const int8_t[16]);

#define pmaddubsw(W, B, C)                                                   \
  INTRIN_SSEVEX_X_X_X_(pmaddubsw, SSSE3, "pmaddubsw", INTRIN_NONCOMMUTATIVE, \
                       W, B, C)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMADDUBSW_H_ */
