#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int WNOHANG;
extern const int WUNTRACED;
extern const int WCONTINUED;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define WNOHANG    LITERALLY(1)
#define WUNTRACED  SYMBOLIC(WUNTRACED)
#define WCONTINUED SYMBOLIC(WCONTINUED)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_W_H_ */
