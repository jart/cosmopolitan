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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _fabs(double) asm("fabs");
float _fabsf(float) asm("fabsf");
long double _fabsl(long double) asm("fabsl");

TEST(fabsl, test) {
  EXPECT_STREQ("0", gc(xdtoal(_fabsl(-0.))));
  EXPECT_STREQ("0", gc(xdtoal(_fabsl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(_fabsl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_fabsl(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_fabsl(-INFINITY))));
}

TEST(fabs, test) {
  EXPECT_STREQ("0", gc(xdtoa(_fabs(-0.))));
  EXPECT_STREQ("0", gc(xdtoa(_fabs(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(_fabs(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_fabs(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_fabs(-INFINITY))));
}

TEST(fabsf, test) {
  EXPECT_STREQ("0", gc(xdtoaf(_fabsf(-0.))));
  EXPECT_STREQ("0", gc(xdtoaf(_fabsf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_fabsf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_fabsf(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_fabsf(-INFINITY))));
}

TEST(fabs, stuff) {
  EXPECT_LDBL_EQ(3.14, _fabs(3.14));
  EXPECT_LDBL_EQ(3.14, _fabs(-3.14));
  EXPECT_EQ(1, !!isnan(_fabs(NAN)));
  EXPECT_EQ(1, !!isnan(_fabs(-NAN)));
  EXPECT_EQ(0, !!signbit(_fabs(-NAN)));
}

BENCH(fabs, bench) {
  EZBENCH2("fabs", donothing, _fabs(.7));   /*  ~6ns */
  EZBENCH2("fabsf", donothing, _fabsf(.7)); /*  ~5ns */
  EZBENCH2("fabsl", donothing, _fabsl(.7)); /* ~28ns */
}
