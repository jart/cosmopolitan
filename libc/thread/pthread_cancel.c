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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/enum/threadaccess.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thread.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

int systemfive_cancel(void);

extern const char systemfive_cancellable[];
extern const char systemfive_cancellable_end[];

long _pthread_cancel_ack(void) {
  struct PosixThread *pt = _pthread_self();
  if (!(pt->pt_flags & (PT_NOCANCEL | PT_MASKED)) ||
      (pt->pt_flags & PT_ASYNC)) {
    pthread_exit(PTHREAD_CANCELED);
  }
  pt->pt_flags |= PT_NOCANCEL | PT_OPENBSD_KLUDGE;
  return ecanceled();
}

static void _pthread_cancel_sig(int sig, siginfo_t *si, void *arg) {
  ucontext_t *ctx = arg;

  // check thread runtime state is initialized and cancelled
  struct PosixThread *pt;
  if (!__tls_enabled) return;
  if (!(pt = _pthread_self())) return;
  if (pt->pt_flags & PT_NOCANCEL) return;
  if (!atomic_load_explicit(&pt->cancelled, memory_order_acquire)) return;

  // in asynchronous mode we'll just the exit asynchronously
  if (pt->pt_flags & PT_ASYNC) {
    sigaddset(&ctx->uc_sigmask, SIGTHR);
    pthread_sigmask(SIG_SETMASK, &ctx->uc_sigmask, 0);
    pthread_exit(PTHREAD_CANCELED);
  }

  // prevent this handler from being called again by thread
  sigaddset(&ctx->uc_sigmask, SIGTHR);

  // check for race condition between pre-check and syscall
  // rewrite the thread's execution state to acknowledge it
  if (systemfive_cancellable <= (char *)ctx->uc_mcontext.PC &&
      (char *)ctx->uc_mcontext.PC < systemfive_cancellable_end) {
    ctx->uc_mcontext.PC = (intptr_t)systemfive_cancel;
    return;
  }

  // punts cancellation to start of next cancellation point
  // we ensure sigthr is a pending signal in case unblocked
  if (IsXnuSilicon()) {
    __syslib->__pthread_kill(_pthread_syshand(pt), sig);
  } else {
    sys_tkill(_pthread_tid(pt), sig, __get_tls());
  }
}

static void _pthread_cancel_listen(void) {
  struct sigaction sa;
  if (!IsWindows()) {
    sa.sa_sigaction = _pthread_cancel_sig;
    sa.sa_flags = SA_SIGINFO | SA_RESTART;
    memset(&sa.sa_mask, -1, sizeof(sa.sa_mask));
    npassert(!sigaction(SIGTHR, &sa, 0));
  }
}

static void pthread_cancel_nt(struct PosixThread *pt, intptr_t hThread) {
  uint32_t old_suspend_count;
  if ((pt->pt_flags & PT_ASYNC) && !(pt->pt_flags & PT_NOCANCEL)) {
    if ((old_suspend_count = SuspendThread(hThread)) != -1u) {
      if (!old_suspend_count) {
        struct NtContext cpu;
        cpu.ContextFlags = kNtContextControl | kNtContextInteger;
        if (GetThreadContext(hThread, &cpu)) {
          pt->pt_flags |= PT_NOCANCEL;
          cpu.Rip = (uintptr_t)pthread_exit;
          cpu.Rdi = (uintptr_t)PTHREAD_CANCELED;
          cpu.Rsp &= -16;
          *(uintptr_t *)(cpu.Rsp -= sizeof(uintptr_t)) = cpu.Rip;
          unassert(SetThreadContext(hThread, &cpu));
          __sig_cancel(pt, 0);
        }
      }
      ResumeThread(hThread);
    }
  }
}

static errno_t _pthread_cancel_impl(struct PosixThread *pt) {

  // install our special signal handler
  static bool once;
  if (!once) {
    _pthread_cancel_listen();
    once = true;
  }

  // check if thread is already dead
  switch (atomic_load_explicit(&pt->status, memory_order_acquire)) {
    case kPosixThreadZombie:
    case kPosixThreadTerminated:
      return ESRCH;
    default:
      break;
  }

  // flip the bit indicating that this thread is cancelled
  atomic_store_explicit(&pt->cancelled, 1, memory_order_release);

  // does this thread want to cancel itself?
  if (pt == _pthread_self()) {
    unassert(!(pt->pt_flags & PT_NOCANCEL));
    if (!(pt->pt_flags & (PT_NOCANCEL | PT_MASKED)) &&
        (pt->pt_flags & PT_ASYNC)) {
      pthread_exit(PTHREAD_CANCELED);
    }
    return 0;
  }

  errno_t err;
  if (IsWindows()) {
    pthread_cancel_nt(pt, _pthread_syshand(pt));
    err = 0;
  } else if (IsXnuSilicon()) {
    err = __syslib->__pthread_kill(_pthread_syshand(pt), SIGTHR);
  } else {
    int e = errno;
    if (!sys_tkill(_pthread_tid(pt), SIGTHR, pt->tib)) {
      err = 0;
    } else {
      err = errno;
      errno = e;
    }
  }
  if (err == ESRCH) {
    err = 0;  // we already reported this
  }
  return err;
}

