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
#include "libc/bits/likely.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/synchronization.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/clockstonanos.internal.h"

textwindows int sys_clock_gettime_nt(int clockid, struct timespec *ts) {
  uint64_t nanos;
  static bool once;
  static char lock;
  struct timespec res;
  static uint64_t base;
  struct NtFileTime ft;
  static struct timespec mono;
  if (!ts) return efault();
  if (clockid == CLOCK_REALTIME) {
    GetSystemTimeAsFileTime(&ft);
    *ts = FileTimeToTimeSpec(ft);
    return 0;
  } else if ((clockid == CLOCK_MONOTONIC || clockid == CLOCK_MONOTONIC_RAW) &&
             X86_HAVE(INVTSC)) {
    // this routine stops being monotonic after 194 years of uptime
    if (__threaded) _spinlock(&lock);
    if (UNLIKELY(!once)) {
      GetSystemTimeAsFileTime(&ft);
      mono = FileTimeToTimeSpec(ft);
      base = rdtsc();
      once = true;
    }
    nanos = ClocksToNanos(rdtsc(), base);
    res = mono;
    res.tv_sec += nanos / 1000000000;
    res.tv_nsec += nanos % 1000000000;
    _spunlock(&lock);
    *ts = res;
    return 0;
  } else {
    return einval();
  }
}
