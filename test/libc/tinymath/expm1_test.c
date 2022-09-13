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
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

#define expm1l(x) expm1l(VEIL("t", (long double)(x)))
#define expm1(x)  expm1(VEIL("x", (double)(x)))
#define expm1f(x) expm1f(VEIL("x", (float)(x)))

TEST(expm1, test) {
  EXPECT_STREQ("0", _gc(xdtoa(expm1(0))));
  EXPECT_STREQ("-0", _gc(xdtoa(expm1(-0.))));
  EXPECT_STREQ("NAN", _gc(xdtoa(expm1(NAN))));
  EXPECT_STREQ("-1", _gc(xdtoa(expm1(-INFINITY))));
  EXPECT_STREQ("INFINITY", _gc(xdtoa(expm1(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", _gc(xdtoa(expm1(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", _gc(xdtoa(expm1(132098844872390)))); */
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", expm1(0.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", expm1(-0.))));
  EXPECT_STREQ("0.648721270700128", _gc(xasprintf("%.15g", expm1(.5))));
  EXPECT_STREQ("-0.393469340287367", _gc(xasprintf("%.15g", expm1(-.5))));
  EXPECT_STREQ("1.71828182845905", _gc(xasprintf("%.15g", expm1(1.))));
  EXPECT_STREQ("-0.632120558828558", _gc(xasprintf("%.15g", expm1(-1.))));
  EXPECT_STREQ("3.48168907033806", _gc(xasprintf("%.15g", expm1(1.5))));
  EXPECT_STREQ("-0.77686983985157", _gc(xasprintf("%.15g", expm1(-1.5))));
  EXPECT_STREQ("6.38905609893065", _gc(xasprintf("%.15g", expm1(2.))));
  EXPECT_TRUE(isnan(expm1(NAN)));
  EXPECT_TRUE(isnan(expm1(-NAN)));
  EXPECT_STREQ("inf", _gc(xasprintf("%.15g", expm1(INFINITY))));
  EXPECT_STREQ("-1", _gc(xasprintf("%.15g", expm1(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               _gc(xasprintf("%.15g", expm1(__DBL_MIN__))));
  EXPECT_STREQ("inf", _gc(xasprintf("%.15g", expm1(__DBL_MAX__))));
}

TEST(expm1l, test) {
  EXPECT_STREQ("1.718281828459045", _gc(xdtoal(expm1l(1))));
  EXPECT_STREQ("1.718281828459045", _gc(xdtoal(expl(1) - 1)));
  EXPECT_STREQ("0", _gc(xdtoal(expm1l(0))));
  EXPECT_STREQ("-0", _gc(xdtoal(expm1l(-0.))));
  EXPECT_STREQ("NAN", _gc(xdtoal(expm1l(NAN))));
  EXPECT_STREQ("-1", _gc(xdtoal(expm1l(-INFINITY))));
  EXPECT_STREQ("INFINITY", _gc(xdtoal(expm1l(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", _gc(xdtoal(expm1l(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", _gc(xdtoal(expm1l(132098844872390)))); */
}

TEST(expm1f, test) {
  EXPECT_STREQ("0", _gc(xdtoaf(expm1f(0))));
  EXPECT_STREQ("-0", _gc(xdtoaf(expm1f(-0.))));
  EXPECT_STREQ("NAN", _gc(xdtoaf(expm1f(NAN))));
  EXPECT_STREQ("-1", _gc(xdtoaf(expm1f(-INFINITY))));
  EXPECT_STREQ("INFINITY", _gc(xdtoaf(expm1f(INFINITY))));
  /* EXPECT_STREQ("-INFINITY", _gc(xdtoaf(expm1f(-132098844872390)))); */
  /* EXPECT_STREQ("INFINITY", _gc(xdtoaf(expm1f(132098844872390)))); */
}
