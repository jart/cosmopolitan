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

double _sqrt(double) asm("sqrt");
float _sqrtf(float) asm("sqrtf");
long double _sqrtl(long double) asm("sqrtl");

TEST(sqrtl, test) {
  EXPECT_STREQ("7", gc(xdtoal(_sqrtl(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoal(_sqrtl(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(_sqrtl(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_sqrtl(INFINITY))));
  EXPECT_TRUE(isnan(_sqrtl(-1)));
}

TEST(sqrt, test) {
  EXPECT_STREQ("7", gc(xdtoa(_sqrt(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoa(_sqrt(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(_sqrt(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_sqrt(INFINITY))));
  EXPECT_TRUE(isnan(_sqrt(-1)));
}

TEST(sqrtf, test) {
  EXPECT_STREQ("7", gc(xdtoaf(_sqrtf(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_sqrtf(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(_sqrtf(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_sqrtf(INFINITY))));
  EXPECT_TRUE(isnan(_sqrtf(-1)));
}

BENCH(_sqrt, bench) {
  EZBENCH2("sqrt", donothing, _sqrt(.7));   /* ~2ns */
  EZBENCH2("sqrtf", donothing, _sqrtf(.7)); /* ~1ns */
  EZBENCH2("sqrtl", donothing, _sqrtl(.7)); /* ~9ns */
}
