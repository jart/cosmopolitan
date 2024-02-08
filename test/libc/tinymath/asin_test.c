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
#include "libc/stdio/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

double asin_(double) asm("asin");
#define asin asin_

TEST(asin, test) {
  EXPECT_STREQ("0", gc(xasprintf("%.15g", asin(0.))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", asin(-0.))));
  EXPECT_STREQ("0.523598775598299", gc(xasprintf("%.15g", asin(.5))));
  EXPECT_STREQ("-0.523598775598299", gc(xasprintf("%.15g", asin(-.5))));
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", asin(1.))));
  EXPECT_STREQ("-1.5707963267949", gc(xasprintf("%.15g", asin(-1.))));
  EXPECT_TRUE(isnan(asin(1.5)));
  EXPECT_TRUE(isnan(asin(-1.5)));
  EXPECT_TRUE(isnan(asin(NAN)));
  EXPECT_TRUE(isnan(asin(-NAN)));
  EXPECT_TRUE(isnan(asin(INFINITY)));
  EXPECT_TRUE(isnan(asin(-INFINITY)));
  EXPECT_STREQ("2.2250738585072e-308",
               gc(xasprintf("%.15g", asin(__DBL_MIN__))));
  EXPECT_TRUE(isnan(asin(__DBL_MAX__)));
}

BENCH(asinl, bench) {
  double _asin(double) asm("asin");
  float _asinf(float) asm("asinf");
  long double _asinl(long double) asm("asinl");
  EZBENCH2("-asin", donothing, _asin(.7));   /* ~16ns */
  EZBENCH2("-asinf", donothing, _asinf(.7)); /* ~12ns */
  EZBENCH2("-asinl", donothing, _asinl(.7)); /* ~39ns */
}
