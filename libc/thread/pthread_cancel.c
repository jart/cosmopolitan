/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

extern const char systemfive_cancellable[] hidden;
extern const char systemfive_cancellable_end[] hidden;

int _pthread_cancel_sys(void) {
  struct PosixThread *pt;
  pt = (struct PosixThread *)__get_tls()->tib_pthread;
  if (!(pt->flags & (PT_NOCANCEL | PT_MASKED)) || (pt->flags & PT_ASYNC)) {
    pthread_exit(PTHREAD_CANCELED);
  }
  pt->flags |= PT_NOCANCEL;
  return ecanceled();
}

static void OnSigCancel(int sig, siginfo_t *si, void *ctx) {
  ucontext_t *uc = ctx;
  struct CosmoTib *tib = __get_tls();
  struct PosixThread *pt = (struct PosixThread *)tib->tib_pthread;
  if (pt && !(pt->flags & PT_NOCANCEL) &&
      atomic_load_explicit(&pt->cancelled, memory_order_acquire)) {
    sigaddset(&uc->uc_sigmask, sig);
    if ((pt->flags & PT_ASYNC) ||
        (systemfive_cancellable <= (char *)uc->uc_mcontext.rip &&
         (char *)uc->uc_mcontext.rip < systemfive_cancellable_end)) {
      uc->uc_mcontext.rip = (intptr_t)_pthread_cancel_sys;
    } else {
      tkill(atomic_load_explicit(&tib->tib_tid, memory_order_relaxed), sig);
    }
  }
}

static void ListenForSigCancel(void) {
  struct sigaction sa;
  sa.sa_sigaction = OnSigCancel;
  sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
  memset(&sa.sa_mask, -1, sizeof(sa.sa_mask));
  _npassert(!sigaction(SIGCANCEL, &sa, 0));
}

/**
 * Cancels thread.
 *
 * When a thread is cancelled, it'll interrupt blocking i/o calls,
 * invoke any cleanup handlers that were pushed on the thread's stack
 * as well as key destructors, and then the thread exits.
 *
 * By default, pthread_cancel() can only take effect when a thread is
 * blocked on a @cancellationpoint, which is any system call that's
 * specified as raising `EINTR`. For example, `openat`, `poll`, `ppoll`,
 * `select`, `pselect`, `read`, `readv`, `pread`, `preadv`, `write`,
 * `writev`, `pwrite`, `pwritev`, `accept`, `connect`, `recvmsg`,
 * `sendmsg`, `recv`, `send`, `tcdrain`, `clock_nanosleep`, `fsync`,
 * `fdatasync`, `fcntl(F_SETLKW)`, `epoll`, `sigsuspend`, `msync`,
 * `wait4`, `getrandom`, `pthread_cond_timedwait`, and `sem_timedwait`
 * are most cancellation points, plus many userspace libraries that call
 * the above functions, unless they're using pthread_setcancelstate() to
 * temporarily disable the cancellation mechanism. Some userspace
 * functions, e.g. system() will eagerly call pthread_testcancel_np() to
 * help avoid the potential for resource leaks later on.
 *
 * It's possible to put a thread in asynchronous cancellation mode using
 * pthread_setcanceltype(), thus allowing a cancellation to occur at any
 * assembly opcode. Please be warned that doing so is risky since it can
 * easily result in resource leaks. For example, a cancellation might be
 * triggered between calling open() and pthread_cleanup_push(), in which
 * case the application will leak a file descriptor.
 *
 * If none of the above options seem savory to you, then a third way is
 * offered for doing cancellations. Cosmopolitan Libc supports the Musl
 * Libc `PTHREAD_CANCEL_MASKED` non-POSIX extension. Any thread may pass
 * this setting to pthread_setcancelstate(), in which case threads won't
 * be abruptly destroyed upon cancellation and have their stack unwound;
 * instead, cancellation points will simply raise an `ECANCELED` error,
 * which can be more safely and intuitively handled for many use cases.
 *
 * @return 0 on success, or errno on error
 * @raise ESRCH if thread isn't alive
 */
