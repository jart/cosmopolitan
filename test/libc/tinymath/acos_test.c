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
#include "libc/stdio/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

double acos_(double) asm("acos");
#define acos acos_

TEST(acos, test) {
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", acos(0.))));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", acos(-0.))));
  EXPECT_STREQ("1.0471975511966", _gc(xasprintf("%.15g", acos(.5))));
  EXPECT_STREQ("2.0943951023932", _gc(xasprintf("%.15g", acos(-.5))));
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", acos(1.))));
  EXPECT_STREQ("3.14159265358979", _gc(xasprintf("%.15g", acos(-1.))));
  EXPECT_TRUE(isnan(acos(1.5)));
  EXPECT_TRUE(isnan(acos(-1.5)));
  EXPECT_TRUE(isnan(acos(2.)));
  EXPECT_TRUE(isnan(acos(NAN)));
  EXPECT_TRUE(isnan(acos(-NAN)));
  EXPECT_TRUE(isnan(acos(INFINITY)));
  EXPECT_TRUE(isnan(acos(-INFINITY)));
  EXPECT_STREQ("1.5707963267949", _gc(xasprintf("%.15g", acos(__DBL_MIN__))));
  EXPECT_TRUE(isnan(acos(__DBL_MAX__)));
}

BENCH(acosl, bench) {
  double _acos(double) asm("acos");
  float _acosf(float) asm("acosf");
  long double _acosl(long double) asm("acosl");
  EZBENCH2("-acos", donothing, _acos(.7));   /* ~17ns */
  EZBENCH2("-acosf", donothing, _acosf(.7)); /* ~11ns */
  EZBENCH2("-acosl", donothing, _acosl(.7)); /* ~40ns */
}
