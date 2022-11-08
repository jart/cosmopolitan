#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/atomic.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"

#define __SIG_QUEUE_LENGTH        32
#define __SIG_POLLING_INTERVAL_MS 50
#define __SIG_LOGGING_INTERVAL_MS 1700

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Signal {
  struct Signal *next;
  bool used;
  int tid;
  int sig;
  int si_code;
};

struct Signals {
  uint64_t sigmask; /* only if tls is disabled */
  struct Signal *queue;
  struct Signal mem[__SIG_QUEUE_LENGTH];
};

extern struct Signals __sig;
extern atomic_long __sig_count;

bool __sig_check(bool) _Hide;
bool __sig_handle(bool, int, int, ucontext_t *) _Hide;
int __sig_add(int, int, int) _Hide;
int __sig_mask(int, const sigset_t *, sigset_t *) _Hide;
int __sig_raise(int, int) _Hide;
void __sig_check_ignore(const int, const unsigned) _Hide;
void __sig_pending(sigset_t *) _Hide;
int __sig_is_applicable(struct Signal *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
