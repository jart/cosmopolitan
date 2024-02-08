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

double _expm1(double) asm("expm1");
float _expm1f(float) asm("expm1f");
long double _expm1l(long double) asm("expm1l");

TEST(expm1, test) {
  EXPECT_STREQ("0", gc(xdtoa(_expm1(0))));
  EXPECT_STREQ("-0", gc(xdtoa(_expm1(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(_expm1(NAN))));
  EXPECT_STREQ("-1", gc(xdtoa(_expm1(-INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_expm1(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", gc(xdtoa(_expm1(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", gc(xdtoa(_expm1(132098844872390)))); */
  EXPECT_STREQ("0", gc(xasprintf("%.15g", _expm1(0.))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", _expm1(-0.))));
  EXPECT_STREQ("0.648721270700128", gc(xasprintf("%.15g", _expm1(.5))));
  EXPECT_STREQ("-0.393469340287367", gc(xasprintf("%.15g", _expm1(-.5))));
  EXPECT_STREQ("1.71828182845905", gc(xasprintf("%.15g", _expm1(1.))));
  EXPECT_STREQ("-0.632120558828558", gc(xasprintf("%.15g", _expm1(-1.))));
  EXPECT_STREQ("3.48168907033806", gc(xasprintf("%.15g", _expm1(1.5))));
  EXPECT_STREQ("-0.77686983985157", gc(xasprintf("%.15g", _expm1(-1.5))));
  EXPECT_STREQ("6.38905609893065", gc(xasprintf("%.15g", _expm1(2.))));
  EXPECT_TRUE(isnan(_expm1(NAN)));
  EXPECT_TRUE(isnan(_expm1(-NAN)));
  EXPECT_STREQ("inf", gc(xasprintf("%.15g", _expm1(INFINITY))));
  EXPECT_STREQ("-1", gc(xasprintf("%.15g", _expm1(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               gc(xasprintf("%.15g", _expm1(__DBL_MIN__))));
  EXPECT_STREQ("inf", gc(xasprintf("%.15g", _expm1(__DBL_MAX__))));
}

TEST(expm1l, test) {
  EXPECT_STREQ("1.718281828459045", gc(xdtoal(_expm1l(1))));
  EXPECT_STREQ("1.718281828459045", gc(xdtoal(expl(1) - 1)));
  EXPECT_STREQ("0", gc(xdtoal(_expm1l(0))));
  EXPECT_STREQ("-0", gc(xdtoal(_expm1l(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(_expm1l(NAN))));
  EXPECT_STREQ("-1", gc(xdtoal(_expm1l(-INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_expm1l(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", gc(xdtoal(_expm1l(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", gc(xdtoal(_expm1l(132098844872390)))); */
}

TEST(expm1f, test) {
  EXPECT_STREQ("0", gc(xdtoaf(_expm1f(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(_expm1f(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_expm1f(NAN))));
  EXPECT_STREQ("-1", gc(xdtoaf(_expm1f(-INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_expm1f(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", gc(xdtoaf(_expm1f(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", gc(xdtoaf(_expm1f(132098844872390)))); */
}

BENCH(expm1, bench) {
  EZBENCH2("expm1", donothing, _expm1(.7));    // ~17ns
  EZBENCH2("expm1f", donothing, _expm1f(.7));  // ~13ns
  EZBENCH2("expm1l", donothing, _expm1l(.7));  // ~33ns
}
