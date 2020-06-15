#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MNT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MNT_H_
#include "libc/runtime/symbolic.h"

#define MNT_DETACH SYMBOLIC(MNT_DETACH)
#define MNT_EXPIRE SYMBOLIC(MNT_EXPIRE)
#define MNT_FORCE SYMBOLIC(MNT_FORCE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MNT_DETACH;
hidden extern const long MNT_EXPIRE;
hidden extern const long MNT_FORCE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MNT_H_ */
