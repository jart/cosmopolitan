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
#include "libc/x/xasprintf.h"

double _atanh(double) asm("atanh");
float _atanhf(float) asm("atanhf");
long double _atanhl(long double) asm("atanhl");

TEST(atanh, test) {
  EXPECT_STREQ("0", gc(xdtoa(_atanh(0))));
  EXPECT_STREQ("-0", gc(xdtoa(_atanh(-0.))));
  EXPECT_STREQ(".549306144334055", gc(xdtoa(_atanh(.5))));
  EXPECT_STREQ("-.549306144334055", gc(xdtoa(_atanh(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_atanh(+1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_atanh(-1))));
  EXPECT_TRUE(isnan(_atanh(+1.1)));
  EXPECT_TRUE(isnan(_atanh(-1.1)));
  EXPECT_STREQ("-2.1073424255447e-08",
               gc(xasprintf("%.15g", _atanh(-2.1073424255447e-08))));
}

TEST(atanhl, test) {
  EXPECT_STREQ("0", gc(xdtoal(_atanhl(0))));
  EXPECT_STREQ("-0", gc(xdtoal(_atanhl(-0.))));
  EXPECT_STREQ(".5493061443340548", gc(xdtoal(_atanhl(.5))));
  EXPECT_STREQ("-.5493061443340548", gc(xdtoal(_atanhl(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_atanhl(+1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_atanhl(-1))));
  EXPECT_TRUE(isnan(_atanhl(+1.1)));
  EXPECT_TRUE(isnan(_atanhl(-1.1)));
}

TEST(atanhf, test) {
  EXPECT_STREQ("0", gc(xdtoaf(_atanhf(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(_atanhf(-0.))));
  EXPECT_STREQ(".549306", gc(xdtoaf(_atanhf(.5))));
  EXPECT_STREQ("-.549306", gc(xdtoaf(_atanhf(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_atanhf(+1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_atanhf(-1))));
  EXPECT_TRUE(isnan(_atanhf(+1.1)));
  EXPECT_TRUE(isnan(_atanhf(-1.1)));
}

BENCH(_atanh, bench) {
  volatile double a = .5;
  EZBENCH2("atanhf", donothing, __expropriate(_atanhf(a)));
  EZBENCH2("atanh", donothing, __expropriate(_atanh(a)));
  EZBENCH2("atanhl", donothing, __expropriate(_atanhl(a)));
}
