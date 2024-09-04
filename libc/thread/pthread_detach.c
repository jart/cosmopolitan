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
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

static errno_t pthread_detach_impl(struct PosixThread *pt) {
  enum PosixThreadStatus status, transition;
  status = atomic_load_explicit(&pt->pt_status, memory_order_relaxed);
  for (;;) {
    if (status == kPosixThreadJoinable) {
      transition = kPosixThreadDetached;
    } else if (status == kPosixThreadTerminated) {
      transition = kPosixThreadZombie;
    } else {
      return EINVAL;
    }
    if (atomic_compare_exchange_weak_explicit(&pt->pt_status, &status,
                                              transition, memory_order_release,
                                              memory_order_relaxed)) {
      if (transition == kPosixThreadZombie)
        _pthread_zombify(pt);
      return 0;
    }
  }
}

/**
 * Asks POSIX thread to free itself automatically upon termination.
 *
 * Detaching a non-joinable thread is undefined behavior. For example,
 * pthread_detach() can't be called twice on the same thread.
 *
 * @return 0 on success, or errno with error
 * @raise EINVAL if `thread` isn't joinable
 * @returnserrno
 */
errno_t pthread_detach(pthread_t thread) {
  unassert(thread);
  struct PosixThread *pt = (struct PosixThread *)thread;
  _pthread_ref(pt);
  int tid = _pthread_tid(pt);
  errno_t err = pthread_detach_impl(pt);
  _pthread_unref(pt);
  STRACE("pthread_detach(%d) → %s", tid, DescribeErrno(err));
  return err;
}
