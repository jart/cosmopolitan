#ifndef COSMOPOLITAN_LIBC_INTRIN_PABSB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PABSB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pabsb(uint8_t[16], const int8_t[16]);

#define pabsb(A, B) INTRIN_SSEVEX_X_X_(pabsb, SSSE3, "pabsb", A, B)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PABSB_H_ */
