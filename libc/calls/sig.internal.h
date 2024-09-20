#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/atomic.h"
#include "libc/calls/struct/sigset.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/thread/posixthread.internal.h"

#define SIG_HANDLED_NO_RESTART 1
#define SIG_HANDLED_SA_RESTART 2

COSMOPOLITAN_C_START_

struct Signals {
  atomic_ulong *process;
  atomic_ulong count;
};

extern struct Signals __sig;

bool __sig_ignored(int);
int __sig_check(void);
int __sig_crash_sig(unsigned, int *);
int __sig_get(sigset_t);
int __sig_kill(struct PosixThread *, int, int);
int __sig_mask(int, const sigset_t *, sigset_t *);
int __sig_raise(int, int);
int __sig_relay(int, int, sigset_t);
void __sig_delete(int);
void __sig_generate(int, int);
void __sig_init(void);

__msabi char16_t *__sig_process_path(char16_t *, uint32_t, int);
__msabi atomic_ulong *__sig_map_process(int, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
