#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#include "libc/runtime/symbolic.h"

#define WNOHANG SYMBOLIC(WNOHANG)
#define WUNTRACED SYMBOLIC(WUNTRACED)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long WNOHANG;
hidden extern const long WUNTRACED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_ */
