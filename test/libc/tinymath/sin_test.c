/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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

#define sinl(x) sinl(VEIL("t", (long double)(x)))
#define sin(x)  sin(VEIL("x", (double)(x)))
#define sinf(x) sinf(VEIL("x", (float)(x)))

TEST(sinl, test) {
  EXPECT_STREQ("NAN", _gc(xdtoal(sinl(NAN))));
  EXPECT_STREQ("-NAN", _gc(xdtoal(sinl(+INFINITY))));
  EXPECT_STREQ("-NAN", _gc(xdtoal(sinl(-INFINITY))));
  EXPECT_STREQ(".479425538604203", _gc(xdtoal(sinl(.5))));
  EXPECT_STREQ("-.479425538604203", _gc(xdtoal(sinl(-.5))));
  EXPECT_STREQ(".8414709794048734", _gc(xdtoal(sinl(.99999999))));
  /* EXPECT_STREQ("-.998836772397", _gc(xdtoal(sinl(555555555555)))); */
  /* EXPECT_STREQ("1", _gc(xdtoal(sinl(5.319372648326541e+255L)))); */
}

TEST(sin, test) {
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", sin(0.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", sin(-0.))));
  EXPECT_STREQ("0.0998334166468282", _gc(xasprintf("%.15g", sin(.1))));
  EXPECT_STREQ("-0.0998334166468282", _gc(xasprintf("%.15g", sin(-.1))));
  EXPECT_STREQ("0.479425538604203", _gc(xasprintf("%.15g", sin(.5))));
  EXPECT_STREQ("-0.479425538604203", _gc(xasprintf("%.15g", sin(-.5))));
  EXPECT_STREQ("0.841470984807897", _gc(xasprintf("%.15g", sin(1.))));
  EXPECT_STREQ("-0.841470984807897", _gc(xasprintf("%.15g", sin(-1.))));
  EXPECT_STREQ("0.997494986604054", _gc(xasprintf("%.15g", sin(1.5))));
  EXPECT_STREQ("-0.997494986604054", _gc(xasprintf("%.15g", sin(-1.5))));
  EXPECT_STREQ("0.909297426825682", _gc(xasprintf("%.15g", sin(2.))));
  EXPECT_TRUE(isnan(sin(NAN)));
  EXPECT_TRUE(isnan(sin(-NAN)));
  EXPECT_TRUE(isnan(sin(INFINITY)));
  EXPECT_TRUE(isnan(sin(-INFINITY)));
  EXPECT_STREQ("2.2250738585072e-308",
               _gc(xasprintf("%.15g", sin(__DBL_MIN__))));
  EXPECT_STREQ("0.00496195478918406",
               _gc(xasprintf("%.15g", sin(__DBL_MAX__))));
  EXPECT_STREQ("-0.841470984807897",
               _gc(xasprintf("%.15g", sin(-1.0000000000000002))));
  EXPECT_STREQ("-2.1073424255447e-08",
               _gc(xasprintf("%.15g", sin(-2.1073424255447e-08))));
}

TEST(sinf, test) {
  EXPECT_TRUE(isnan(sinf(NAN)));
  EXPECT_TRUE(isnan(sinf(+INFINITY)));
  EXPECT_TRUE(isnan(sinf(-INFINITY)));
  EXPECT_STREQ("NAN", _gc(xdtoaf(sinf(NAN))));
  EXPECT_STARTSWITH(".479426", _gc(xdtoaf(sinf(.5f))));
  EXPECT_STARTSWITH("-.479426", _gc(xdtoaf(sinf(-.5f))));
  EXPECT_STARTSWITH(".873283", _gc(xdtoaf(sinf(555))));
}

BENCH(sin, bench) {
  double _sin(double) asm("sin");
  float _sinf(float) asm("sinf");
  long double _sinl(long double) asm("sinl");
  EZBENCH2("sin", donothing, _sin(.7));   /*  ~5ns */
  EZBENCH2("sinf", donothing, _sinf(.7)); /*  ~5ns */
  EZBENCH2("sinl", donothing, _sinl(.7)); /* ~28ns */
}
