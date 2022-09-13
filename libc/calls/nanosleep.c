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
#include "libc/calls/asan.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sleeps for a particular amount of time.
 *
 * @param req is the duration of time we should sleep
 * @param rem if non-NULL will receive the amount of time that wasn't
 *     slept because a signal was delivered. If no signal's delivered
 *     then this value will be set to `{0, 0}`. It's also fine to set
 *     this value to the same pointer as `req`.
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `req->tv_nsec ∉ [0,1000000000)`
 * @raise EINTR if a signal was delivered, and `rem` is updated
 * @raise EFAULT if `req` is NULL or `req` / `rem` is a bad pointer
 * @raise ENOSYS on bare metal
 * @note POSIX.1 specifies nanosleep() measures against `CLOCK_REALTIME`
 *     however Linux measures uses  `CLOCK_MONOTONIC`. This shouldn't
 *     matter, since POSIX.1 further specifies that discontinuous
 *     changes in `CLOCK_REALTIME` shouldn't impact nanosleep()
 * @norestart
 */
int nanosleep(const struct timespec *req, struct timespec *rem) {
  int rc;

  if (!req || (IsAsan() && (!__asan_is_valid_timespec(req) ||
                            (rem && !__asan_is_valid_timespec(rem))))) {
    rc = efault();
  } else if (req->tv_sec < 0 ||
             !(0 <= req->tv_nsec && req->tv_nsec <= 999999999)) {
    rc = einval();
  } else if (!IsWindows() && !IsMetal() && !IsXnu()) {
    rc = sys_nanosleep(req, rem);
  } else if (IsXnu()) {
    rc = sys_nanosleep_xnu(req, rem);
  } else if (IsMetal()) {
    rc = enosys(); /* TODO: Sleep on Metal */
  } else {
    rc = sys_nanosleep_nt(req, rem);
  }

  // Linux Kernel doesn't change the remainder value on success, but
  // some kernels like OpenBSD will. POSIX doesn't specify the Linux
  // behavior. So we polyfill it here.
  if (!rc && rem) {
    rem->tv_sec = 0;
    rem->tv_nsec = 0;
  }

#if defined(SYSDEBUG) && _POLLTRACE
  if (!__time_critical) {
    POLLTRACE("nanosleep(%s, [%s]) → %d% m", DescribeTimespec(rc, req),
              DescribeTimespec(rc, rem), rc);
  }
#endif

  return rc;
}
