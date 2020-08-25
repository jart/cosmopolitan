#ifndef COSMOPOLITAN_LIBC_INTRIN_PSUBUSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSUBUSB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void psubusb(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define psubusb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(psubusb, SSE2, "psubusb", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSUBUSB_H_ */
