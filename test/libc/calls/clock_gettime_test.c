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
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

TEST(clock_gettime, fault) {
  ASSERT_SYS(EFAULT, -1, clock_gettime(0, 0));
}

TEST(clock_gettime, test) {
  bool isfast;
  struct timespec ts = {0};
  ASSERT_EQ(0, clock_gettime(0, &ts));
  ASSERT_NE(0, ts.tv_sec);
  ASSERT_NE(0, ts.tv_nsec);
  if (__is_linux_2_6_23()) {
    ASSERT_GT((intptr_t)__clock_gettime_get(&isfast),
              getauxval(AT_SYSINFO_EHDR));
    ASSERT_TRUE(isfast);
  }
}

BENCH(clock_gettime, bench) {
  struct timeval tv;
  struct timespec ts;
  gettimeofday(&tv, 0);   // trigger init
  clock_gettime(0, &ts);  // trigger init
  EZBENCH2("nowl", donothing, nowl());
  EZBENCH2("rdtsc", donothing, rdtsc());
  EZBENCH2("gettimeofday", donothing, gettimeofday(&tv, 0));
  EZBENCH2("timespec_real", donothing, timespec_real());
  EZBENCH2("clock_gettime 0", donothing,
           clock_gettime(CLOCK_REALTIME_FAST, &ts));
  EZBENCH2("clock_gettime 1", donothing,
           clock_gettime(CLOCK_MONOTONIC_FAST, &ts));
  EZBENCH2("__clock_gettime 0", donothing,
           __clock_gettime(CLOCK_REALTIME_FAST, &ts));
  EZBENCH2("__clock_gettime 1", donothing,
           __clock_gettime(CLOCK_MONOTONIC_FAST, &ts));
  if (IsWindows()) {
    EZBENCH2("sys_clock_gettime 0", donothing,
             sys_clock_gettime_nt(CLOCK_REALTIME_FAST, &ts));
    EZBENCH2("sys_clock_gettime 1", donothing,
             sys_clock_gettime_nt(CLOCK_MONOTONIC_FAST, &ts));
  } else {
    EZBENCH2("sys_clock_gettime 0", donothing,
             sys_clock_gettime(CLOCK_REALTIME_FAST, &ts));
    EZBENCH2("sys_clock_gettime 1", donothing,
             sys_clock_gettime(CLOCK_MONOTONIC_FAST, &ts));
  }
}
