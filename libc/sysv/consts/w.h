#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#include "libc/runtime/symbolic.h"

#define WNOHANG    SYMBOLIC(WNOHANG)
#define WUNTRACED  SYMBOLIC(WUNTRACED)
#define WCONTINUED SYMBOLIC(WCONTINUED)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long WNOHANG;
hidden extern const long WUNTRACED;
hidden extern const long WCONTINUED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_ */