/**
 * Cancels thread.
 *
 * When a thread is cancelled, it'll interrupt blocking i/o calls,
 * invoke any cleanup handlers that were pushed on the thread's stack
 * before the cancellation occurred, in addition to destructing pthread
 * keys, before finally, the thread shall abruptly exit.
 *
 * By default, pthread_cancel() can only take effect when a thread
 * reaches a cancellation point. Such functions are documented with
 * `@cancellationpoint`. They check the cancellation state before the
 * underlying system call is issued. If the system call is issued and
 * blocks, then pthread_cancel() will interrupt the operation in which
 * case the syscall wrapper will check the cancelled state a second
 * time, only if the raw system call returned EINTR.
 *
 * The following system calls are implemented as cancellation points.
 *
 * - `accept4`
 * - `accept`
 * - `clock_nanosleep`
 * - `connect`
 * - `copy_file_range`
 * - `creat`
 * - `epoll_wait`
 * - `fcntl(F_OFD_SETLKW)`
 * - `fcntl(F_SETLKW)`
 * - `fdatasync`
 * - `flock`
 * - `fstatfs`
 * - `fsync`
 * - `ftruncate`
 * - `getrandom`
 * - `msync`
 * - `nanosleep`
 * - `open`
 * - `openat`
 * - `pause`
 * - `poll`
 * - `ppoll`
 * - `pread`
 * - `preadv`
 * - `pselect`
 * - `pwrite`
 * - `pwritev`
 * - `read`
 * - `readv`
 * - `recvfrom`
 * - `recvmsg`
 * - `select`
 * - `sendmsg`
 * - `sendto`
 * - `sigsuspend`
 * - `sigtimedwait`
 * - `sigwaitinfo`
 * - `statfs`
 * - `tcdrain`
 * - `truncate`
 * - `wait3`
 * - `wait4`
 * - `wait`
 * - `waitpid`
 * - `write`
 * - `writev`
 *
 * The following library calls are implemented as cancellation points.
 *
 * - `fopen`
 * - `gzopen`, `gzread`, `gzwrite`, etc.
 * - `lockf(F_LOCK)`
 * - `nsync_cv_wait_with_deadline`
 * - `nsync_cv_wait`
 * - `opendir`
 * - `openatemp`, 'mkstemp', etc.
 * - `pclose`
 * - `popen`
 * - `fwrite`, `printf`, `fprintf`, `putc`, etc.
 * - `pthread_cond_timedwait`
 * - `pthread_cond_wait`
 * - `pthread_join`
 * - `sem_timedwait`
 * - `sem_wait`
 * - `sleep`
 * - `timespec_sleep_until`
 * - `tmpfd`
 * - `tmpfile`
 * - `usleep`
 *
 * Other userspace libraries provided by Cosmopolitan Libc that call the
 * cancellation points above will block cancellations while running. The
 * following are examples of functions that *aren't* cancellation points
 *
 * - `INFOF()`, `WARNF()`, etc.
 * - `getentropy`
 * - `gmtime_r`
 * - `kprintf` (by virtue of asm(syscall) and write_nocancel() on xnu)
 * - `localtime_r`
 * - `nsync_mu_lock`
 * - `nsync_mu_unlock`
 * - `openpty`
 * - `pthread_getname_np`
 * - `pthread_mutex_lock`
 * - `pthread_mutex_unlock`
 * - `pthread_setname_np`
 * - `sem_open`
 * - `system`
 * - `openatemp`, 'mkstemp', etc.
 * - `timespec_sleep`
 * - `touch`
 *
 * The way to block cancellations temporarily is:
 *
 *     int cs;
 *     pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
 *     // ...
 *     pthread_setcancelstate(cs, 0);
 *
 * In order to support cancellations all your code needs to be rewritten
 * so that when resources such as file descriptors are managed they must
 * have a cleanup crew pushed to the stack. For example even malloc() is
 * technically unsafe w.r.t. leaks without doing something like this:
 *
 *     void *p = malloc(123);
 *     pthread_cleanup_push(free, p);
 *     read(0, p, 123);
 *     pthread_cleanup_pop(1);
 *
 * Consider using Cosmopolitan Libc's garbage collector since it will be
 * executed when a thread exits due to a cancellation.
 *
 *     void *p = _gc(malloc(123));
 *     read(0, p, 123);
 *
 * It's possible to put a thread in asynchronous cancellation mode with
 *
 *     pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
 *     for (;;) donothing;
 *
 * In which case a thread may be cancelled at any assembly opcode. This
 * is useful for immediately halting threads that consume cpu and don't
 * use any system calls. It shouldn't be used on threads that will call
 * cancellation points since in that case asynchronous mode could cause
 * resource leaks to happen, in such a way that can't be worked around.
 *
 * If none of the above options seem savory to you, then a third way is
 * offered for doing cancellations. Cosmopolitan Libc supports the Musl
 * Libc `PTHREAD_CANCEL_MASKED` non-POSIX extension. Any thread may pass
 * this setting to pthread_setcancelstate(), in which case threads won't
 * be abruptly destroyed upon cancellation and have their stack unwound;
 * instead, cancellation points will simply raise an `ECANCELED` error,
 * which can be more safely and intuitively handled for many use cases.
 * For example:
 *
 *     pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
 *     void *p = malloc(123);
 *     int rc = read(0, p, 123);
 *     free(p);
 *     if (rc == ECANCELED) {
 *       pthread_exit(0);
 *     }
 *
 * Shows how the masked cancellations paradigm can be safely used. Note
 * that it's so important that cancellation point error return codes be
 * checked. Code such as the following:
 *
 *     pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
 *     void *p = malloc(123);
 *     write(2, "log\n", 4); // XXX: fails to check result
 *     int rc = read(0, p, 123);
 *     free(p);
 *     if (rc == ECANCELED) {
 *       pthread_exit(0); // XXX: not run if write() was cancelled
 *     }
 *
 * Isn't safe to use in masked mode. That's because if a cancellation
 * occurs during the write() operation then cancellations are blocked
 * while running read(). MASKED MODE DOESN'T HAVE SECOND CHANCES. You
 * must rigorously check the results of each cancellation point call.
 *
 * Unit tests should be able to safely ignore the return value, or at
 * the very least be programmed to consider ESRCH a successful status
 *
 * @param thread may be 0 to cancel all threads except self
 * @return 0 on success, or errno on error
 * @raise ESRCH if system thread wasn't alive or we lost a race
 */
