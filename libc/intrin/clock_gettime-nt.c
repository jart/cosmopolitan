/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/wintime.internal.h"
#include "libc/nt/accounting.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/time.h"
#ifdef __x86_64__

#define _CLOCK_REALTIME           0
#define _CLOCK_MONOTONIC          1
#define _CLOCK_REALTIME_COARSE    2
#define _CLOCK_BOOTTIME           3
#define _CLOCK_PROCESS_CPUTIME_ID 4
#define _CLOCK_THREAD_CPUTIME_ID  5
#define _CLOCK_MONOTONIC_COARSE   6

textwindows int sys_clock_gettime_nt(int clock, struct timespec *ts) {
  uint64_t hectons;
  struct NtFileTime ft, ftExit, ftUser, ftKernel, ftCreation;
  switch (clock) {
    case _CLOCK_REALTIME:
      GetSystemTimePreciseAsFileTime(&ft);
      *ts = FileTimeToTimeSpec(ft);
      return 0;
    case _CLOCK_REALTIME_COARSE:
      GetSystemTimeAsFileTime(&ft);
      *ts = FileTimeToTimeSpec(ft);
      return 0;
    case _CLOCK_MONOTONIC:
      //
      // "If you need a higher resolution timer, use the
      //  QueryUnbiasedInterruptTime function, a multimedia timer, or a
      //  high-resolution timer. The elapsed time retrieved by the
      //  QueryUnbiasedInterruptTime function includes only time that
      //  the system spends in the working state."
      //
      //                     —Quoth MSDN § Windows Time
      //
      QueryUnbiasedInterruptTimePrecise(&hectons);
      *ts = WindowsDurationToTimeSpec(hectons);
      return 0;
    case _CLOCK_MONOTONIC_COARSE:
      //
      // "QueryUnbiasedInterruptTimePrecise is similar to the
      //  QueryUnbiasedInterruptTime routine, but is more precise. The
      //  interrupt time reported by QueryUnbiasedInterruptTime is based
      //  on the latest tick of the system clock timer. The system clock
      //  timer is the hardware timer that periodically generates
      //  interrupts for the system clock. The uniform period between
      //  system clock timer interrupts is referred to as a system clock
      //  tick, and is typically in the range of 0.5 milliseconds to
      //  15.625 milliseconds, depending on the hardware platform. The
      //  interrupt time value retrieved by QueryUnbiasedInterruptTime
      //  is accurate within a system clock tick. ¶To provide a system
      //  time value that is more precise than that of
      //  QueryUnbiasedInterruptTime, QueryUnbiasedInterruptTimePrecise
      //  reads the timer hardware directly, therefore a
      //  QueryUnbiasedInterruptTimePrecise call can be slower than a
      //  QueryUnbiasedInterruptTime call."
      //
      //                     —Quoth MSDN § QueryUnbiasedInterruptTimePrecise
      //
      QueryUnbiasedInterruptTime(&hectons);
      *ts = WindowsDurationToTimeSpec(hectons);
      return 0;
    case _CLOCK_BOOTTIME:
      //
      // "Unbiased interrupt-time means that only time that the system
      //  is in the working state is counted; therefore, the interrupt
      //  time count is not "biased" by time the system spends in sleep
      //  or hibernation."
      //
      //                     —Quoth MSDN § Interrupt Time
      //
      QueryInterruptTimePrecise(&hectons);
      *ts = WindowsDurationToTimeSpec(hectons);
      return 0;
    case _CLOCK_PROCESS_CPUTIME_ID:
      GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel,
                      &ftUser);
      *ts = WindowsDurationToTimeSpec(ReadFileTime(ftUser) +
                                      ReadFileTime(ftKernel));
      return 0;
    case _CLOCK_THREAD_CPUTIME_ID:
      GetThreadTimes(GetCurrentThread(), &ftCreation, &ftExit, &ftKernel,
                     &ftUser);
      *ts = WindowsDurationToTimeSpec(ReadFileTime(ftUser) +
                                      ReadFileTime(ftKernel));
      return 0;
    default:
      return -EINVAL;
  }
}

#endif  // __x86_64__
