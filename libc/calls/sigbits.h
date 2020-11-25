#ifndef COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sigaddset(sigset_t *, int) paramsnonnull();
int sigdelset(sigset_t *, int) paramsnonnull();
int sigemptyset(sigset_t *) paramsnonnull();
int sigfillset(sigset_t *) paramsnonnull();
int sigismember(const sigset_t *, int) paramsnonnull() nosideeffect;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGBITS_H_ */