errno_t pthread_cancel(pthread_t thread) {
  errno_t err;
  struct Dll *e;
  struct PosixThread *arg, *other;
  if ((arg = (struct PosixThread *)thread)) {
    err = _pthread_cancel_impl(arg);
  } else {
    err = ESRCH;
    pthread_spin_lock(&_pthread_lock);
    for (e = dll_first(_pthread_list); e; e = dll_next(_pthread_list, e)) {
      other = POSIXTHREAD_CONTAINER(e);
      if (other != _pthread_self() &&
          atomic_load_explicit(&other->status, memory_order_acquire) <
              kPosixThreadTerminated) {
        _pthread_cancel_impl(other);
        err = 0;
      }
    }
    pthread_spin_unlock(&_pthread_lock);
  }
  STRACE("pthread_cancel(%d) → %s", _pthread_tid(arg), DescribeErrno(err));
  return err;
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
  if (!(pt = _pthread_self())) return;
  if (pt->pt_flags & PT_NOCANCEL) return;
  if ((!(pt->pt_flags & PT_MASKED) || (pt->pt_flags & PT_ASYNC)) &&
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
  struct PosixThread *pt;
  if (!__tls_enabled) return 0;
  if (!(pt = _pthread_self())) return 0;
  if (pt->pt_flags & PT_NOCANCEL) return 0;
  if (!atomic_load_explicit(&pt->cancelled, memory_order_acquire)) return 0;
  if (!(pt->pt_flags & PT_MASKED) || (pt->pt_flags & PT_ASYNC)) {
    pthread_exit(PTHREAD_CANCELED);
  } else {
    pt->pt_flags |= PT_NOCANCEL;
    return ECANCELED;
  }
}
