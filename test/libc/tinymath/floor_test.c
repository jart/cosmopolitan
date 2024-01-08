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

TEST(floor, test) {
  EXPECT_STREQ("3", gc(xdtoa(floor(3))));
  EXPECT_STREQ("3", gc(xdtoa(floor(3.14))));
  EXPECT_STREQ("-4", gc(xdtoa(floor(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoa(floor(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(floor(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(floor(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(floor(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(floor(-INFINITY))));
}

TEST(floorf, test) {
  EXPECT_STREQ("3", gc(xdtoaf(floorf(3))));
  EXPECT_STREQ("3", gc(xdtoaf(floorf(3.14))));
  EXPECT_STREQ("-4", gc(xdtoaf(floorf(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoaf(floorf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(floorf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(floorf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(floorf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(floorf(-INFINITY))));
}

TEST(floorl, test) {
  EXPECT_STREQ("3", gc(xdtoal(floorl(3))));
  EXPECT_STREQ("3", gc(xdtoal(floorl(3.14))));
  EXPECT_STREQ("-4", gc(xdtoal(floorl(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoal(floorl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(floorl(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoal(floorl(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(floorl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(floorl(-INFINITY))));
}

BENCH(floorl, bench) {
  double _floor(double) asm("floor");
  float _floorf(float) asm("floorf");
  long double _floorl(long double) asm("floorl");
  EZBENCH2("floor", donothing, _floor(.7));   /* ~1ns */
  EZBENCH2("floorf", donothing, _floorf(.7)); /* ~3ns */
  EZBENCH2("floorl", donothing, _floorl(.7)); /* ~9ns */
}
