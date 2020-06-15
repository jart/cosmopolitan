#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long LOCK_EX;
hidden extern const long LOCK_NB;
hidden extern const long LOCK_SH;
hidden extern const long LOCK_UN;
hidden extern const long LOCK_UNLOCK_CACHE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define LOCK_EX LITERALLY(2)
#define LOCK_NB SYMBOLIC(LOCK_NB)
#define LOCK_SH SYMBOLIC(LOCK_SH)
#define LOCK_UN SYMBOLIC(LOCK_UN)
#define LOCK_UNLOCK_CACHE SYMBOLIC(LOCK_UNLOCK_CACHE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_ */
