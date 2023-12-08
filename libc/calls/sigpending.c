/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

/**
 * Determines the blocked pending signals
 *
 * @param pending is where the bitset of pending signals is returned,
 *     which may not be null
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if `pending` points to invalid memory
 * @asyncsignalsafe
 */
int sigpending(sigset_t *pending) {
  int rc;
  if (!pending || (IsAsan() && !__asan_is_valid(pending, sizeof(*pending)))) {
    rc = efault();
  } else if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    // 128 signals on NetBSD and FreeBSD, 64 on Linux, 32 on OpenBSD and XNU
    uint64_t mem[2];
    rc = sys_sigpending(mem, 8);
    if (IsOpenbsd()) {
      *pending = rc;
    } else {
      *pending = mem[0];
    }
    if (IsXnu() || IsOpenbsd()) {
      *pending &= 0xffffffff;
    }
    rc = 0;
  } else if (IsWindows()) {
    *pending = atomic_load_explicit(&__sig.pending, memory_order_acquire) |
               atomic_load_explicit(&__get_tls()->tib_sigpending,
                                    memory_order_acquire);
    rc = 0;
  } else {
    rc = enosys();
  }
  STRACE("sigpending([%s]) → %d% m", DescribeSigset(rc, pending), rc);
  return rc;
}