errno_t pthread_cancel(pthread_t thread) {
  int e, rc, tid;
  static bool once;
  struct PosixThread *pt;
  __require_tls();
  if (!once) ListenForSigCancel(), once = true;
  pt = (struct PosixThread *)thread;
  switch (atomic_load_explicit(&pt->status, memory_order_acquire)) {
    case kPosixThreadZombie:
    case kPosixThreadTerminated:
      return ESRCH;
    default:
      break;
  }
  atomic_exchange_explicit(&pt->cancelled, 1, memory_order_release);
  if (thread == __get_tls()->tib_pthread) {
    if (!(pt->flags & (PT_NOCANCEL | PT_MASKED)) && (pt->flags & PT_ASYNC)) {
      pthread_exit(PTHREAD_CANCELED);
    }
    return 0;
  }
  if (IsWindows()) return 0;  // TODO(jart): Should we do this?
  tid = atomic_load_explicit(&pt->tib->tib_tid, memory_order_acquire);
  if (tid <= 0) return 0;  // TODO(jart): Do we need this?
  e = errno;
  if (!tkill(tid, SIGCANCEL)) {
    return 0;
  } else {
    rc = errno;
    errno = e;
    return rc;
  }
  return 0;
}

/**
 * Creates cancellation point in calling thread.
 *
 * This function can be used to force `PTHREAD_CANCEL_DEFERRED` threads
 * to cancel without needing to invoke an interruptible system call. If
 * the calling thread is in the `PTHREAD_CANCEL_DISABLE` then this will
 * do nothing. If the calling thread hasn't yet been cancelled, this'll
 * do nothing. In `PTHREAD_CANCEL_MASKED` mode, this also does nothing.
 *
 * @see pthread_testcancel_np()
 */
void pthread_testcancel(void) {
  struct PosixThread *pt;
  if (!__tls_enabled) return;
  pt = (struct PosixThread *)__get_tls()->tib_pthread;
  if (pt->flags & PT_NOCANCEL) return;
  if ((!(pt->flags & PT_MASKED) || (pt->flags & PT_ASYNC)) &&
      atomic_load_explicit(&pt->cancelled, memory_order_acquire)) {
    pthread_exit(PTHREAD_CANCELED);
  }
}

/**
 * Creates cancellation point in calling thread.
 *
 * This function can be used to force `PTHREAD_CANCEL_DEFERRED` threads
 * to cancel without needing to invoke an interruptible system call. If
 * the calling thread is in the `PTHREAD_CANCEL_DISABLE` then this will
 * do nothing. If the calling thread hasn't yet been cancelled, this'll
 * do nothing. If the calling thread uses `PTHREAD_CANCEL_MASKED`, then
 * this function returns `ECANCELED` if a cancellation occurred, rather
 * than the normal behavior which is to destroy and cleanup the thread.
 * Any `ECANCELED` result must not be ignored, because the thread shall
 * have cancellations disabled once it occurs.
 *
 * @return 0 if not cancelled or cancellation is blocked or `ECANCELED`
 *     in masked mode when the calling thread has been cancelled
 */
errno_t pthread_testcancel_np(void) {
  int rc;
  struct PosixThread *pt;
  if (!__tls_enabled) return 0;
  pt = (struct PosixThread *)__get_tls()->tib_pthread;
  if (pt->flags & PT_NOCANCEL) return 0;
  if (!atomic_load_explicit(&pt->cancelled, memory_order_acquire)) return 0;
  if (!(pt->flags & PT_MASKED) || (pt->flags & PT_ASYNC)) {
    pthread_exit(PTHREAD_CANCELED);
  } else {
    pt->flags |= PT_NOCANCEL;
    return ECANCELED;
  }
}
