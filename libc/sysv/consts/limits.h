#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int PIPE_BUF;
extern const int _ARG_MAX;
extern const int _NAME_MAX;
extern const int _PATH_MAX;
extern const int _NSIG;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define PIPE_BUF  SYMBOLIC(PIPE_BUF)
#define _ARG_MAX  SYMBOLIC(_ARG_MAX)
#define _NAME_MAX SYMBOLIC(_NAME_MAX)
#define _PATH_MAX SYMBOLIC(_PATH_MAX)
#define _NSIG     SYMBOLIC(_NSIG)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_ */
