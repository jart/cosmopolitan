#ifndef COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/thread/posixthread.internal.h"

#define __SIG_LOCK_INTERVAL_MS    1000 /* semaphore synchronization: solid */
#define __SIG_SIG_INTERVAL_MS     1000 /* posix signal polyfill also solid */
#define __SIG_PROC_INTERVAL_MS    1000 /* process waiting also pretty good */
#define __SIG_IO_INTERVAL_MS      1000 /* read/write cancel/notify is good */
#define __SIG_POLL_INTERVAL_MS    20   /* poll on windows is dumpster fire */
#define __SIG_LOGGING_INTERVAL_MS 1700
#define __SIG_QUEUE_LENGTH        32

#if !(__ASSEMBLER__ + __LINKER__ + 0)
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
int __sig_raise(int, int);
void __sig_cancel(struct PosixThread *, unsigned);
void __sig_generate(int, int);
void __sig_init(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGNALS_INTERNAL_H_ */
