#ifndef COSMOPOLITAN_LIBC_INTRIN_PABSD_H_
#define COSMOPOLITAN_LIBC_INTRIN_PABSD_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pabsd(uint32_t[4], const int32_t[4]);

#define pabsd(A, B) INTRIN_SSEVEX_X_X_(pabsd, SSSE3, "pabsd", A, B)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PABSD_H_ */
