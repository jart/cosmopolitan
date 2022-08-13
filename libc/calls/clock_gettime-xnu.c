/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/clock_gettime.internal.h"
#include "libc/calls/struct/timeval.internal.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"

int sys_clock_gettime_xnu(int clock, struct timespec *ts) {
  axdx_t ad;
  if (clock == CLOCK_REALTIME) {
    ad = sys_gettimeofday((struct timeval *)ts, 0, 0);
    if (ad.ax != -1) {
      if (ad.ax) {
        ts->tv_sec = ad.ax;
        ts->tv_nsec = ad.dx;
      }
      ts->tv_nsec *= 1000;
      return 0;
    } else {
      return -1;
    }
  } else if (clock == CLOCK_MONOTONIC) {
    return sys_clock_gettime_mono(ts);
  } else {
    return einval();
  }
}
