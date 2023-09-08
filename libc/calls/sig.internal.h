#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/atomic.h"
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

bool __sig_check(int);
bool __sig_is_core(int);
bool __sig_is_fatal(int);
bool __sig_handle(int, int, int, ucontext_t *);
int __sig_add(int, int, int);
int __sig_mask(int, const sigset_t *, sigset_t *);
void __sig_check_ignore(const int, const unsigned);
void __sig_pending(sigset_t *);
int __sig_is_applicable(struct Signal *);
bool __sig_deliver(int, int, int, ucontext_t *);
int __sig_tramp(struct Delivery *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
