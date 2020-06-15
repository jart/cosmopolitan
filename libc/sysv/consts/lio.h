#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LIO_H_
#include "libc/runtime/symbolic.h"

#define LIO_NOP SYMBOLIC(LIO_NOP)
#define LIO_NOWAIT SYMBOLIC(LIO_NOWAIT)
#define LIO_READ SYMBOLIC(LIO_READ)
#define LIO_WAIT SYMBOLIC(LIO_WAIT)
#define LIO_WRITE SYMBOLIC(LIO_WRITE)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long LIO_NOP;
hidden extern const long LIO_NOWAIT;
hidden extern const long LIO_READ;
hidden extern const long LIO_WAIT;
hidden extern const long LIO_WRITE;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LIO_H_ */
