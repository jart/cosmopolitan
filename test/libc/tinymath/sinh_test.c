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
#include "libc/mem/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define sinhl(x) sinhl(VEIL("t", (long double)(x)))
#define sinh(x)  sinh(VEIL("x", (double)(x)))
#define sinhf(x) sinhf(VEIL("x", (float)(x)))

TEST(sinh, test) {
  EXPECT_STREQ(".521095305493747", gc(xdtoa(sinh(+.5))));
  EXPECT_STREQ("-.521095305493747", gc(xdtoa(sinh(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(sinh(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(sinh(-30000))));
  EXPECT_STREQ("0", gc(xdtoa(sinh(0))));
  EXPECT_STREQ("-0", gc(xdtoa(sinh(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(sinh(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(sinh(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(sinh(-INFINITY))));
}

TEST(sinhl, test) {
  EXPECT_STREQ(".5210953054937474", gc(xdtoal(sinhl(+.5))));
  EXPECT_STREQ("-.5210953054937474", gc(xdtoal(sinhl(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(sinhl(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(sinhl(-30000))));
  EXPECT_STREQ("0", gc(xdtoal(sinhl(0))));
  EXPECT_STREQ("-0", gc(xdtoal(sinhl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(sinhl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(sinhl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(sinhl(-INFINITY))));
}

TEST(sinhf, test) {
  EXPECT_STREQ(".521095", gc(xdtoaf(sinhf(+.5))));
  EXPECT_STREQ("-.521095", gc(xdtoaf(sinhf(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(sinhf(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(sinhf(-30000))));
  EXPECT_STREQ("0", gc(xdtoaf(sinhf(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(sinhf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(sinhf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(sinhf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(sinhf(-INFINITY))));
}
