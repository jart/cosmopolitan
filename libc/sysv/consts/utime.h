#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_UTIME_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_UTIME_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const int UTIME_NOW;
hidden extern const int UTIME_OMIT;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define UTIME_NOW  SYMBOLIC(UTIME_NOW)
#define UTIME_OMIT SYMBOLIC(UTIME_OMIT)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_UTIME_H_ */
