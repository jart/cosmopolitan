#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_MLOCK_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_MLOCK_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long MCL_CURRENT;
hidden extern const long MCL_FUTURE;
hidden extern const long MCL_ONFAULT;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define MCL_CURRENT LITERALLY(1)
#define MCL_FUTURE  LITERALLY(2)
#define MCL_ONFAULT SYMBOLIC(MCL_ONFAULT)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_MLOCK_H_ */
