/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

double _ldexp(double, int) asm("ldexp");
float _ldexpf(float, int) asm("ldexpf");
long double _ldexpl(long double, int) asm("ldexpl");

int rando;

void SetUp(void) {
  rando = rand() & 0xffff;
}

TEST(ldexpl, test) {
  EXPECT_EQ(rando, _ldexpl(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoal(_ldexpl(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoal(_ldexpl(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_ldexpl(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_ldexpl(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoal(_ldexpl(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoal(_ldexpl(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_ldexpl(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_ldexpl(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoal(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoal(_ldexpl(1, -8))));
  EXPECT_STREQ("0", gc(xdtoal(_ldexpl(0, -8))));
  EXPECT_STREQ("0", gc(xdtoal(_ldexpl(0, 8))));
  EXPECT_STREQ("256", gc(xdtoal(_ldexpl(1, 8))));
  EXPECT_STREQ("512", gc(xdtoal(_ldexpl(2, 8))));
  EXPECT_STREQ("768", gc(xdtoal(_ldexpl(3, 8))));
  EXPECT_STREQ("6.997616471358197e+3461", gc(xdtoal(_ldexpl(1, 11500))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_ldexpl(1, 999999))));
  // EXPECT_STREQ("0", gc(xdtoal(_ldexpl(1, -999999))));
}

TEST(ldexp, test) {
  EXPECT_EQ(rando, _ldexp(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoa(_ldexp(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoa(_ldexp(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_ldexp(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_ldexp(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_ldexp(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoa(_ldexp(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_ldexp(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_ldexp(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoa(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoa(_ldexp(1, -8))));
  EXPECT_STREQ("0", gc(xdtoa(_ldexp(0, -8))));
  EXPECT_STREQ("0", gc(xdtoa(_ldexp(0, 8))));
  EXPECT_STREQ("256", gc(xdtoa(_ldexp(1, 8))));
  EXPECT_STREQ("512", gc(xdtoa(_ldexp(2, 8))));
  EXPECT_STREQ("768", gc(xdtoa(_ldexp(3, 8))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_ldexp(1, 999999))));
  EXPECT_STREQ("0", gc(xdtoa(_ldexp(1, -999999))));
}

TEST(ldexpf, test) {
  EXPECT_EQ(rando, _ldexpf(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoaf(_ldexpf(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(_ldexpf(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_ldexpf(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_ldexpf(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_ldexpf(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(_ldexpf(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_ldexpf(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_ldexpf(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoaf(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoaf(_ldexpf(1, -8))));
  EXPECT_STREQ("0", gc(xdtoaf(_ldexpf(0, -8))));
  EXPECT_STREQ("0", gc(xdtoaf(_ldexpf(0, 8))));
  EXPECT_STREQ("256", gc(xdtoaf(_ldexpf(1, 8))));
  EXPECT_STREQ("512", gc(xdtoaf(_ldexpf(2, 8))));
  EXPECT_STREQ("768", gc(xdtoaf(_ldexpf(3, 8))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_ldexpf(1, 999999))));
  EXPECT_STREQ("0", gc(xdtoaf(_ldexpf(1, -999999))));
}

TEST(ldexp, stuff) {
  volatile int twopow = 5;
  volatile double pi = 3.14;
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", _ldexp(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", _ldexpf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalb(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbn(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbnf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbln(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalblnf(pi, twopow))));
#ifndef __aarch64__
  // TODO: implement quad floating point in printf
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", _ldexpl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalbnl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalblnl(pi, twopow))));
#endif
}

BENCH(ldexpl, bench) {
  EZBENCH2("ldexp", donothing, _ldexp(.7, 3));    // ~2ns
  EZBENCH2("ldexpf", donothing, _ldexpf(.7, 3));  // ~2ns
  EZBENCH2("ldexpl", donothing, _ldexpl(.7, 3));  // ~8ns
}
