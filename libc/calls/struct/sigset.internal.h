#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __sys_sigprocmask(int, const struct sigset *, struct sigset *,
                      uint64_t) _Hide;
int sys_sigprocmask(int, const struct sigset *, struct sigset *) _Hide;
int sys_sigsuspend(const struct sigset *, uint64_t) _Hide;
int sys_sigpending(struct sigset *, size_t) _Hide;

const char *DescribeSigset(char[128], int, const sigset_t *);
#define DescribeSigset(rc, ss) DescribeSigset(alloca(128), rc, ss)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_ */
