#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FD_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FD_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long FD_CLOEXEC;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define FD_CLOEXEC LITERALLY(1)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FD_H_ */
