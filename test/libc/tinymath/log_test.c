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

TEST(log, test) {
  EXPECT_STREQ("1", gc(xdtoa(log(M_E))));
  EXPECT_STREQ("2", gc(xdtoa(log(M_E * M_E))));
  EXPECT_STREQ("NAN", gc(xdtoa(log(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(log(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(log(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log(-0.))));
  EXPECT_TRUE(isnan(log(-1)));
  EXPECT_TRUE(isnan(log(-2)));
}

TEST(logf, test) {
  EXPECT_STREQ("1", gc(xdtoaf(logf(M_E))));
  EXPECT_STREQ("NAN", gc(xdtoaf(logf(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(logf(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(logf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(logf(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(logf(-0.))));
  EXPECT_TRUE(isnan(logf(-1)));
  EXPECT_TRUE(isnan(logf(-2)));
}

TEST(logl, test) {
  EXPECT_STREQ("1", gc(xdtoal(logl(2.71828182845904523536L))));
  EXPECT_STREQ("NAN", gc(xdtoal(logl(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(logl(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(logl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(logl(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(logl(-0.))));
  EXPECT_TRUE(isnan(logl(-1)));
  EXPECT_TRUE(isnan(logl(-2)));
}

BENCH(logl, bench) {
  double _log(double) asm("log");
  float _logf(float) asm("logf");
  long double _logl(long double) asm("logl");
  EZBENCH2("log", donothing, _log(.7));    //  ~8ns
  EZBENCH2("logf", donothing, _logf(.7));  //  ~6ns
  EZBENCH2("logl", donothing, _logl(.7));  // ~21ns
}
