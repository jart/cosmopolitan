/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/initializer.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"

static struct Now {
  bool once;
  uint64_t k0;
  long double r0, cpn;
} g_now;

static long double GetTimeSample(void) {
  uint64_t tick1, tick2;
  long double time1, time2;
  sched_yield();
  time1 = dtime(CLOCK_MONOTONIC);
  tick1 = rdtsc();
  nanosleep(&(struct timespec){0, 100000}, NULL);
  time2 = dtime(CLOCK_MONOTONIC);
  tick2 = rdtsc();
  return (time2 - time1) * 1e9 / MAX(1, tick2 - tick1);
}

static long double MeasureNanosPerCycle(void) {
  int i;
  long double avg, samp;
  for (avg = 1.0L, i = 1; i < 5; ++i) {
    samp = GetTimeSample();
    avg += (samp - avg) / i;
  }
  return avg;
}

static void InitTime(void) {
  g_now.cpn = MeasureNanosPerCycle();
  g_now.r0 = dtime(CLOCK_REALTIME);
  g_now.k0 = rdtsc();
  g_now.once = true;
}

long double converttickstonanos(uint64_t ticks) {
  if (!g_now.once) InitTime();
  return ticks * g_now.cpn; /* pico scale */
}

long double converttickstoseconds(uint64_t ticks) {
  return 1 / 1e9 * converttickstonanos(ticks);
}

long double nowl$sys(void) {
  return dtime(CLOCK_REALTIME);
}

long double nowl$art(void) {
  uint64_t ticks;
  if (!g_now.once) InitTime();
  ticks = unsignedsubtract(rdtsc(), g_now.k0);
  return g_now.r0 + converttickstoseconds(ticks);
}
