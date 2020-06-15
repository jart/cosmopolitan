/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/sysv/consts/clock.h"
#include "libc/time/time.h"

static struct Now {
  uint64_t k0;
  long double r0, cpn;
} now_;

/**
 * Returns timestamp without needing system calls.
 * @note uses microsecond scale fallback on k8 or vm
 */
long double (*nowl)(void);

long double converttickstonanos(uint64_t ticks) {
  return ticks * now_.cpn; /* pico scale */
}

long double converttickstoseconds(uint64_t ticks) {
  return 1 / 1e9 * converttickstonanos(ticks);
}

static long double nowl$sys(void) {
  return dtime(CLOCK_REALTIME);
}

static long double nowl$art(void) {
  uint64_t ticks;
  ticks = unsignedsubtract(rdtsc(), now_.k0);
  return now_.r0 + converttickstoseconds(ticks);
}

static long double GetSample(void) {
  uint64_t tick1, tick2;
  long double time1, time2;
  sched_yield();
  time1 = dtime(CLOCK_MONOTONIC);
  tick1 = rdtsc();
  nanosleep(&(struct timespec){0, 100000}, NULL);
  time2 = dtime(CLOCK_MONOTONIC);
  tick2 = rdtsc();
  return (time2 - time1) * 1e9 / (tick2 - tick1);
}

static long double MeasureNanosPerCycle(void) {
  int i;
  long double avg, samp;
  for (avg = 1.0L, i = 1; i < 5; ++i) {
    samp = GetSample();
    avg += (samp - avg) / i;
  }
  return avg;
}

INITIALIZER(301, _init_time, {
  if (X86_HAVE(INVTSC)) {
    now_.cpn = MeasureNanosPerCycle();
    now_.r0 = dtime(CLOCK_REALTIME);
    now_.k0 = rdtsc();
    nowl = nowl$art;
  } else {
    nowl = nowl$sys;
  }
})
