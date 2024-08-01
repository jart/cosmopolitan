/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread2.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/futex.internal.h"

static const char *DescribeReturnValue(char buf[30], int err, void **value) {
  char *p = buf;
  if (!value)
    return "NULL";
  if (err)
    return "[n/a]";
  *p++ = '[';
  p = FormatHex64(p, (uintptr_t)*value, 1);
  *p++ = ']';
  *p = 0;
  return buf;
}

/**
 * Blocks until memory location becomes zero.
 *
 * This is intended to be used on the child thread id, which is updated
 * by the clone() system call when a thread terminates. We need this in
 * order to know when it's safe to free a thread's stack. This function
 * uses futexes on Linux, FreeBSD, OpenBSD, and Windows. On other
 * platforms this uses polling with exponential backoff.
 *
 * @return 0 on success, or errno on error
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @raise EDEADLK if `ctid` refers calling thread's own ctid futex
 * @raise EBUSY if `abstime` was specified and deadline expired
 * @cancelationpoint
 */
static errno_t _pthread_wait(atomic_int *ctid, struct timespec *abstime) {
  int x, e;
  errno_t err = 0;
  if (ctid == &__get_tls()->tib_tid) {
    // "If an implementation detects that the value specified by the
    //  thread argument to pthread_join() refers to the calling thread,
    //  it is recommended that the function should fail and report an
    //  [EDEADLK] error." ──Quoth POSIX.1-2017
    err = EDEADLK;
  } else {
    // "If the thread calling pthread_join() is canceled, then the target
    //  thread shall not be detached."  ──Quoth POSIX.1-2017
    if (!(err = pthread_testcancel_np())) {
      BEGIN_CANCELATION_POINT;
      while ((x = atomic_load_explicit(ctid, memory_order_acquire))) {
        e = nsync_futex_wait_(ctid, x, !IsWindows() && !IsXnu(), abstime);
        if (e == -ECANCELED) {
          err = ECANCELED;
          break;
        } else if (e == -ETIMEDOUT) {
          err = EBUSY;
          break;
        }
      }
      END_CANCELATION_POINT;
    }
  }
  return err;
}

/**
 * Waits for thread to terminate.
 *
 * Multiple threads joining the same thread is undefined behavior. If a
 * deferred or masked cancelation happens to the calling thread either
 * before or during the waiting process then the target thread will not
 * be joined. Calling pthread_join() on a non-joinable thread, e.g. one
 * that's been detached, is undefined behavior. If a thread attempts to
 * join itself, then the behavior is undefined.
 *
 * @param value_ptr if non-null will receive pthread_exit() argument
 *     if the thread called pthread_exit(), or `PTHREAD_CANCELED` if
 *     pthread_cancel() destroyed the thread instead
 * @param abstime specifies an absolute deadline or the timestamp of
 *     when we'll stop waiting; if this is null we will wait forever
 * @return 0 on success, or errno on error
 * @raise ECANCELED if calling thread was cancelled in masked mode
 * @raise EDEADLK if `thread` refers to calling thread
 * @raise EBUSY if `abstime` deadline elapsed
 * @cancelationpoint
 * @returnserrno
 */
errno_t pthread_timedjoin_np(pthread_t thread, void **value_ptr,
                             struct timespec *abstime) {
  int tid;
  errno_t err = 0;
  struct PosixThread *pt;
  enum PosixThreadStatus status;
  pt = (struct PosixThread *)thread;
  unassert(thread);

  // "The behavior is undefined if the value specified by the thread
  //  argument to pthread_join() does not refer to a joinable thread."
  //                                  ──Quoth POSIX.1-2017
  unassert((tid = _pthread_tid(pt)));
  status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
  unassert(status == kPosixThreadJoinable || status == kPosixThreadTerminated);

  // "The results of multiple simultaneous calls to pthread_join()
  //  specifying the same target thread are undefined."
  //                                  ──Quoth POSIX.1-2017
  if (!(err = _pthread_wait(&pt->tib->tib_tid, abstime))) {
    atomic_store_explicit(&pt->pt_status, kPosixThreadZombie,
                          memory_order_release);
    _pthread_zombify(pt);
    if (value_ptr)
      *value_ptr = pt->pt_rc;
  }

  STRACE("pthread_timedjoin_np(%d, %s, %s) → %s", tid,
         DescribeReturnValue(alloca(30), err, value_ptr),
         DescribeTimespec(err ? -1 : 0, abstime), DescribeErrno(err));
  return err;
}
