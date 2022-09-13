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
#include "libc/atomic.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/futex.h"
#include "libc/thread/thread.h"
#include "libc/thread/wait0.internal.h"

int _futex(atomic_int *, int, int, const struct timespec *);

/**
 * Blocks until memory location becomes zero.
 *
 * This is intended to be used on the child thread id, which is updated
 * by the _spawn() system call when a thread terminates. The purpose of
 * this operation is to know when it's safe to munmap() a threads stack
 */
void _wait0(const atomic_int *ctid) {
  int x, rc;
  char buf[12];
  for (;;) {
    if (!(x = atomic_load_explicit(ctid, memory_order_relaxed))) {
      break;
    } else if (IsLinux() || IsOpenbsd()) {
      rc = _futex(ctid, FUTEX_WAIT, x, &(struct timespec){2});
      STRACE("futex(%t, FUTEX_WAIT, %d, {2, 0}) → %s", ctid, x,
             (DescribeFutexResult)(buf, rc));
      if (IsOpenbsd() && rc > 0) rc = -rc;
      if (!(rc == 0 ||           //
            rc == -EINTR ||      //
            rc == -ETIMEDOUT ||  //
            rc == -EWOULDBLOCK)) {
        notpossible;
      }
    } else {
      pthread_yield();
    }
  }
  if (IsOpenbsd()) {
    // TODO(jart): Why do we need it? It's not even perfect.
    //             What's up with all these OpenBSD flakes??
    pthread_yield();
  }
}
