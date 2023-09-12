#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/nt/struct/context.h"

#define __SIG_QUEUE_LENGTH        32
#define __SIG_POLLING_INTERVAL_MS 20
#define __SIG_LOGGING_INTERVAL_MS 1700

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Delivery {
  int ops;
  int sig;
  int sic;
  struct NtContext *nc;
};

struct Signals {
  uint64_t mask;
  uint64_t pending;
  uint64_t count;
};

extern struct Signals __sig;

bool __sig_check(int);
bool __sig_is_core(int);
bool __sig_is_ignored(int);
bool __sig_handle(int, int, int, ucontext_t *);
int __sig_mask(int, const sigset_t *, sigset_t *);
bool __sig_deliver(int, int, int, ucontext_t *);
int __sig_tramp(struct Delivery *);
bool32 __sig_notify(int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
