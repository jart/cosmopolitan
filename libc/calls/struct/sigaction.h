#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
COSMOPOLITAN_C_START_

typedef void (*sighandler_t)(int);
typedef void (*sigaction_f)(int, struct siginfo *, void *);

struct sigaction {
  union {
    sighandler_t sa_handler;
    sigaction_f sa_sigaction;
  };
  uint64_t sa_flags;
  void (*sa_restorer)(void);
  sigset_t sa_mask;
};

sighandler_t signal(int, sighandler_t) libcesque;
int sigaction(int, const struct sigaction *, struct sigaction *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_ */
