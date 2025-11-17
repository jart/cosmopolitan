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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/sysv/consts/clock.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

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
  struct timespec ts;
  BENCHMARK(1, 1, timespec_real());
  BENCHMARK(1000, 1, timespec_real());
  if (!clock_gettime(CLOCK_REALTIME, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_REALTIME, &ts));
  if (!clock_gettime(CLOCK_REALTIME_COARSE, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_REALTIME_COARSE, &ts));
  if (!clock_gettime(CLOCK_MONOTONIC, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_MONOTONIC, &ts));
  if (!clock_gettime(CLOCK_MONOTONIC_COARSE, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_MONOTONIC_COARSE, &ts));
  if (!clock_gettime(CLOCK_MONOTONIC_RAW, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_MONOTONIC_RAW, &ts));
  if (!clock_gettime(CLOCK_BOOTTIME, 0))
    BENCHMARK(1000, 1, clock_gettime(CLOCK_BOOTTIME, &ts));
}
