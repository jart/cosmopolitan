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
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/futex.internal.h"
#include "libc/intrin/pthread.h"
#include "libc/sysv/consts/futex.h"

int _futex(void *, int, int, struct timespec *) hidden;

static dontinline int _futex_wait_impl(void *addr, int expect,
                                       struct timespec *timeout, int private) {
  int op, ax;
  if (IsLinux() || IsOpenbsd()) {
    op = FUTEX_WAIT | private;
    ax = _futex(addr, op, expect, timeout);
    if (SupportsLinux() && private && ax == -ENOSYS) {
      // RHEL5 doesn't support FUTEX_PRIVATE_FLAG
      op = FUTEX_WAIT;
      ax = _futex(addr, op, expect, timeout);
    }
    if (IsOpenbsd() && ax > 0) ax = -ax;  // yes openbsd does this w/o cf
    STRACE("futex(%t, %s, %d, %s) → %s", addr, DescribeFutexOp(op), expect,
           DescribeTimespec(0, timeout), DescribeFutexResult(ax));
    return ax;
  } else {
    return pthread_yield();
  }
}

int _futex_wait_public(void *addr, int expect, struct timespec *timeout) {
  return _futex_wait_impl(addr, expect, timeout, 0);
}

int _futex_wait_private(void *addr, int expect, struct timespec *timeout) {
  return _futex_wait_impl(addr, expect, timeout, FUTEX_PRIVATE_FLAG);
}
