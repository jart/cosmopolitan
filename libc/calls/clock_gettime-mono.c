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
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/pthread.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/errfuns.h"
#include "libc/time/clockstonanos.internal.h"

static struct {
  pthread_once_t once;
  uint64_t base;
  struct timespec mono;
} g_mono;

static void sys_clock_gettime_mono_init(void) {
  clock_gettime(CLOCK_REALTIME, &g_mono.mono);
  g_mono.base = rdtsc();
  g_mono.once = true;
}

int sys_clock_gettime_mono(struct timespec *ts) {
  // this routine stops being monotonic after 194 years of uptime
  uint64_t nanos;
  struct timespec res;
  if (X86_HAVE(INVTSC)) {
    pthread_once(&g_mono.once, sys_clock_gettime_mono_init);
    nanos = ClocksToNanos(rdtsc(), g_mono.base);
    res = g_mono.mono;
    res.tv_sec += nanos / 1000000000;
    res.tv_nsec += nanos % 1000000000;
    *ts = res;
    return 0;
  } else {
    return einval();
  }
}
