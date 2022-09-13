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

#define acoshl(x) acoshl(VEIL("t", (long double)(x)))
#define acosh(x)  acosh(VEIL("x", (double)(x)))
#define acoshf(x) acoshf(VEIL("x", (float)(x)))

TEST(acosh, test) {
  EXPECT_STREQ(".962423650119207", gc(xdtoa(acosh(1.5))));
  EXPECT_STREQ("0", gc(xdtoa(acosh(1))));
  EXPECT_TRUE(isnan(acosh(NAN)));
  EXPECT_TRUE(isnan(acosh(.5)));
  EXPECT_TRUE(isnan(acosh(-.5)));
  EXPECT_TRUE(isnan(acosh(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoa(acosh(INFINITY))));
}

TEST(acoshf, test) {
  EXPECT_STREQ(".962424", gc(xdtoaf(acoshf(1.5))));
  EXPECT_STREQ("0", gc(xdtoaf(acoshf(1))));
  EXPECT_TRUE(isnan(acoshf(NAN)));
  EXPECT_TRUE(isnan(acoshf(.5)));
  EXPECT_TRUE(isnan(acoshf(-.5)));
  EXPECT_TRUE(isnan(acoshf(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(acoshf(INFINITY))));
}

TEST(acoshl, test) {
  EXPECT_STREQ(".9624236501192069", gc(xdtoal(acoshl(1.5))));
  EXPECT_STREQ("0", gc(xdtoal(acoshl(1))));
  EXPECT_TRUE(isnan(acoshl(NAN)));
  EXPECT_TRUE(isnan(acoshl(.5)));
  EXPECT_TRUE(isnan(acoshl(-.5)));
  EXPECT_TRUE(isnan(acoshl(-1.5)));
  EXPECT_STREQ("INFINITY", gc(xdtoal(acoshl(INFINITY))));
}
