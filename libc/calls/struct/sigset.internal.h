#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/mem/alloca.h"
#include "libc/sysv/consts/sig.h"
COSMOPOLITAN_C_START_

#ifndef MODE_DBG
/* block sigs because theoretical edge cases */
#define BLOCK_SIGNALS  \
  do {                 \
    sigset_t _SigMask; \
  _SigMask = __sig_block()
#define ALLOW_SIGNALS      \
  __sig_unblock(_SigMask); \
  }                        \
  while (0)
#else
/* doesn't block signals so we can get a crash
   report, when a core runtime library crashes */
#define BLOCK_SIGNALS  \
  do {                 \
    sigset_t _SigMask; \
  sigprocmask(SIG_SETMASK, 0, &_SigMask)
#define ALLOW_SIGNALS \
  }                   \
  while (0)
#endif

sigset_t __sig_block(void);
void __sig_unblock(sigset_t);
int __sys_sigprocmask(int, const uint64_t *, uint64_t *, uint64_t);
int sys_sigprocmask(int, const sigset_t *, sigset_t *);
int sys_sigsuspend(const uint64_t *, uint64_t);
int sys_sigpending(uint64_t *, size_t);

const char *_DescribeSigset(char[128], int, const sigset_t *);
#define DescribeSigset(rc, ss) _DescribeSigset(alloca(128), rc, ss)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGSET_INTERNAL_H_ */
