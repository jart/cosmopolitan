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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/thread/wait0.internal.h"
#include "third_party/nsync/futex.internal.h"

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
 */
errno_t _wait0(const atomic_int *ctid) {
  int x, rc;
  while ((x = atomic_load_explicit(ctid, memory_order_acquire))) {
    if (nsync_futex_wait_(ctid, x, !IsWindows(), 0) == -ECANCELED) {
      return ECANCELED;
    }
  }
  return 0;
}
