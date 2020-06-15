#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_IOV_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_IOV_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long IOV_MAX;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define IOV_MAX SYMBOLIC(IOV_MAX)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_IOV_H_ */
