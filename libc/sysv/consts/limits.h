#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_
#include "libc/runtime/symbolic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const int NGROUPS_MAX;
extern const int PIPE_BUF;
extern const int SOMAXCONN;
extern const int _ARG_MAX;
extern const int _NAME_MAX;
extern const int _NSIG;
extern const int _PATH_MAX;
extern const int LINK_MAX;
extern const int MAX_CANON;
extern const int MAX_INPUT;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#define NGROUPS_MAX SYMBOLIC(NGROUPS_MAX)
#define PIPE_BUF    SYMBOLIC(PIPE_BUF)
#define SOMAXCONN   SYMBOLIC(SOMAXCONN)
#define _ARG_MAX    SYMBOLIC(_ARG_MAX)
#define _NAME_MAX   SYMBOLIC(_NAME_MAX)
#define _NSIG       SYMBOLIC(_NSIG)
#define _PATH_MAX   SYMBOLIC(_PATH_MAX)
#define LINK_MAX    SYMBOLIC(LINK_MAX)
#define MAX_CANON   SYMBOLIC(MAX_CANON)
#define MAX_INPUT   SYMBOLIC(MAX_INPUT)

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_LIMITS_H_ */
