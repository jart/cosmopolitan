#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_
#include "libc/runtime/symbolic.h"

#define SA_NOCLDSTOP SYMBOLIC(SA_NOCLDSTOP)
#define SA_NOCLDWAIT SYMBOLIC(SA_NOCLDWAIT)
#define SA_NODEFER SYMBOLIC(SA_NODEFER)
#define SA_NOMASK SYMBOLIC(SA_NOMASK)
#define SA_ONESHOT SYMBOLIC(SA_ONESHOT)
#define SA_ONSTACK SYMBOLIC(SA_ONSTACK)
#define SA_RESETHAND SYMBOLIC(SA_RESETHAND)
#define SA_RESTART SYMBOLIC(SA_RESTART)
#define SA_RESTORER SYMBOLIC(SA_RESTORER)
#define SA_SIGINFO SYMBOLIC(SA_SIGINFO)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern const long SA_NOCLDSTOP;
hidden extern const long SA_NOCLDWAIT;
hidden extern const long SA_NODEFER;
hidden extern const long SA_NOMASK;
hidden extern const long SA_ONESHOT;
hidden extern const long SA_ONSTACK;
hidden extern const long SA_RESETHAND;
hidden extern const long SA_RESTART;
hidden extern const long SA_RESTORER;
hidden extern const long SA_SIGINFO;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_SA_H_ */
