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

double _sinh(double) asm("sinh");
float _sinhf(float) asm("sinhf");
long double _sinhl(long double) asm("sinhl");

TEST(sinh, test) {
  EXPECT_STREQ(".521095305493747", gc(xdtoa(_sinh(+.5))));
  EXPECT_STREQ("-.521095305493747", gc(xdtoa(_sinh(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_sinh(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_sinh(-30000))));
  EXPECT_STREQ("0", gc(xdtoa(_sinh(0))));
  EXPECT_STREQ("-0", gc(xdtoa(_sinh(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(_sinh(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_sinh(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_sinh(-INFINITY))));
}

TEST(sinhl, test) {
  EXPECT_STREQ(".5210953054937474", gc(xdtoal(_sinhl(+.5))));
  EXPECT_STREQ("-.5210953054937474", gc(xdtoal(_sinhl(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_sinhl(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_sinhl(-30000))));
  EXPECT_STREQ("0", gc(xdtoal(_sinhl(0))));
  EXPECT_STREQ("-0", gc(xdtoal(_sinhl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(_sinhl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_sinhl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(_sinhl(-INFINITY))));
}

TEST(sinhf, test) {
  EXPECT_STREQ(".521095", gc(xdtoaf(_sinhf(+.5))));
  EXPECT_STREQ("-.521095", gc(xdtoaf(_sinhf(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_sinhf(30000))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_sinhf(-30000))));
  EXPECT_STREQ("0", gc(xdtoaf(_sinhf(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(_sinhf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_sinhf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_sinhf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(_sinhf(-INFINITY))));
}

BENCH(sinh, bench) {
  EZBENCH2("sinh", donothing, _sinh(.7));    // ~24ns
  EZBENCH2("sinhf", donothing, _sinhf(.7));  // ~19ns
  EZBENCH2("sinhl", donothing, _sinhl(.7));  // ~15ns
}
