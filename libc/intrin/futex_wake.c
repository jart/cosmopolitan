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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/futex.internal.h"
#include "libc/sysv/consts/futex.h"

int _futex(void *, int, int) hidden;

static dontinline int _futex_wake_impl(void *addr, int count, int private) {
  int op, ax;
  op = FUTEX_WAKE | private;
  ax = _futex(addr, op, count);
  if (SupportsLinux() && private && ax == -ENOSYS) {
    // RHEL5 doesn't support FUTEX_PRIVATE_FLAG
    op = FUTEX_WAKE;
    ax = _futex(addr, op, count);
  }
  STRACE("futex(%t, %s, %d) → %s", addr, DescribeFutexOp(op), count,
         DescribeFutexResult(ax));
  return ax;
}

int _futex_wake_public(void *addr, int count) {
  return _futex_wake_impl(addr, count, 0);
}

int _futex_wake_private(void *addr, int count) {
  return _futex_wake_impl(addr, count, FUTEX_PRIVATE_FLAG);
}
