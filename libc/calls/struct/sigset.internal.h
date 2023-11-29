#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

#define BLOCK_SIGNALS  \
  do {                 \
    sigset_t _SigMask; \
  _SigMask = __sig_block()

#define ALLOW_SIGNALS      \
  __sig_unblock(_SigMask); \
  }                        \
  while (0)

sigset_t __sig_block(void);
void __sig_unblock(sigset_t);
int __sys_sigprocmask(int, const uint64_t *, uint64_t *, uint64_t);
int sys_sigprocmask(int, const sigset_t *, sigset_t *);
int sys_sigsuspend(const uint64_t *, uint64_t);
int sys_sigpending(uint64_t *, size_t);

const char *DescribeSigset(char[128], int, const sigset_t *);
#define DescribeSigset(rc, ss) DescribeSigset(alloca(128), rc, ss)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_ */
