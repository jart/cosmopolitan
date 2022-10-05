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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/futex.h"
#include "libc/thread/freebsd.internal.h"
#include "libc/thread/wait0.internal.h"

int _futex(atomic_int *, int, int, const struct timespec *);

static int _wait0_sleep(struct timespec *ts) {
  int rc, e = errno;
  if ((rc = nanosleep(ts, 0))) {
    _npassert(errno == EINTR);
    errno = e;
  }
  return rc;
}

static void _wait0_poll(struct timespec *ts) {
  if (ts->tv_nsec < 1000) {
    // prefer sched_yield() for small time intervals because nanosleep()
    // will ceiling round to 1ms on the new technology.
    sched_yield();
    ts->tv_nsec <<= 1;
  } else if (!_wait0_sleep(ts)) {
    if (ts->tv_nsec < 100 * 1000 * 1000) {
      ts->tv_nsec <<= 1;
    }
  }
}

static void _wait0_futex(const atomic_int *a, int e) {
  int rc, op;
  op = FUTEX_WAIT;  // we need a shared mutex
  if (IsWindows()) {
    if (WaitOnAddress(a, &e, sizeof(*a), -1)) {
      rc = 0;
    } else {
      rc = -GetLastError();
    }
  } else if (IsFreebsd()) {
    rc = sys_umtx_op(a, UMTX_OP_WAIT_UINT, 0, 0, 0);
  } else {
    rc = _futex(a, op, e, 0);
    if (IsOpenbsd() && rc > 0) {
      rc = -rc;
    }
  }
  STRACE("futex(%t, %s, %d, %s) → %s", a, DescribeFutexOp(op), e, "NULL",
         DescribeFutexResult(rc));
  _npassert(rc == 0 ||           //
            rc == -EINTR ||      //
            rc == -ETIMEDOUT ||  //
            rc == -EWOULDBLOCK);
}

/**
 * Blocks until memory location becomes zero.
 *
 * This is intended to be used on the child thread id, which is updated
 * by the clone() system call when a thread terminates. We need this in
 * order to know when it's safe to free a thread's stack. This function
 * uses futexes on Linux, OpenBSD, and Windows. On other platforms this
 * uses polling with exponential backoff.
 */
void _wait0(const atomic_int *ctid) {
  int x;
  struct timespec ts = {0, 1};
  while ((x = atomic_load_explicit(ctid, memory_order_acquire))) {
    if (IsLinux() || IsOpenbsd() || IsWindows()) {
      _wait0_futex(ctid, x);
    } else {
      _wait0_poll(&ts);
    }
  }
  if (IsOpenbsd()) {
    sched_yield();  // TODO(jart): whhhy?
  }
}
