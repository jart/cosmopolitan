/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Determines the blocked pending signals
 *
 * @param pending is where the bitset of pending signals is returned,
 *     may not be NULL.
 * @return -1 w/ EFAULT
 * @asyncsignalsafe
 */
int sigpending(sigset_t *pending) {
  int rc;
  if (IsAsan() && !__asan_is_valid(pending, sizeof(*pending))) {
    rc = efault();
  } else if (IsLinux() || IsNetbsd() || IsOpenbsd() || IsFreebsd() || IsXnu()) {
    rc = sys_sigpending(pending, 8);
    // 128 signals on NetBSD and FreeBSD, 64 on Linux, 32 on OpenBSD and XNU.
    if (IsOpenbsd()) {
      pending->__bits[0] = (unsigned)rc;
      rc = 0;
    } else if (IsXnu()) {
      pending->__bits[0] &= 0xFFFFFFFF;
    }
    if (IsLinux() || IsOpenbsd() || IsXnu()) {
      pending->__bits[1] = 0;
    }
  } else if (IsWindows()) {
    sigemptyset(pending);
    __sig_pending(pending);
    rc = 0;
  } else {
    rc = enosys();
  }
  STRACE("sigpending([%s]) → %d% m", DescribeSigset(rc, pending), rc);
  return rc;
}
