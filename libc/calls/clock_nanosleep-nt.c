/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/timer.h"
#include "libc/sysv/errfuns.h"

textwindows int sys_clock_nanosleep_nt(int clock, int flags,
                                       const struct timespec *req,
                                       struct timespec *rem) {
  struct timespec now, abs;
  if (flags & TIMER_ABSTIME) {
    abs = *req;
    for (;;) {
      if (sys_clock_gettime_nt(clock, &now)) return -1;
      if (timespec_cmp(now, abs) >= 0) return 0;
      if (_check_interrupts(false, g_fds.p)) return -1;
      SleepEx(MIN(__SIG_POLLING_INTERVAL_MS,
                  timespec_tomillis(timespec_sub(abs, now))),
              false);
    }
  } else {
    if (sys_clock_gettime_nt(clock, &now)) return -1;
    abs = timespec_add(now, *req);
    for (;;) {
      sys_clock_gettime_nt(clock, &now);
      if (timespec_cmp(now, abs) >= 0) return 0;
      if (_check_interrupts(false, g_fds.p)) {
        if (rem) *rem = timespec_sub(abs, now);
        return -1;
      }
      SleepEx(MIN(__SIG_POLLING_INTERVAL_MS,
                  timespec_tomillis(timespec_sub(abs, now))),
              false);
    }
  }
}
