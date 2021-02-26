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
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

int rando;

void SetUp(void) {
  rando = rand() & 0xffff;
}

TEST(ldexpl, test) {
  EXPECT_EQ(rando, ldexpl(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoal(ldexpl(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoal(ldexpl(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(ldexpl(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(ldexpl(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoal(ldexpl(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoal(ldexpl(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(ldexpl(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(ldexpl(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoal(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoal(ldexpl(1, -8))));
  EXPECT_STREQ("0", gc(xdtoal(ldexpl(0, -8))));
  EXPECT_STREQ("0", gc(xdtoal(ldexpl(0, 8))));
  EXPECT_STREQ("256", gc(xdtoal(ldexpl(1, 8))));
  EXPECT_STREQ("512", gc(xdtoal(ldexpl(2, 8))));
  EXPECT_STREQ("768", gc(xdtoal(ldexpl(3, 8))));
  EXPECT_STREQ("6.997616471358197e+3461", gc(xdtoal(ldexpl(1, 11500))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(ldexpl(1, 999999))));
  EXPECT_STREQ("0", gc(xdtoal(ldexpl(1, -999999))));
}

TEST(ldexp, test) {
  EXPECT_EQ(rando, ldexp(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoa(ldexp(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoa(ldexp(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(ldexp(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(ldexp(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoa(ldexp(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoa(ldexp(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(ldexp(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(ldexp(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoa(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoa(ldexp(1, -8))));
  EXPECT_STREQ("0", gc(xdtoa(ldexp(0, -8))));
  EXPECT_STREQ("0", gc(xdtoa(ldexp(0, 8))));
  EXPECT_STREQ("256", gc(xdtoa(ldexp(1, 8))));
  EXPECT_STREQ("512", gc(xdtoa(ldexp(2, 8))));
  EXPECT_STREQ("768", gc(xdtoa(ldexp(3, 8))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(ldexp(1, 999999))));
  EXPECT_STREQ("0", gc(xdtoa(ldexp(1, -999999))));
}

TEST(ldexpf, test) {
  EXPECT_EQ(rando, ldexpf(rando, 0));
  EXPECT_STREQ("NAN", gc(xdtoaf(ldexpf(NAN, 0))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(ldexpf(-NAN, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(ldexpf(INFINITY, 0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(ldexpf(-INFINITY, 0))));
  EXPECT_STREQ("NAN", gc(xdtoaf(ldexpf(NAN, 1))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(ldexpf(-NAN, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(ldexpf(INFINITY, 1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(ldexpf(-INFINITY, 1))));
  EXPECT_STREQ("16384", gc(xdtoaf(log2l(LDBL_MAX))));
  EXPECT_STREQ(".00390625", gc(xdtoaf(ldexpf(1, -8))));
  EXPECT_STREQ("0", gc(xdtoaf(ldexpf(0, -8))));
  EXPECT_STREQ("0", gc(xdtoaf(ldexpf(0, 8))));
  EXPECT_STREQ("256", gc(xdtoaf(ldexpf(1, 8))));
  EXPECT_STREQ("512", gc(xdtoaf(ldexpf(2, 8))));
  EXPECT_STREQ("768", gc(xdtoaf(ldexpf(3, 8))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(ldexpf(1, 999999))));
  EXPECT_STREQ("0", gc(xdtoaf(ldexpf(1, -999999))));
}

TEST(ldexp, stuff) {
  volatile int twopow = 5;
  volatile double pi = 3.14;
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", ldexp(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", ldexpf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", ldexpl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalb(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalbl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbn(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbnf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalbnl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbln(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalblnf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalblnl(pi, twopow))));
}

TEST(exp10, test) {
  ASSERT_EQ(100, (int)exp10(2));
  ASSERT_STREQ("100.000000", gc(xasprintf("%Lf", exp10l(2))));
}
