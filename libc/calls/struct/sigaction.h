#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/typedef/sigaction_f.h"
#include "libc/calls/typedef/sighandler_t.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct sigaction { /* cosmo abi */
  union {
    sighandler_t sa_handler;
    sigaction_f sa_sigaction;
  };
  uint64_t sa_flags;
  void (*sa_restorer)(void);
  struct sigset sa_mask;
  int64_t __pad;
} forcealign(8);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_H_ */
