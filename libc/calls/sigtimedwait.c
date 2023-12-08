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
#include "libc/calls/sigtimedwait.h"
#include "libc/calls/asan.internal.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/sigtimedwait.internal.h"
#include "libc/calls/struct/siginfo.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Waits for signal synchronously, w/ timeout.
 *
 * @param set is signals for which we'll be waiting
 * @param info if not null shall receive info about signal
 * @param timeout is relative deadline and null means wait forever
 * @return signal number on success, or -1 w/ errno
 * @raise EINTR if an asynchronous signal was delivered instead
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise EINVAL if nanoseconds parameter was out of range
 * @raise EAGAIN if deadline expired
 * @raise ENOSYS on Windows, XNU, OpenBSD, Metal
 * @raise EFAULT if invalid memory was supplied
 * @cancelationpoint
 */
int sigtimedwait(const sigset_t *set, siginfo_t *info,
                 const struct timespec *timeout) {
  int rc;
  char strsig[21];
  struct timespec ts;
  union siginfo_meta si = {0};
  BEGIN_CANCELATION_POINT;

  if (IsAsan() && (!__asan_is_valid(set, sizeof(*set)) ||
                   (info && !__asan_is_valid(info, sizeof(*info))) ||
                   (timeout && !__asan_is_valid_timespec(timeout)))) {
    rc = efault();
  } else if (IsLinux() || IsFreebsd() || IsNetbsd()) {
    if (timeout) {
      // 1. Linux needs its size parameter
      // 2. NetBSD modifies timeout argument
      ts = *timeout;
      rc = sys_sigtimedwait(set, &si, &ts, 8);
    } else {
      rc = sys_sigtimedwait(set, &si, 0, 8);
    }
    if (rc != -1 && info) {
      __siginfo2cosmo(info, &si);
    }
  } else {
    rc = enosys();
  }

  END_CANCELATION_POINT;
  STRACE("sigtimedwait(%s, [%s], %s) → %s% m", DescribeSigset(0, set),
         DescribeSiginfo(rc, info), DescribeTimespec(0, timeout),
         strsignal_r(rc, strsig));
  return rc;
}
