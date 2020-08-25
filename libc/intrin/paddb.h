#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDB_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void paddb(int8_t[16], const int8_t[16], const int8_t[16]);

#define paddb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddb, SSE2, "paddb", INTRIN_COMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDB_H_ */
