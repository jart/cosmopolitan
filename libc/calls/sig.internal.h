#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"

#define __SIG_QUEUE_LENGTH        8
#define __SIG_POLLING_INTERVAL_MS 50
#define __SIG_LOGGING_INTERVAL_MS 1700

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Signal {
  struct Signal *next;
  bool used;
  int sig;
  int si_code;
};

struct Signals {
  sigset_t mask;
  struct Signal *queue;
  struct Signal mem[__SIG_QUEUE_LENGTH];
};

extern struct Signals __sig;  // TODO(jart): Need TLS
extern long __sig_count;

void __sig_lock(void) hidden;
void __sig_unlock(void) hidden;
bool __sig_check(bool) hidden;
bool __sig_handle(bool, int, int, ucontext_t *) hidden;
int __sig_add(int, int) hidden;
int __sig_mask(int, const sigset_t *, sigset_t *) hidden;
int __sig_raise(int, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
