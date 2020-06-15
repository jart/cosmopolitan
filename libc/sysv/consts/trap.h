#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TRAP_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TRAP_H_
#include "libc/runtime/symbolic.h"

#define TRAP_BRKPT SYMBOLIC(TRAP_BRKPT)
#define TRAP_TRACE SYMBOLIC(TRAP_TRACE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long TRAP_BRKPT;
hidden extern const long TRAP_TRACE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TRAP_H_ */
