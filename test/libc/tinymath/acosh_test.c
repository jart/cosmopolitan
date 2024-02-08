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
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _acosh(double) asm("acosh");
float _acoshf(float) asm("acoshf");
long double _acoshl(long double) asm("acoshl");

TEST(acosh, test) {
  EXPECT_STREQ(".962423650119207", gc(xdtoa(_acosh(1.5))));
  EXPECT_STREQ("0", gc(xdtoa(_acosh(1))));
  EXPECT_TRUE(isnan(_acosh(NAN)));
  EXPECT_TRUE(isnan(_acosh(.5)));
  EXPECT_TRUE(isnan(_acosh(-.5)));
  EXPECT_TRUE(isnan(_acosh(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_acosh(INFINITY))));
}

TEST(acoshf, test) {
  EXPECT_STREQ(".962424", gc(xdtoaf(_acoshf(1.5))));
  EXPECT_STREQ("0", gc(xdtoaf(_acoshf(1))));
  EXPECT_TRUE(isnan(_acoshf(NAN)));
  EXPECT_TRUE(isnan(_acoshf(.5)));
  EXPECT_TRUE(isnan(_acoshf(-.5)));
  EXPECT_TRUE(isnan(_acoshf(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_acoshf(INFINITY))));
}

TEST(acoshl, test) {
  volatile long double x = 16;
  EXPECT_STREQ("4", gc(xdtoal(sqrtl(x))));
  EXPECT_STREQ(".9624236501192069", gc(xdtoal(_acoshl(1.5))));
  EXPECT_STREQ("0", gc(xdtoal(_acoshl(1))));
  EXPECT_TRUE(isnan(_acoshl(NAN)));
  EXPECT_TRUE(isnan(_acoshl(.5)));
  EXPECT_TRUE(isnan(_acoshl(-.5)));
  EXPECT_TRUE(isnan(_acoshl(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_acoshl(INFINITY))));
}
