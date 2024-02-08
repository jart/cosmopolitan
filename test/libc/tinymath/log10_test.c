/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _log10(double) asm("log10");
float _log10f(float) asm("log10f");
long double _log10l(long double) asm("log10l");

TEST(log10l, test) {
  EXPECT_STREQ("1", gc(xdtoal(_log10l(10))));
  EXPECT_STREQ("NAN", gc(xdtoal(_log10l(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(_log10l(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_log10l(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_log10l(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_log10l(-0.))));
  EXPECT_TRUE(isnan(_log10l(-1)));
  EXPECT_TRUE(isnan(_log10l(-2)));
}

TEST(log10, test) {
  EXPECT_STREQ(".301029995663981", gc(xdtoa(_log10(2))));
  EXPECT_STREQ("2", gc(xdtoa(_log10(100))));
  EXPECT_STREQ("NAN", gc(xdtoa(_log10(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(_log10(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_log10(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_log10(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_log10(-0.))));
  EXPECT_TRUE(isnan(_log10(-1)));
  EXPECT_TRUE(isnan(_log10(-2)));
}

TEST(log10f, test) {
  EXPECT_STREQ("1", gc(xdtoaf(_log10f(10))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_log10f(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(_log10f(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_log10f(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_log10f(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_log10f(-0.))));
  EXPECT_TRUE(isnan(_log10f(-1)));
  EXPECT_TRUE(isnan(_log10f(-2)));
}

BENCH(log10, bench) {
  EZBENCH2("log10", donothing, _log10(.7));    // ~17ns
  EZBENCH2("log10f", donothing, _log10f(.7));  // ~15ns
  EZBENCH2("log10l", donothing, _log10l(.7));  // ~21ns
}
