#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SHUT_RD;
hidden extern const long SHUT_RDWR;
hidden extern const long SHUT_WR;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define SHUT_RD LITERALLY(0)
#define SHUT_RDWR LITERALLY(2)
#define SHUT_WR LITERALLY(1)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SHUT_H_ */
