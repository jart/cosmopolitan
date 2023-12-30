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
#include "libc/atomic.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/freebsd.internal.h"

int sys_umtx_timedwait_uint_cp(atomic_int *, int, int, size_t,
                               struct _umtx_time *) asm("sys_futex_cp");

int sys_umtx_timedwait_uint(atomic_int *p, int expect, bool pshare,
                            const struct timespec *abstime) {
  int op;
  size_t size;
  struct _umtx_time *tm_p, timo;
  if (!abstime) {
    tm_p = 0;
    size = 0;
  } else {
    timo._clockid = CLOCK_REALTIME;
    timo._flags = UMTX_ABSTIME;
    timo._timeout = *abstime;
    tm_p = &timo;
    size = sizeof(timo);
  }
  if (pshare) {
    op = UMTX_OP_WAIT_UINT;
  } else {
    op = UMTX_OP_WAIT_UINT_PRIVATE;
  }
  return sys_umtx_timedwait_uint_cp(p, op, expect, size, tm_p);
}
