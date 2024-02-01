/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/syslib.internal.h"

#ifdef __aarch64__
#define CGT_VDSO __vdsosym("LINUX_2.6.39", "__kernel_clock_gettime")
#else
#define CGT_VDSO __vdsosym("LINUX_2.6", "__vdso_clock_gettime")
#endif

typedef int clock_gettime_f(int, struct timespec *);

static clock_gettime_f *__clock_gettime_get(void) {
  clock_gettime_f *cgt;
  if (IsLinux() && (cgt = CGT_VDSO)) {
    return cgt;
  } else if (__syslib) {
    return (void *)__syslib->__clock_gettime;
  } else if (IsWindows()) {
    return sys_clock_gettime_nt;
#ifdef __x86_64__
  } else if (IsXnu()) {
    return sys_clock_gettime_xnu;
#endif
  } else {
    return sys_clock_gettime;
  }
}

static int __clock_gettime_init(int, struct timespec *);
static clock_gettime_f *__clock_gettime = __clock_gettime_init;
static int __clock_gettime_init(int clockid, struct timespec *ts) {
  clock_gettime_f *cgt;
  __clock_gettime = cgt = __clock_gettime_get();
  return cgt(clockid, ts);
}

/**
 * Returns nanosecond time.
 *
 * @param clock supports the following values across OSes:
 *    - `CLOCK_REALTIME`
 *    - `CLOCK_MONOTONIC`
 *    - `CLOCK_REALTIME_COARSE`
 *    - `CLOCK_MONOTONIC_COARSE`
 *    - `CLOCK_THREAD_CPUTIME_ID`
 *    - `CLOCK_PROCESS_CPUTIME_ID`
 * @param ts is where the result is stored (or null to do clock check)
 * @return 0 on success, or -1 w/ errno
 * @raise EFAULT if `ts` points to invalid memory
 * @error EINVAL if `clock` isn't supported on this system
 * @error EPERM if pledge() is in play without stdio promise
 * @error ESRCH on NetBSD if PID/TID OR'd into `clock` wasn't found
 * @see strftime(), gettimeofday()
 * @asyncsignalsafe
 * @vforksafe
 */
int clock_gettime(int clock, struct timespec *ts) {
  // threads on win32 stacks call this so we can't asan check *ts
  int rc = __clock_gettime(clock, ts);
  if (rc) {
    errno = -rc;
    rc = -1;
  }
  TIMETRACE("clock_gettime(%s, [%s]) → %d% m", DescribeClockName(clock),
            DescribeTimespec(rc, ts), rc);
  return rc;
}
