#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long LOCK_EX;
extern const long LOCK_NB;
extern const long LOCK_SH;
extern const long LOCK_UN;
extern const long LOCK_UNLOCK_CACHE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define LOCK_EX LITERALLY(2)
#define LOCK_NB SYMBOLIC(LOCK_NB)
#define LOCK_SH SYMBOLIC(LOCK_SH)
#define LOCK_UN SYMBOLIC(LOCK_UN)
#define LOCK_UNLOCK_CACHE SYMBOLIC(LOCK_UNLOCK_CACHE)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LOCK_H_ */
