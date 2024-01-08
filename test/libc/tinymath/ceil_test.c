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

TEST(ceil, test) {
  EXPECT_STREQ("3", gc(xdtoa(ceil(3))));
  EXPECT_STREQ("4", gc(xdtoa(ceil(3.14))));
  EXPECT_STREQ("-3", gc(xdtoa(ceil(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoa(ceil(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(ceil(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(ceil(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(ceil(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(ceil(-INFINITY))));
}

TEST(ceilf, test) {
  EXPECT_STREQ("3", gc(xdtoaf(ceilf(3))));
  EXPECT_STREQ("4", gc(xdtoaf(ceilf(3.14))));
  EXPECT_STREQ("-3", gc(xdtoaf(ceilf(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoaf(ceilf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(ceilf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(ceilf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(ceilf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(ceilf(-INFINITY))));
}

TEST(ceill, test) {
  EXPECT_STREQ("3", gc(xdtoal(ceill(3))));
  EXPECT_STREQ("4", gc(xdtoal(ceill(3.14))));
  EXPECT_STREQ("-3", gc(xdtoal(ceill(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoal(ceill(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(ceill(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoal(ceill(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(ceill(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(ceill(-INFINITY))));
}

BENCH(ceill, bench) {
  double _ceil(double) asm("ceil");
  float _ceilf(float) asm("ceilf");
  long double _ceill(long double) asm("ceill");
  EZBENCH2("ceil", donothing, _ceil(.7));   /* ~1ns */
  EZBENCH2("ceilf", donothing, _ceilf(.7)); /* ~3ns */
  EZBENCH2("ceill", donothing, _ceill(.7)); /* ~9ns */
}
