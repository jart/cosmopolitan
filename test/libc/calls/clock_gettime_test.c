/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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

TEST(clock_gettime, nullResult_validatesClockParam) {
  ASSERT_SYS(EINVAL, -1, clock_gettime(666, 0));
}

TEST(clock_gettime, test) {
  struct timespec ts = {0};
  ASSERT_EQ(0, clock_gettime(0, &ts));
  ASSERT_NE(0, ts.tv_sec);
  ASSERT_NE(0, ts.tv_nsec);
}

TEST(clock_gettime, testClockRealtime) {
  struct timeval tv;
  struct timespec ts;
  EXPECT_NE(-1, gettimeofday(&tv, NULL));
  EXPECT_NE(-1, clock_gettime(CLOCK_REALTIME, &ts));
  EXPECT_LT((unsigned)ABS(ts.tv_sec - tv.tv_sec), 5u);
}

TEST(clock_gettime, bench) {
  struct timeval tv;
  struct timespec ts;
  gettimeofday(&tv, 0);   // trigger init
  clock_gettime(0, &ts);  // trigger init
  EZBENCH2("rdtsc", donothing, rdtsc());
  EZBENCH2("clock_gettime(mono)", donothing,
           clock_gettime(CLOCK_MONOTONIC_FAST, &ts));
  EZBENCH2("clock_gettime(real)", donothing,
           clock_gettime(CLOCK_REALTIME_FAST, &ts));
  EZBENCH2("timespec_real", donothing, timespec_real());
  EZBENCH2("gettimeofday", donothing, gettimeofday(&tv, 0));
  if (IsWindows()) {
    EZBENCH2("sys_clock_gettime r", donothing,
             sys_clock_gettime_nt(CLOCK_REALTIME_FAST, &ts));
    EZBENCH2("sys_clock_gettime m", donothing,
             sys_clock_gettime_nt(CLOCK_MONOTONIC_FAST, &ts));
  } else {
    EZBENCH2("sys_clock_gettime r", donothing,
             sys_clock_gettime(CLOCK_REALTIME_FAST, &ts));
    EZBENCH2("sys_clock_gettime m", donothing,
             sys_clock_gettime(CLOCK_MONOTONIC_FAST, &ts));
  }
}
