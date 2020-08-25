#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDUSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDUSB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddusb(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define paddusb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddusb, SSE2, "paddusb", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDUSB_H_ */
