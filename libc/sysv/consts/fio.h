#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long FIONBIO;
hidden extern const long FIONREAD;
hidden extern const long FIOASYNC;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define FIONBIO SYMBOLIC(FIONBIO)
#define FIONREAD SYMBOLIC(FIONREAD)
#define FIOASYNC SYMBOLIC(FIOASYNC)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_FIO_H_ */
