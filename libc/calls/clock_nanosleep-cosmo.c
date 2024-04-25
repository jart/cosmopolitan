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
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/timer.h"

/**
 * Sleeps with higher accuracy at the cost of cpu.
 */
int cosmo_clock_nanosleep(int clock, int flags, const struct timespec *req,
                          struct timespec *rem) {

  // pick clocks
  int time_clock;
  int sleep_clock;
  if (clock == CLOCK_REALTIME ||  //
      clock == CLOCK_REALTIME_PRECISE) {
    time_clock = clock;
    sleep_clock = CLOCK_REALTIME;
  } else if (clock == CLOCK_MONOTONIC ||  //
             clock == CLOCK_MONOTONIC_PRECISE) {
    time_clock = clock;
    sleep_clock = CLOCK_MONOTONIC;
  } else if (clock == CLOCK_REALTIME_COARSE ||  //
             clock == CLOCK_REALTIME_FAST) {
    return sys_clock_nanosleep(CLOCK_REALTIME, flags, req, rem);
  } else if (clock == CLOCK_MONOTONIC_COARSE ||  //
             clock == CLOCK_MONOTONIC_FAST) {
    return sys_clock_nanosleep(CLOCK_MONOTONIC, flags, req, rem);
  } else {
    return sys_clock_nanosleep(clock, flags, req, rem);
  }

  // sleep bulk of time in kernel
  struct timespec start, deadline, remain, waitfor, now;
  struct timespec quantum = timespec_fromnanos(1000000000 / CLK_TCK);
  clock_gettime(time_clock, &start);
  deadline = flags & TIMER_ABSTIME ? *req : timespec_add(start, *req);
  if (timespec_cmp(start, deadline) >= 0)
    return 0;
  remain = timespec_sub(deadline, start);
  if (timespec_cmp(remain, quantum) > 0) {
    waitfor = timespec_sub(remain, quantum);
    if (sys_clock_nanosleep(sleep_clock, 0, &waitfor, rem) == -1) {
      if (!flags && rem && errno == EINTR) {
        *rem = timespec_add(*rem, quantum);
      }
      return -1;
    }
  }

  // spin through final scheduling quantum
  int rc = 0;
  ftrace_enabled(-1);
  do {
    if (_check_cancel()) {
      rc = -1;
      break;
    }
    clock_gettime(time_clock, &now);
  } while (timespec_cmp(now, deadline) < 0);
  ftrace_enabled(+1);
  return rc;
}
