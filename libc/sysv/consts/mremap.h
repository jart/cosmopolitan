#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MREMAP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MREMAP_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MREMAP_FIXED;
hidden extern const long MREMAP_MAYMOVE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MREMAP_MAYMOVE LITERALLY(1)
#define MREMAP_FIXED   LITERALLY(2)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MREMAP_H_ */
