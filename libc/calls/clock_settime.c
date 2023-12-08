/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes time.
 */
int clock_settime(int clockid, const struct timespec *ts) {
  int rc;
  struct timeval tv;
  if (clockid == 127) {
    rc = einval();  // 127 is used by consts.sh to mean unsupported
  } else if (!ts || (IsAsan() && !__asan_is_valid_timespec(ts))) {
    rc = efault();
  } else if (IsXnu()) {
    if (clockid == CLOCK_REALTIME) {
      tv = timespec_totimeval(*ts);
      rc = sys_settimeofday(&tv, 0);
    } else {
      rc = einval();
    }
  } else {
    rc = sys_clock_settime(clockid, ts);
  }
  STRACE("clock_settime(%s, %s) → %d% m", DescribeClockName(clockid),
         DescribeTimespec(0, ts), rc);
  return rc;
}
