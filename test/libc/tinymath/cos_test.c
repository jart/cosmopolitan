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

double _cos(double) asm("cos");
float _cosf(float) asm("cosf");
long double _cosl(long double) asm("cosl");

TEST(cos, test) {
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _cos(0.))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _cos(-0.))));
  EXPECT_STREQ("0.995004165278026", gc(xasprintf("%.15g", _cos(.1))));
  EXPECT_STREQ("0.995004165278026", gc(xasprintf("%.15g", _cos(-.1))));
  EXPECT_STREQ("0.877582561890373", gc(xasprintf("%.15g", _cos(.5))));
  EXPECT_STREQ("0.877582561890373", gc(xasprintf("%.15g", _cos(-.5))));
  EXPECT_STREQ("0.54030230586814", gc(xasprintf("%.15g", _cos(1.))));
  EXPECT_STREQ("0.54030230586814", gc(xasprintf("%.15g", _cos(-1.))));
  EXPECT_STREQ("0.0707372016677029", gc(xasprintf("%.15g", _cos(1.5))));
  EXPECT_STREQ("0.0707372016677029", gc(xasprintf("%.15g", _cos(-1.5))));
  EXPECT_STREQ("-0.416146836547142", gc(xasprintf("%.15g", _cos(2.))));
  EXPECT_TRUE(isnan(_cos(NAN)));
  EXPECT_TRUE(isnan(_cos(-NAN)));
  EXPECT_TRUE(isnan(_cos(INFINITY)));
  EXPECT_TRUE(isnan(_cos(-INFINITY)));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _cos(__DBL_MIN__))));
  EXPECT_STREQ("-0.99998768942656", gc(xasprintf("%.15g", _cos(__DBL_MAX__))));
  EXPECT_STREQ("0.54030230586814",
               gc(xasprintf("%.15g", _cos(-1.0000000000000002))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _cos(-2.1073424255447e-08))));
}

BENCH(cos, bench) {
  EZBENCH2("cos", donothing, _cos(.7));    //  ~6ns
  EZBENCH2("cosf", donothing, _cosf(.7));  //  ~5ns
  EZBENCH2("cosl", donothing, _cosl(.7));  // ~25ns
}
