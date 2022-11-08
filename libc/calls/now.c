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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/clock_gettime.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/initializer.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/thread/tls.h"
#include "libc/time/time.h"

static clock_gettime_f *__gettime;

static struct Now {
  uint64_t k0;
  long double r0, cpn;
} g_now;

static long double GetTimeSample(void) {
  uint64_t tick1, tick2;
  long double time1, time2;
  sched_yield();
  time1 = dtime(CLOCK_REALTIME_PRECISE);
  tick1 = rdtsc();
  nanosleep(&(struct timespec){0, 1000000}, NULL);
  time2 = dtime(CLOCK_REALTIME_PRECISE);
  tick2 = rdtsc();
  return (time2 - time1) * 1e9 / MAX(1, tick2 - tick1);
}

static long double MeasureNanosPerCycle(void) {
  int i, n;
  long double avg, samp;
  if (__tls_enabled) __get_tls()->tib_flags |= TIB_FLAG_TIME_CRITICAL;
  if (IsWindows()) {
    n = 10;
  } else {
    n = 5;
  }
  for (avg = 1.0L, i = 1; i < n; ++i) {
    samp = GetTimeSample();
    avg += (samp - avg) / i;
  }
  if (__tls_enabled) __get_tls()->tib_flags &= ~TIB_FLAG_TIME_CRITICAL;
  STRACE("MeasureNanosPerCycle cpn*1000=%d", (long)(avg * 1000));
  return avg;
}

void RefreshTime(void) {
  struct Now now;
  now.cpn = MeasureNanosPerCycle();
  now.r0 = dtime(CLOCK_REALTIME_PRECISE);
  now.k0 = rdtsc();
  memcpy(&g_now, &now, sizeof(now));
}

static long double nowl_sys(void) {
  return dtime(CLOCK_REALTIME_PRECISE);
}

static long double nowl_art(void) {
  uint64_t ticks = rdtsc() - g_now.k0;
  return g_now.r0 + (1 / 1e9L * (ticks * g_now.cpn));
}

static long double nowl_vdso(void) {
  long double secs;
  struct timespec tv;
  _unassert(__gettime);
  __gettime(CLOCK_REALTIME_PRECISE, &tv);
  secs = tv.tv_nsec;
  secs *= 1 / 1e9L;
  secs += tv.tv_sec;
  return secs;
}

long double nowl_setup(void) {
  bool isfast;
  uint64_t ticks;
  __gettime = __clock_gettime_get(&isfast);
  if (isfast) {
    nowl = nowl_vdso;
  } else if (X86_HAVE(INVTSC)) {
    RefreshTime();
    nowl = nowl_art;
  } else {
    nowl = nowl_sys;
  }
  return nowl();
}
