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
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/strace.internal.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread2.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/futex.internal.h"

static const char *DescribeReturnValue(char buf[30], int err, void **value) {
  char *p = buf;
  if (!value) return "NULL";
  if (err) return "[n/a]";
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
 * @raise EBUSY if `abstime` was specified and deadline expired
 * @cancelationpoint
 */
static errno_t _pthread_wait(atomic_int *ctid, struct timespec *abstime) {
  int x, e, rc = 0;
  unassert(ctid != &__get_tls()->tib_tid);
  // "If the thread calling pthread_join() is canceled, then the target
  //  thread shall not be detached."  ──Quoth POSIX.1-2017
  if (!(rc = pthread_testcancel_np())) {
    BEGIN_CANCELATION_POINT;
    while ((x = atomic_load_explicit(ctid, memory_order_acquire))) {
      e = nsync_futex_wait_(ctid, x, !IsWindows() && !IsXnu(), abstime);
      if (e == -ECANCELED) {
        rc = ECANCELED;
        break;
      } else if (e == -ETIMEDOUT) {
        rc = EBUSY;
        break;
      }
    }
    END_CANCELATION_POINT;
  }
  return rc;
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
 * @raise EBUSY if `abstime` deadline elapsed
 * @cancelationpoint
 * @returnserrno
 */
errno_t pthread_timedjoin_np(pthread_t thread, void **value_ptr,
                             struct timespec *abstime) {
  errno_t err;
  struct PosixThread *pt;
  enum PosixThreadStatus status;
  pt = (struct PosixThread *)thread;
  status = atomic_load_explicit(&pt->pt_status, memory_order_acquire);
  // "The behavior is undefined if the value specified by the thread
  //  argument to pthread_join() does not refer to a joinable thread."
  //                                  ──Quoth POSIX.1-2017
  unassert(status == kPosixThreadJoinable || status == kPosixThreadTerminated);
  if (!(err = _pthread_wait(&pt->tib->tib_tid, abstime))) {
    _pthread_lock();
    dll_remove(&_pthread_list, &pt->list);
    _pthread_unlock();
    if (value_ptr) {
      *value_ptr = pt->pt_rc;
    }
    _pthread_unref(pt);
  }
  STRACE("pthread_timedjoin_np(%d, %s, %s) → %s", _pthread_tid(pt),
         DescribeReturnValue(alloca(30), err, value_ptr),
         DescribeTimespec(err ? -1 : 0, abstime), DescribeErrno(err));
  return err;
}
