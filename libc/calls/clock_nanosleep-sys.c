/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

int sys_clock_nanosleep(int clock, int flags,  //
                        const struct timespec *req, struct timespec *rem) {
  int rc;
  BEGIN_CANCELATION_POINT;
  if (IsLinux() || IsFreebsd() || IsNetbsd()) {
    rc = __sys_clock_nanosleep(clock, flags, req, rem);
  } else if (IsXnu()) {
    rc = sys_clock_nanosleep_xnu(clock, flags, req, rem);
  } else if (IsOpenbsd()) {
    rc = sys_clock_nanosleep_openbsd(clock, flags, req, rem);
  } else if (IsWindows()) {
    rc = sys_clock_nanosleep_nt(clock, flags, req, rem);
  } else {
    rc = enosys();
  }
  if (rc > 0) {
    errno = rc;
    rc = -1;
  }
  // system call support might not detect cancelation on bsds
  if (rc == -1 && errno == EINTR &&      //
      _weaken(pthread_testcancel_np) &&  //
      _weaken(pthread_testcancel_np)()) {
    rc = ecanceled();
  }
  END_CANCELATION_POINT;
  STRACE("sys_clock_nanosleep(%s, %s, %s, [%s]) → %d% m",
         DescribeClockName(clock), DescribeSleepFlags(flags),
         DescribeTimespec(0, req), DescribeTimespec(rc, rem), rc);
  return rc;
}
