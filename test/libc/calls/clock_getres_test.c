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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

struct timespec ts;

TEST(clock_getres, realtimeHasMillisecondPrecisionOrBetter) {
  ASSERT_EQ(0, clock_getres(CLOCK_REALTIME, &ts));
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 1000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, realtimeFastHasMillisecondPrecisionOrBetter) {
  ASSERT_EQ(0, clock_getres(CLOCK_REALTIME_FAST, &ts));
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 1000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, realtimeCoarseHasMillisecondPrecisionOrBetter) {
  if (clock_getres(CLOCK_REALTIME_COARSE, &ts)) return;
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 100000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, realtimePreciseHasMillisecondPrecisionOrBetter) {
  if (clock_getres(CLOCK_REALTIME_PRECISE, &ts)) return;
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 100000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, monotonicHasMillisecondPrecisionOrBetter) {
  ASSERT_EQ(0, clock_getres(CLOCK_MONOTONIC, &ts));
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 1000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, monotonicFastHasMillisecondPrecisionOrBetter) {
  ASSERT_EQ(0, clock_getres(CLOCK_MONOTONIC_FAST, &ts));
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 1000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, monotonicCoarseHasMillisecondPrecisionOrBetter) {
  if (clock_getres(CLOCK_MONOTONIC_COARSE, &ts)) return;
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 100000000);
  EXPECT_GT(ts.tv_nsec, 0);
}

TEST(clock_getres, monotonicPreciseHasMillisecondPrecisionOrBetter) {
  if (clock_getres(CLOCK_MONOTONIC_PRECISE, &ts)) return;
  EXPECT_EQ(0, ts.tv_sec);
  EXPECT_LT(ts.tv_nsec, 100000000);
  EXPECT_GT(ts.tv_nsec, 0);
}
