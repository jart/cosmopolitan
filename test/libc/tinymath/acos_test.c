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

double _acos(double) asm("acos");
float _acosf(float) asm("acosf");
long double _acosl(long double) asm("acosl");

TEST(acos, test) {
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", _acos(0.))));
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", _acos(-0.))));
  EXPECT_STREQ("1.0471975511966", gc(xasprintf("%.15g", _acos(.5))));
  EXPECT_STREQ("2.0943951023932", gc(xasprintf("%.15g", _acos(-.5))));
  EXPECT_STREQ("0", gc(xasprintf("%.15g", _acos(1.))));
  EXPECT_STREQ("3.14159265358979", gc(xasprintf("%.15g", _acos(-1.))));
  EXPECT_TRUE(isnan(_acos(1.5)));
  EXPECT_TRUE(isnan(_acos(-1.5)));
  EXPECT_TRUE(isnan(_acos(2.)));
  EXPECT_TRUE(isnan(_acos(NAN)));
  EXPECT_TRUE(isnan(_acos(-NAN)));
  EXPECT_TRUE(isnan(_acos(INFINITY)));
  EXPECT_TRUE(isnan(_acos(-INFINITY)));
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", _acos(__DBL_MIN__))));
  EXPECT_TRUE(isnan(_acos(__DBL_MAX__)));
}

BENCH(acos, bench) {
  EZBENCH2("acos", donothing, _acos(.7));   /* ~13ns */
  EZBENCH2("acosf", donothing, _acosf(.7)); /* ~10ns */
  EZBENCH2("acosl", donothing, _acosl(.7)); /* ~26ns */
}
