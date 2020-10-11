#ifndef COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_
#define COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef void (*sigaction_f)(int, struct siginfo *, struct ucontext *);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_TYPEDEF_SIGACTION_F_H_ */
