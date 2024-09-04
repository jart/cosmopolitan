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
#include "libc/calls/struct/timespec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/runtime/clktck.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"
#include "libc/time.h"

static uint64_t hz_to_nanos(uint64_t frequency) {
  if (!frequency)
    return 0;
  uint64_t quotient = 1000000000 / frequency;
  uint64_t remainder = 1000000000 % frequency;
  if (remainder > 0)
    quotient += 1;
  return quotient;
}

static int sys_clock_getres_poly(int clock, struct timespec *ts, int64_t prec) {
  if (ts)
    ts->tv_sec = 0;
  if (clock == CLOCK_REALTIME ||   //
      clock == CLOCK_BOOTTIME ||   //
      clock == CLOCK_MONOTONIC ||  //
      clock == CLOCK_MONOTONIC_RAW) {
    if (ts)
      ts->tv_nsec = prec;
    return 0;
  } else if (clock == CLOCK_REALTIME_COARSE ||
             clock == CLOCK_MONOTONIC_COARSE ||
             clock == CLOCK_THREAD_CPUTIME_ID ||
             clock == CLOCK_PROCESS_CPUTIME_ID) {
    if (ts)
      *ts = timespec_fromnanos(hz_to_nanos(CLK_TCK));
    return 0;
  } else {
    return einval();
  }
}

static int sys_clock_getres_nt(int clock, struct timespec *ts) {
  return sys_clock_getres_poly(clock, ts, 100);
}

static int sys_clock_getres_xnu(int clock, struct timespec *ts) {
  return sys_clock_getres_poly(clock, ts, 1000);
}

/**
 * Returns granularity of clock.
 *
 * @return 0 on success, or -1 w/ errno
 * @error EPERM if pledge() is in play without stdio promise
 * @error EINVAL if `clock` isn't supported on this system
 * @error EFAULT if `ts` points to bad memory
 */
int clock_getres(int clock, struct timespec *ts) {
  int rc;
  if (!ts) {
    rc = efault();
  } else if (clock == 127) {
    rc = einval();  // 127 is used by consts.sh to mean unsupported
  } else if (IsWindows()) {
    rc = sys_clock_getres_nt(clock, ts);
  } else if (IsXnu()) {
    rc = sys_clock_getres_xnu(clock, ts);
  } else {
    rc = sys_clock_getres(clock, ts);
  }
  STRACE("clock_getres(%s, [%s]) → %d% m", DescribeClockName(clock),
         DescribeTimespec(rc, ts), rc);
  return rc;
}
