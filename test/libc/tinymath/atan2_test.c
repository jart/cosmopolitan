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
#include "libc/x/xasprintf.h"

TEST(atan2, yx) {
  EXPECT_STREQ("-0.321750554396642", _gc(xasprintf("%.15g", atan(-.5 / 1.5))));
  EXPECT_STREQ("-0.321750554396642", _gc(xasprintf("%.15g", atan2(-.5, 1.5))));
}

TEST(atan2, test) {
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., 0.))));
  EXPECT_STREQ("3.14159265358979", _gc(xasprintf("%.15g", atan2(0., -0.))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., .5))));
  EXPECT_STREQ("3.14159265358979", _gc(xasprintf("%.15g", atan2(0., -.5))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., 1.))));
  EXPECT_STREQ("3.14159265358979", _gc(xasprintf("%.15g", atan2(0., -1.))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., 1.5))));
  EXPECT_STREQ("3.14159265358979", _gc(xasprintf("%.15g", atan2(0., -1.5))));
  EXPECT_TRUE(isnan(atan2(0., NAN)));
  EXPECT_TRUE(isnan(atan2(0., -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(0., -INFINITY))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., __DBL_MIN__))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(0., __DBL_MAX__))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., 0.))));
  EXPECT_STREQ("-3.14159265358979", _gc(xasprintf("%.15g", atan2(-0., -0.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., .5))));
  EXPECT_STREQ("-3.14159265358979", _gc(xasprintf("%.15g", atan2(-0., -.5))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., 1.))));
  EXPECT_STREQ("-3.14159265358979", _gc(xasprintf("%.15g", atan2(-0., -1.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., 1.5))));
  EXPECT_STREQ("-3.14159265358979", _gc(xasprintf("%.15g", atan2(-0., -1.5))));
  EXPECT_TRUE(isnan(atan2(-0., NAN)));
  EXPECT_TRUE(isnan(atan2(-0., -NAN)));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., INFINITY))));
  EXPECT_STREQ("-3.14159265358979",
               _gc(xasprintf("%.15g", atan2(-0., -INFINITY))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., __DBL_MIN__))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-0., __DBL_MAX__))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(.5, 0.))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(.5, -0.))));
  EXPECT_STREQ("0.785398163397448", _gc(xasprintf("%.15g", atan2(.5, .5))));
  EXPECT_STREQ("2.35619449019234", _gc(xasprintf("%.15g", atan2(.5, -.5))));
  EXPECT_STREQ("0.463647609000806", _gc(xasprintf("%.15g", atan2(.5, 1.))));
  EXPECT_STREQ("2.67794504458899", _gc(xasprintf("%.15g", atan2(.5, -1.))));
  EXPECT_STREQ("0.321750554396642", _gc(xasprintf("%.15g", atan2(.5, 1.5))));
  EXPECT_STREQ("2.81984209919315", _gc(xasprintf("%.15g", atan2(.5, -1.5))));
  EXPECT_TRUE(isnan(atan2(.5, NAN)));
  EXPECT_TRUE(isnan(atan2(.5, -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(.5, INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(.5, -INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(.5, __DBL_MIN__))));
  EXPECT_STREQ("2.781342323134e-309",
               _gc(xasprintf("%.15g", atan2(.5, __DBL_MAX__))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-.5, 0.))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-.5, -0.))));
  EXPECT_STREQ("-0.785398163397448", _gc(xasprintf("%.15g", atan2(-.5, .5))));
  EXPECT_STREQ("-2.35619449019234", _gc(xasprintf("%.15g", atan2(-.5, -.5))));
  EXPECT_STREQ("-0.463647609000806", _gc(xasprintf("%.15g", atan2(-.5, 1.))));
  EXPECT_STREQ("-2.67794504458899", _gc(xasprintf("%.15g", atan2(-.5, -1.))));
  EXPECT_STREQ("-0.321750554396642", _gc(xasprintf("%.15g", atan2(-.5, 1.5))));
  EXPECT_STREQ("-2.81984209919315", _gc(xasprintf("%.15g", atan2(-.5, -1.5))));
  EXPECT_TRUE(isnan(atan2(-.5, NAN)));
  EXPECT_TRUE(isnan(atan2(-.5, -NAN)));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-.5, INFINITY))));
  EXPECT_STREQ("-3.14159265358979",
               _gc(xasprintf("%.15g", atan2(-.5, -INFINITY))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-.5, __DBL_MIN__))));
  EXPECT_STREQ("-2.781342323134e-309",
               _gc(xasprintf("%.15g", atan2(-.5, __DBL_MAX__))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(1., 0.))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(1., -0.))));
  EXPECT_STREQ("1.10714871779409", _gc(xasprintf("%.15g", atan2(1., .5))));
  EXPECT_STREQ("2.0344439357957", _gc(xasprintf("%.15g", atan2(1., -.5))));
  EXPECT_STREQ("0.785398163397448", _gc(xasprintf("%.15g", atan2(1., 1.))));
  EXPECT_STREQ("2.35619449019234", _gc(xasprintf("%.15g", atan2(1., -1.))));
  EXPECT_STREQ("0.588002603547568", _gc(xasprintf("%.15g", atan2(1., 1.5))));
  EXPECT_STREQ("2.55359005004223", _gc(xasprintf("%.15g", atan2(1., -1.5))));
  EXPECT_TRUE(isnan(atan2(1., NAN)));
  EXPECT_TRUE(isnan(atan2(1., -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(1., INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(1., -INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(1., __DBL_MIN__))));
  EXPECT_STREQ("5.562684646268e-309",
               _gc(xasprintf("%.15g", atan2(1., __DBL_MAX__))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-1., 0.))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-1., -0.))));
  EXPECT_STREQ("-1.10714871779409", _gc(xasprintf("%.15g", atan2(-1., .5))));
  EXPECT_STREQ("-2.0344439357957", _gc(xasprintf("%.15g", atan2(-1., -.5))));
  EXPECT_STREQ("-0.785398163397448", _gc(xasprintf("%.15g", atan2(-1., 1.))));
  EXPECT_STREQ("-2.35619449019234", _gc(xasprintf("%.15g", atan2(-1., -1.))));
  EXPECT_STREQ("-0.588002603547568", _gc(xasprintf("%.15g", atan2(-1., 1.5))));
  EXPECT_STREQ("-2.55359005004223", _gc(xasprintf("%.15g", atan2(-1., -1.5))));
  EXPECT_TRUE(isnan(atan2(-1., NAN)));
  EXPECT_TRUE(isnan(atan2(-1., -NAN)));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-1., INFINITY))));
  EXPECT_STREQ("-3.14159265358979",
               _gc(xasprintf("%.15g", atan2(-1., -INFINITY))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-1., __DBL_MIN__))));
  EXPECT_STREQ("-5.562684646268e-309",
               _gc(xasprintf("%.15g", atan2(-1., __DBL_MAX__))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(1.5, 0.))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(1.5, -0.))));
  EXPECT_STREQ("1.24904577239825", _gc(xasprintf("%.15g", atan2(1.5, .5))));
  EXPECT_STREQ("1.89254688119154", _gc(xasprintf("%.15g", atan2(1.5, -.5))));
  EXPECT_STREQ("0.982793723247329", _gc(xasprintf("%.15g", atan2(1.5, 1.))));
  EXPECT_STREQ("2.15879893034246", _gc(xasprintf("%.15g", atan2(1.5, -1.))));
  EXPECT_STREQ("0.785398163397448", _gc(xasprintf("%.15g", atan2(1.5, 1.5))));
  EXPECT_STREQ("2.35619449019234", _gc(xasprintf("%.15g", atan2(1.5, -1.5))));
  EXPECT_TRUE(isnan(atan2(1.5, NAN)));
  EXPECT_TRUE(isnan(atan2(1.5, -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(1.5, INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(1.5, -INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(1.5, __DBL_MIN__))));
  EXPECT_STREQ("8.34402696940201e-309",
               _gc(xasprintf("%.15g", atan2(1.5, __DBL_MAX__))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-1.5, 0.))));
  EXPECT_STREQ("-1.5707963267949", _gc(xasprintf("%.15g", atan2(-1.5, -0.))));
  EXPECT_STREQ("-1.24904577239825", _gc(xasprintf("%.15g", atan2(-1.5, .5))));
  EXPECT_STREQ("-1.89254688119154", _gc(xasprintf("%.15g", atan2(-1.5, -.5))));
  EXPECT_STREQ("-0.982793723247329", _gc(xasprintf("%.15g", atan2(-1.5, 1.))));
  EXPECT_STREQ("-2.15879893034246", _gc(xasprintf("%.15g", atan2(-1.5, -1.))));
  EXPECT_STREQ("-0.785398163397448", _gc(xasprintf("%.15g", atan2(-1.5, 1.5))));
  EXPECT_STREQ("-2.35619449019234", _gc(xasprintf("%.15g", atan2(-1.5, -1.5))));
  EXPECT_TRUE(isnan(atan2(-1.5, NAN)));
  EXPECT_TRUE(isnan(atan2(-1.5, -NAN)));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-1.5, INFINITY))));
  EXPECT_STREQ("-3.14159265358979",
               _gc(xasprintf("%.15g", atan2(-1.5, -INFINITY))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-1.5, __DBL_MIN__))));
  EXPECT_STREQ("-8.34402696940201e-309",
               _gc(xasprintf("%.15g", atan2(-1.5, __DBL_MAX__))));
  EXPECT_TRUE(isnan(atan2(NAN, 0.)));
  EXPECT_TRUE(isnan(atan2(NAN, -0.)));
  EXPECT_TRUE(isnan(atan2(NAN, .5)));
  EXPECT_TRUE(isnan(atan2(NAN, -.5)));
  EXPECT_TRUE(isnan(atan2(NAN, 1.)));
  EXPECT_TRUE(isnan(atan2(NAN, -1.)));
  EXPECT_TRUE(isnan(atan2(NAN, 1.5)));
  EXPECT_TRUE(isnan(atan2(NAN, -1.5)));
  EXPECT_TRUE(isnan(atan2(NAN, NAN)));
  EXPECT_TRUE(isnan(atan2(NAN, -NAN)));
  EXPECT_TRUE(isnan(atan2(NAN, INFINITY)));
  EXPECT_TRUE(isnan(atan2(NAN, -INFINITY)));
  EXPECT_TRUE(isnan(atan2(NAN, __DBL_MIN__)));
  EXPECT_TRUE(isnan(atan2(NAN, __DBL_MAX__)));
  EXPECT_TRUE(isnan(atan2(-NAN, 0.)));
  EXPECT_TRUE(isnan(atan2(-NAN, -0.)));
  EXPECT_TRUE(isnan(atan2(-NAN, .5)));
  EXPECT_TRUE(isnan(atan2(-NAN, -.5)));
  EXPECT_TRUE(isnan(atan2(-NAN, 1.)));
  EXPECT_TRUE(isnan(atan2(-NAN, -1.)));
  EXPECT_TRUE(isnan(atan2(-NAN, 1.5)));
  EXPECT_TRUE(isnan(atan2(-NAN, -1.5)));
  EXPECT_TRUE(isnan(atan2(-NAN, NAN)));
  EXPECT_TRUE(isnan(atan2(-NAN, -NAN)));
  EXPECT_TRUE(isnan(atan2(-NAN, INFINITY)));
  EXPECT_TRUE(isnan(atan2(-NAN, -INFINITY)));
  EXPECT_TRUE(isnan(atan2(-NAN, __DBL_MIN__)));
  EXPECT_TRUE(isnan(atan2(-NAN, __DBL_MAX__)));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(INFINITY, 0.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, -0.))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(INFINITY, .5))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, -.5))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", atan2(INFINITY, 1.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, -1.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, 1.5))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, -1.5))));
  EXPECT_TRUE(isnan(atan2(INFINITY, NAN)));
  EXPECT_TRUE(isnan(atan2(INFINITY, -NAN)));
  EXPECT_STREQ("0.785398163397448",
               _gc(xasprintf("%.15g", atan2(INFINITY, INFINITY))));
  EXPECT_STREQ("2.35619449019234",
               _gc(xasprintf("%.15g", atan2(INFINITY, -INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, __DBL_MIN__))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(INFINITY, __DBL_MAX__))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, 0.))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, -0.))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, .5))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, -.5))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, 1.))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, -1.))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, 1.5))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, -1.5))));
  EXPECT_TRUE(isnan(atan2(-INFINITY, NAN)));
  EXPECT_TRUE(isnan(atan2(-INFINITY, -NAN)));
  EXPECT_STREQ("-0.785398163397448",
               _gc(xasprintf("%.15g", atan2(-INFINITY, INFINITY))));
  EXPECT_STREQ("-2.35619449019234",
               _gc(xasprintf("%.15g", atan2(-INFINITY, -INFINITY))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, __DBL_MIN__))));
  EXPECT_STREQ("-1.5707963267949",
               _gc(xasprintf("%.15g", atan2(-INFINITY, __DBL_MAX__))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, 0.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, -0.))));
  EXPECT_STREQ("4.4501477170144e-308",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, .5))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, -.5))));
  EXPECT_STREQ("2.2250738585072e-308",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, 1.))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, -1.))));
  EXPECT_STREQ("1.48338257233813e-308",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, 1.5))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, -1.5))));
  EXPECT_TRUE(isnan(atan2(__DBL_MIN__, NAN)));
  EXPECT_TRUE(isnan(atan2(__DBL_MIN__, -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(__DBL_MIN__, INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, -INFINITY))));
  EXPECT_STREQ("0.785398163397448",
               _gc(xasprintf("%.15g", atan2(__DBL_MIN__, __DBL_MIN__))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(__DBL_MIN__, __DBL_MAX__))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, 0.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, -0.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, .5))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, -.5))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, 1.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, -1.))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, 1.5))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, -1.5))));
  EXPECT_TRUE(isnan(atan2(__DBL_MAX__, NAN)));
  EXPECT_TRUE(isnan(atan2(__DBL_MAX__, -NAN)));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", atan2(__DBL_MAX__, INFINITY))));
  EXPECT_STREQ("3.14159265358979",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, -INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, __DBL_MIN__))));
  EXPECT_STREQ("0.785398163397448",
               _gc(xasprintf("%.15g", atan2(__DBL_MAX__, __DBL_MAX__))));
  EXPECT_STREQ("-0",
               _gc(xasprintf("%.15g", atan2(-0.000000000000001, INFINITY))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", atan2(-1, INFINITY))));
  EXPECT_STREQ(
      "-0", _gc(xasprintf("%.15g", atan2(-1.7976931348623157e308, INFINITY))));
  EXPECT_STREQ("1.5707963267949",
               _gc(xasprintf("%.15g", atan2(1.4142135623731, 0))));
}

BENCH(atan2, bench) {
  volatile float f = 3;
  volatile double d = 3;
  volatile long double l = 3;
  EZBENCH2("atan2", donothing, atan2(d, d));    // ~31ns
  EZBENCH2("atan2f", donothing, atan2f(f, f));  // ~31ns
  EZBENCH2("atan2l", donothing, atan2l(l, l));  // ~31ns
}
