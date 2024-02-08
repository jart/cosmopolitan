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
#include "libc/x/xasprintf.h"

TEST(atan, test) {
  EXPECT_STREQ("0", gc(xasprintf("%.15g", atan(0.))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", atan(-0.))));
  EXPECT_STREQ("0.463647609000806", gc(xasprintf("%.15g", atan(.5))));
  EXPECT_STREQ("-0.463647609000806", gc(xasprintf("%.15g", atan(-.5))));
  EXPECT_STREQ("0.785398163397448", gc(xasprintf("%.15g", atan(1.))));
  EXPECT_STREQ("-0.785398163397448", gc(xasprintf("%.15g", atan(-1.))));
  EXPECT_STREQ("0.982793723247329", gc(xasprintf("%.15g", atan(1.5))));
  EXPECT_STREQ("-0.982793723247329", gc(xasprintf("%.15g", atan(-1.5))));
  EXPECT_STREQ("nan", gc(xasprintf("%.15g", atan(NAN))));
  EXPECT_STREQ("-nan", gc(xasprintf("%.15g", atan(-NAN))));
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", atan(INFINITY))));
  EXPECT_STREQ("-1.5707963267949", gc(xasprintf("%.15g", atan(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               gc(xasprintf("%.15g", atan(__DBL_MIN__))));
  EXPECT_STREQ("1.5707963267949", gc(xasprintf("%.15g", atan(__DBL_MAX__))));
}

BENCH(atanl, bench) {
  double _atan(double) asm("atan");
  float _atanf(float) asm("atanf");
  long double _atanl(long double) asm("atanl");
  EZBENCH2("-atan", donothing, _atan(.7));   /* ~18ns */
  EZBENCH2("-atanf", donothing, _atanf(.7)); /* ~12ns */
  EZBENCH2("-atanl", donothing, _atanl(.7)); /* ~34ns */
}
