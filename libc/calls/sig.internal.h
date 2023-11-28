#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/thread/posixthread.internal.h"

#define SIG_HANDLED_NO_RESTART 1
#define SIG_HANDLED_SA_RESTART 2

COSMOPOLITAN_C_START_

struct Signals {
  _Atomic(uint64_t) pending;
  _Atomic(uint64_t) count;
};

extern struct Signals __sig;

bool __sig_ignored(int);
int __sig_check(void);
int __sig_kill(struct PosixThread *, int, int);
int __sig_mask(int, const sigset_t *, sigset_t *);
int __sig_relay(int, int, sigset_t);
int __sig_raise(int, int);
int __sig_get(sigset_t);
void __sig_delete(int);
void __sig_generate(int, int);
void __sig_init(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
