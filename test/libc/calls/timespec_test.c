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
#include "libc/limits.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/testlib.h"
#include "third_party/nsync/time.h"

TEST(timespec_sub, test) {
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){-1},
                    timespec_sub((struct timespec){1}, (struct timespec){2})));
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){1},
                    timespec_sub((struct timespec){2}, (struct timespec){1})));
  EXPECT_TRUE(!timespec_cmp(
      (struct timespec){1, 1},
      timespec_sub((struct timespec){2, 2}, (struct timespec){1, 1})));
  EXPECT_TRUE(!timespec_cmp(
      (struct timespec){0, 999999999},
      timespec_sub((struct timespec){2, 1}, (struct timespec){1, 2})));
}

TEST(timespec_frommillis, test) {
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){0, 1000000}, timespec_frommillis(1)));
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){0, 2000000}, timespec_frommillis(2)));
  EXPECT_TRUE(!timespec_cmp((struct timespec){1}, timespec_frommillis(1000)));
}

TEST(timespec_frommicros, test) {
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){0, 1000}, timespec_frommicros(1)));
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){0, 2000}, timespec_frommicros(2)));
  EXPECT_TRUE(
      !timespec_cmp((struct timespec){1}, timespec_frommicros(1000000)));
  EXPECT_TRUE(!timespec_cmp((struct timespec){2, 123000},
                            timespec_frommicros(2000123)));
}

TEST(timespec_tomillis, test) {
  EXPECT_EQ(0, timespec_tomillis((struct timespec){0, 0}));
  EXPECT_EQ(1, timespec_tomillis((struct timespec){0, 1}));
  EXPECT_EQ(1, timespec_tomillis((struct timespec){0, 999999}));
  EXPECT_EQ(1, timespec_tomillis((struct timespec){0, 1000000}));
  EXPECT_EQ(1000, timespec_tomillis((struct timespec){0, 999999999}));
  EXPECT_EQ(2123, timespec_tomillis((struct timespec){2, 123000000}));
  EXPECT_EQ(INT64_MAX, timespec_tomillis((struct timespec){INT64_MAX, 0}));
  EXPECT_EQ(INT64_MIN, timespec_tomillis((struct timespec){INT64_MIN, 0}));
  EXPECT_EQ(INT64_MAX, timespec_tomillis(
                           (struct timespec){0x7fffffffffffffff, 999999999}));
}

TEST(timespec_tomicros, test) {
  EXPECT_EQ(0, timespec_tomicros((struct timespec){0, 0}));
  EXPECT_EQ(1, timespec_tomicros((struct timespec){0, 1}));
  EXPECT_EQ(2000123, timespec_tomicros((struct timespec){2, 123000}));
  EXPECT_EQ(INT64_MAX, timespec_tomicros((struct timespec){INT64_MAX, 0}));
  EXPECT_EQ(INT64_MIN, timespec_tomicros((struct timespec){INT64_MIN, 0}));
}

TEST(timespec_tonanos, test) {
  EXPECT_EQ(2000123000, timespec_tonanos((struct timespec){2, 123000}));
  EXPECT_EQ(INT64_MAX, timespec_tonanos((struct timespec){INT64_MAX, 0}));
  EXPECT_EQ(INT64_MIN, timespec_tonanos((struct timespec){INT64_MIN, 0}));
}

TEST(timeval_toseconds, test) {
  ASSERT_EQ(0, timeval_toseconds((struct timeval){0, 0}));
  ASSERT_EQ(1, timeval_toseconds((struct timeval){0, 1}));
  ASSERT_EQ(1, timeval_toseconds((struct timeval){0, 2}));
  ASSERT_EQ(1, timeval_toseconds((struct timeval){1, 0}));
  ASSERT_EQ(2, timeval_toseconds((struct timeval){1, 1}));
  ASSERT_EQ(INT64_MAX, timeval_toseconds(timeval_max));
}

static long mod(long x, long y) {
  if (y == -1) return 0;
  return x - y * (x / y - (x % y && (x ^ y) < 0));
}

TEST(timespec_sub, math) {
  for (int i = 0; i < 1000; ++i) {
    struct timespec x = {mod(lemur64(), 10), mod(lemur64(), 10)};
    struct timespec y = {mod(lemur64(), 10), mod(lemur64(), 10)};
    EXPECT_TRUE(!timespec_cmp(x, timespec_add(timespec_sub(x, y), y)));
  }
}
