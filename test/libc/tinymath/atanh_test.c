/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/x/xasprintf.h"

#define atanhl(x) atanhl(VEIL("t", (long double)(x)))
#define atanh(x)  atanh(VEIL("x", (double)(x)))
#define atanhf(x) atanhf(VEIL("x", (float)(x)))

TEST(atanh, test) {
  EXPECT_STREQ("0", _gc(xdtoa(atanh(0))));
  EXPECT_STREQ("-0", _gc(xdtoa(atanh(-0.))));
  EXPECT_STREQ(".549306144334055", _gc(xdtoa(atanh(.5))));
  EXPECT_STREQ("-.549306144334055", _gc(xdtoa(atanh(-.5))));
  EXPECT_STREQ("INFINITY", _gc(xdtoa(atanh(+1))));
  EXPECT_STREQ("-INFINITY", _gc(xdtoa(atanh(-1))));
  EXPECT_TRUE(isnan(atanh(+1.1)));
  EXPECT_TRUE(isnan(atanh(-1.1)));
  EXPECT_STREQ("-2.1073424255447e-08",
               _gc(xasprintf("%.15g", atanh(-2.1073424255447e-08))));
}

TEST(atanhl, test) {
  EXPECT_STREQ("0", _gc(xdtoal(atanhl(0))));
  EXPECT_STREQ("-0", _gc(xdtoal(atanhl(-0.))));
  EXPECT_STREQ(".5493061443340548", _gc(xdtoal(atanhl(.5))));
  EXPECT_STREQ("-.5493061443340548", _gc(xdtoal(atanhl(-.5))));
  EXPECT_STREQ("INFINITY", _gc(xdtoal(atanhl(+1))));
  EXPECT_STREQ("-INFINITY", _gc(xdtoal(atanhl(-1))));
  EXPECT_TRUE(isnan(atanhl(+1.1)));
  EXPECT_TRUE(isnan(atanhl(-1.1)));
}

TEST(atanhf, test) {
  EXPECT_STREQ("0", _gc(xdtoaf(atanhf(0))));
  EXPECT_STREQ("-0", _gc(xdtoaf(atanhf(-0.))));
  EXPECT_STREQ(".549306", _gc(xdtoaf(atanhf(.5))));
  EXPECT_STREQ("-.549306", _gc(xdtoaf(atanhf(-.5))));
  EXPECT_STREQ("INFINITY", _gc(xdtoaf(atanhf(+1))));
  EXPECT_STREQ("-INFINITY", _gc(xdtoaf(atanhf(-1))));
  EXPECT_TRUE(isnan(atanhf(+1.1)));
  EXPECT_TRUE(isnan(atanhf(-1.1)));
}

BENCH(atanh, bench) {
  volatile double a = .5;
  EZBENCH2("atanhf", donothing, EXPROPRIATE(atanhf(a)));
  EZBENCH2("atanh", donothing, EXPROPRIATE(atanh(a)));
  EZBENCH2("atanhl", donothing, EXPROPRIATE(atanhl(a)));
}
