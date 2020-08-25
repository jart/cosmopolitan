#ifndef COSMOPOLITAN_LIBC_INTRIN_PABSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PABSW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pabsw(uint16_t[8], const int16_t[8]);

#define pabsw(A, B) INTRIN_SSEVEX_X_X_(pabsw, SSSE3, "pabsw", A, B)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PABSW_H_ */
