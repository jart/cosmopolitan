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
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/itimerval.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/time.h"

/**
 * Schedules delivery of one-shot or intermittent interrupt signal, e.g.
 *
 * Raise SIGALRM every 1.5s:
 *
 *     sigaction(SIGALRM,
 *               &(struct sigaction){.sa_handler = OnSigalrm},
 *               NULL);
 *     setitimer(ITIMER_REAL,
 *               &(const struct itimerval){{1, 500000},
 *                                         {1, 500000}},
 *               NULL);
 *
 * Single-shot alarm to interrupt connect() after 50ms:
 *
 *     sigaction(SIGALRM,
 *               &(struct sigaction){.sa_handler = OnSigalrm,
 *                                   .sa_flags = SA_RESETHAND},
 *               NULL);
 *     setitimer(ITIMER_REAL,
 *               &(const struct itimerval){{0, 0}, {0, 50000}},
 *               NULL);
 *     if (connect(...) == -1 && errno == EINTR) { ... }
 *
 * Disarm existing timer:
 *
 *     setitimer(ITIMER_REAL, &(const struct itimerval){0}, NULL);
 *
 * If the goal is to use alarms to interrupt blocking i/o routines, e.g.
 * read(), connect(), etc. then it's important to install the signal
 * handler using sigaction() rather than signal(), because the latter
 * sets the `SA_RESTART` flag.
 *
 * Timers are not inherited across fork.
 *
 * @param which can be ITIMER_REAL, ITIMER_VIRTUAL, etc.
 * @param newvalue specifies the interval ({0,0} means one-shot) and
 *     duration ({0,0} means disarm) in microseconds ∈ [0,999999] and
 *     if this parameter is NULL, we'll polyfill getitimer() behavior
 * @param out_opt_old may receive remainder of previous op (if any)
 * @return 0 on success or -1 w/ errno
 * @asyncsignalsafe
 */
int setitimer(int which, const struct itimerval *newvalue,
              struct itimerval *oldvalue) {
  int rc;
  if (IsAsan() &&
      ((newvalue && !__asan_is_valid(newvalue, sizeof(*newvalue))) ||
       (oldvalue && !__asan_is_valid(oldvalue, sizeof(*oldvalue))))) {
    rc = efault();
  } else if (!IsWindows()) {
    if (newvalue) {
      rc = sys_setitimer(which, newvalue, oldvalue);
    } else {
      rc = sys_getitimer(which, oldvalue);
    }
  } else {
    rc = sys_setitimer_nt(which, newvalue, oldvalue);
  }
  STRACE("setitimer(%s, %s, [%s]) → %d% m", DescribeItimer(which),
         DescribeItimerval(0, newvalue), DescribeItimerval(rc, oldvalue), rc);
  return rc;
}
