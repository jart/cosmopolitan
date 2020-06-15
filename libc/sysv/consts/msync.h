#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MSYNC_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MSYNC_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MS_SYNC;
hidden extern const long MS_ASYNC;
hidden extern const long MS_INVALIDATE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MS_SYNC       SYMBOLIC(MS_SYNC)
#define MS_ASYNC      LITERALLY(1)
#define MS_INVALIDATE SYMBOLIC(MS_INVALIDATE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MSYNC_H_ */
