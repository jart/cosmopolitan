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
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/kprintf.h"
#include "libc/rand/rand.h"
#include "libc/testlib/testlib.h"

TEST(_timespec_gte, test) {
  EXPECT_FALSE(_timespec_gte((struct timespec){1}, (struct timespec){2}));
  EXPECT_TRUE(_timespec_gte((struct timespec){2}, (struct timespec){2}));
  EXPECT_TRUE(_timespec_gte((struct timespec){2}, (struct timespec){1}));
  EXPECT_FALSE(_timespec_gte((struct timespec){2}, (struct timespec){2, 1}));
  EXPECT_TRUE(_timespec_gte((struct timespec){2, 1}, (struct timespec){2}));
}

TEST(_timespec_sub, test) {
  EXPECT_TRUE(
      _timespec_eq((struct timespec){-1},
                   _timespec_sub((struct timespec){1}, (struct timespec){2})));
  EXPECT_TRUE(
      _timespec_eq((struct timespec){1},
                   _timespec_sub((struct timespec){2}, (struct timespec){1})));
  EXPECT_TRUE(_timespec_eq(
      (struct timespec){1, 1},
      _timespec_sub((struct timespec){2, 2}, (struct timespec){1, 1})));
  EXPECT_TRUE(_timespec_eq(
      (struct timespec){0, 999999999},
      _timespec_sub((struct timespec){2, 1}, (struct timespec){1, 2})));
}

TEST(_timespec_frommillis, test) {
  EXPECT_TRUE(
      _timespec_eq((struct timespec){0, 1000000}, _timespec_frommillis(1)));
  EXPECT_TRUE(
      _timespec_eq((struct timespec){0, 2000000}, _timespec_frommillis(2)));
  EXPECT_TRUE(_timespec_eq((struct timespec){1}, _timespec_frommillis(1000)));
}

TEST(_timespec_frommicros, test) {
  EXPECT_TRUE(
      _timespec_eq((struct timespec){0, 1000}, _timespec_frommicros(1)));
  EXPECT_TRUE(
      _timespec_eq((struct timespec){0, 2000}, _timespec_frommicros(2)));
  EXPECT_TRUE(
      _timespec_eq((struct timespec){1}, _timespec_frommicros(1000000)));
  EXPECT_TRUE(_timespec_eq((struct timespec){2, 123000},
                           _timespec_frommicros(2000123)));
}

TEST(_timespec_tomillis, test) {
  EXPECT_EQ(2123, _timespec_tomillis((struct timespec){2, 123000000}));
}

TEST(_timespec_tomicros, test) {
  EXPECT_EQ(2000123, _timespec_tomicros((struct timespec){2, 123000}));
}

TEST(_timespec_tonanos, test) {
  EXPECT_EQ(2000123000, _timespec_tonanos((struct timespec){2, 123000}));
}

static long mod(long x, long y) {
  if (y == -1) return 0;
  return x - y * (x / y - (x % y && (x ^ y) < 0));
}

TEST(_timespec_sub, math) {
  for (int i = 0; i < 1000; ++i) {
    struct timespec x = {mod(lemur64(), 10), mod(lemur64(), 10)};
    struct timespec y = {mod(lemur64(), 10), mod(lemur64(), 10)};
    struct timespec z = _timespec_add(_timespec_sub(x, y), y);
    EXPECT_TRUE(_timespec_eq(x, _timespec_add(_timespec_sub(x, y), y)));
  }
}
