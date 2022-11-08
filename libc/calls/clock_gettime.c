/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/asan.internal.h"
#include "libc/calls/clock_gettime.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asmflag.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/mem/alloca.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

/**
 * Returns nanosecond time.
 *
 * This is a high-precision timer that supports multiple definitions of
 * time. Among the more popular is CLOCK_MONOTONIC. This function has a
 * zero syscall implementation of that on modern x86.
 *
 *     nowl                l:        45𝑐        15𝑛𝑠
 *     rdtsc               l:        13𝑐         4𝑛𝑠
 *     gettimeofday        l:        44𝑐        14𝑛𝑠
 *     clock_gettime       l:        40𝑐        13𝑛𝑠
 *     __clock_gettime     l:        35𝑐        11𝑛𝑠
 *     sys_clock_gettime   l:       220𝑐        71𝑛𝑠
 *
 * @param clock can be one of:
 *    - `CLOCK_REALTIME`: universally supported
 *    - `CLOCK_REALTIME_FAST`: ditto but faster on freebsd
 *    - `CLOCK_REALTIME_PRECISE`: ditto but better on freebsd
 *    - `CLOCK_REALTIME_COARSE`: : like `CLOCK_REALTIME_FAST` w/ Linux 2.6.32+
 *    - `CLOCK_MONOTONIC`: universally supported
 *    - `CLOCK_MONOTONIC_FAST`: ditto but faster on freebsd
 *    - `CLOCK_MONOTONIC_PRECISE`: ditto but better on freebsd
 *    - `CLOCK_MONOTONIC_COARSE`: : like `CLOCK_MONOTONIC_FAST` w/ Linux 2.6.32+
 *    - `CLOCK_MONOTONIC_RAW`: is actually monotonic but needs Linux 2.6.28+
 *    - `CLOCK_PROCESS_CPUTIME_ID`: linux and bsd
 *    - `CLOCK_THREAD_CPUTIME_ID`: linux and bsd
 *    - `CLOCK_MONOTONIC_COARSE`: linux, freebsd
 *    - `CLOCK_PROF`: linux and netbsd
 *    - `CLOCK_BOOTTIME`: linux and openbsd
 *    - `CLOCK_REALTIME_ALARM`: linux-only
 *    - `CLOCK_BOOTTIME_ALARM`: linux-only
 *    - `CLOCK_TAI`: linux-only
 * @param ts is where the result is stored
 * @return 0 on success, or -1 w/ errno
 * @error EPERM if pledge() is in play without stdio promise
 * @error EINVAL if `clock` isn't supported on this system
 * @error EFAULT if `ts` points to bad memory
 * @see strftime(), gettimeofday()
 * @asyncsignalsafe
 * @threadsafe
 * @vforksafe
 */
int clock_gettime(int clock, struct timespec *ts) {
  int rc;
  if (clock == 127) {
    rc = einval();  // 127 is used by consts.sh to mean unsupported
  } else if (!ts || (IsAsan() && !__asan_is_valid_timespec(ts))) {
    rc = efault();
  } else {
    rc = __clock_gettime(clock, ts);
  }
#if SYSDEBUG
  if (__tls_enabled && !(__get_tls()->tib_flags & TIB_FLAG_TIME_CRITICAL)) {
    STRACE("clock_gettime(%s, [%s]) → %d% m", DescribeClockName(clock),
           DescribeTimespec(rc, ts), rc);
  }
#endif
  return rc;
}

/**
 * Returns pointer to fastest clock_gettime().
 */
clock_gettime_f *__clock_gettime_get(bool *opt_out_isfast) {
  bool isfast;
  clock_gettime_f *res;
  if (IsLinux() && (res = __vdsosym("LINUX_2.6", "__vdso_clock_gettime"))) {
    isfast = true;
  } else if (IsXnu()) {
    isfast = false;
    res = sys_clock_gettime_xnu;
  } else if (IsWindows()) {
    isfast = true;
    res = sys_clock_gettime_nt;
  } else {
    isfast = false;
    res = sys_clock_gettime;
  }
  if (opt_out_isfast) {
    *opt_out_isfast = isfast;
  }
  return res;
}

_Hide int __clock_gettime_init(int clockid, struct timespec *ts) {
  clock_gettime_f *gettime;
  __clock_gettime = gettime = __clock_gettime_get(0);
  return gettime(clockid, ts);
}
