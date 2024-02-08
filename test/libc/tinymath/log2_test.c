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

TEST(log2l, test) {
  EXPECT_STREQ("1", gc(xdtoal(log2l(2))));
  EXPECT_STREQ("NAN", gc(xdtoal(log2l(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(log2l(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(log2l(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(log2l(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(log2l(-0.))));
  EXPECT_TRUE(isnan(log2l(-1)));
  EXPECT_TRUE(isnan(log2l(-2)));
}

TEST(log2, test) {
  EXPECT_STREQ("1", gc(xdtoa(log2(2))));
  EXPECT_STREQ("2", gc(xdtoa(log2(2 * 2))));
  EXPECT_STREQ("NAN", gc(xdtoa(log2(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(log2(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(log2(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log2(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log2(-0.))));
  EXPECT_TRUE(isnan(log2(-1)));
  EXPECT_TRUE(isnan(log2(-2)));
}

TEST(log2f, test) {
  EXPECT_STREQ("1", gc(xdtoaf(log2f(2))));
  EXPECT_STREQ("NAN", gc(xdtoaf(log2f(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(log2f(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(log2f(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(log2f(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(log2f(-0.))));
  EXPECT_TRUE(isnan(log2f(-1)));
  EXPECT_TRUE(isnan(log2f(-2)));
}

BENCH(log2, bench) {
  double _log2(double) asm("log2");
  float _log2f(float) asm("log2f");
  long double _log2l(long double) asm("log2l");
  EZBENCH2("log2", donothing, _log2(.7));    //  ~8ns
  EZBENCH2("log2f", donothing, _log2f(.7));  //  ~6ns
  EZBENCH2("log2l", donothing, _log2l(.7));  // ~21ns
}
