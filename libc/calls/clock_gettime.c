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
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sysv/consts/clock.h"

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
#ifdef __x86_64__
  } else if (IsWindows()) {
    return sys_clock_gettime_nt;
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

static int clock_gettime_impl(int clock, struct timespec *ts) {
  // BSDs and sometimes Linux too will crash when `ts` is NULL
  // it's also nice to not have to check for null in polyfills
  struct timespec memory;
  if (!ts)
    ts = &memory;
  return __clock_gettime(clock, ts);
}

/**
 * Returns nanosecond time.
 *
 * The `clock` parameter may bo set to:
 *
 * - `CLOCK_REALTIME` returns a wall clock timestamp represented in
 *   nanoseconds since the UNIX epoch (~1970). It'll count time in the
 *   suspend state. This clock is subject to being smeared by various
 *   adjustments made by NTP. These timestamps can have unpredictable
 *   discontinuous jumps when clock_settime() is used. Therefore this
 *   clock is the default clock for everything, even pthread condition
 *   variables. Cosmopoiltan guarantees this clock will never raise
 *   `EINVAL` and also guarantees `CLOCK_REALTIME == 0` will always be
 *   the case. On Windows this maps to GetSystemTimePreciseAsFileTime().
 *   On platforms with vDSOs like Linux, Windows, and MacOS ARM64 this
 *   should take about 20 nanoseconds.
 *
 * - `CLOCK_MONOTONIC` returns a timestamp with an unspecified epoch,
 *   that should be when the system was powered on. These timestamps
 *   shouldn't go backwards. Timestamps shouldn't count time spent in
 *   the sleep, suspend, and hibernation states. These timestamps won't
 *   be impacted by clock_settime(). These timestamps may be impacted by
 *   frequency adjustments made by NTP. Cosmopoiltan guarantees this
 *   clock will never raise `EINVAL`. MacOS and BSDs use the word
 *   "uptime" to describe this clock. On Windows this maps to
 *   QueryUnbiasedInterruptTimePrecise().
 *
 * - `CLOCK_BOOTTIME` is a monotonic clock returning a timestamp with an
 *   unspecified epoch, that should be relative to when the host system
 *   was powered on. These timestamps shouldn't go backwards. Timestamps
 *   should also include time spent in a sleep, suspend, or hibernation
 *   state. These timestamps aren't impacted by clock_settime(), but
 *   they may be impacted by frequency adjustments made by NTP. This
 *   clock will raise an `EINVAL` error on extremely old Linux distros
 *   like RHEL5. MacOS and BSDs use the word "monotonic" to describe
 *   this clock. On Windows this maps to QueryInterruptTimePrecise().
 *
 * - `CLOCK_MONOTONIC_RAW` returns a timestamp from an unspecified
 *   epoch. These timestamps don't count time spent in the sleep,
 *   suspend, and hibernation states. This clock is not impacted by
 *   clock_settime(). Unlike `CLOCK_MONOTONIC` this clock is guaranteed
 *   to not be impacted by frequency adjustments. Providing this level
 *   of assurances may make this clock 10x slower than the monotonic
 *   clock. Furthermore this clock may cause `EINVAL` to be raised if
 *   running on a host system that doesn't provide those guarantees,
 *   e.g. OpenBSD and MacOS on AMD64.
 *
 * - `CLOCK_REALTIME_COARSE` is the same as `CLOCK_REALTIME` except
 *   it'll go faster if the host OS provides a cheaper way to read the
 *   wall time. Please be warned that coarse can be really coarse.
 *   Rather than nano precision, you're looking at `CLK_TCK` precision,
 *   which can lag as far as 30 milliseconds behind or possibly more.
 *   Cosmopolitan may fallback to `CLOCK_REALTIME` if a faster less
 *   accurate clock isn't provided by the system. This clock will raise
 *   an `EINVAL` error on extremely old Linux distros like RHEL5. On
 *   platforms with vDSOs like Linux, Windows, and MacOS ARM64 this
 *   should take about 5 nanoseconds.
 *
 * - `CLOCK_MONOTONIC_COARSE` is the same as `CLOCK_MONOTONIC` except
 *   it'll go faster if the host OS provides a cheaper way to read the
 *   unbiased time. Please be warned that coarse can be really coarse.
 *   Rather than nano precision, you're looking at `CLK_TCK` precision,
 *   which can lag as far as 30 milliseconds behind or possibly more.
 *   Cosmopolitan may fallback to `CLOCK_REALTIME` if a faster less
 *   accurate clock isn't provided by the system. This clock will raise
 *   an `EINVAL` error on extremely old Linux distros like RHEL5. On
 *   platforms with vDSOs like Linux, Windows, and MacOS ARM64 this
 *   should take about 5 nanoseconds.
 *
 * - `CLOCK_PROCESS_CPUTIME_ID` returns the amount of time this process
 *   was actively scheduled. This is similar to getrusage() and clock().
 *
 * - `CLOCK_THREAD_CPUTIME_ID` returns the amount of time this thread
 *   was actively scheduled. This is similar to getrusage() and clock().
 *
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
  int rc = clock_gettime_impl(clock, ts);
  if (rc) {
    errno = -rc;
    rc = -1;
  }
  TIMETRACE("clock_gettime(%s, [%s]) → %d% m", DescribeClockName(clock),
            DescribeTimespec(rc, ts), rc);
  return rc;
}
