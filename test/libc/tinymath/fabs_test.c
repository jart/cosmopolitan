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
#include "libc/mem/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define fabsl(x) fabsl(VEIL("t", (long double)(x)))
#define fabs(x)  fabs(VEIL("x", (double)(x)))
#define fabsf(x) fabsf(VEIL("x", (float)(x)))

TEST(fabsl, test) {
  EXPECT_STREQ("0", gc(xdtoal(fabsl(-0.))));
  EXPECT_STREQ("0", gc(xdtoal(fabsl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(fabsl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(fabsl(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(fabsl(-INFINITY))));
}

TEST(fabs, test) {
  EXPECT_STREQ("0", gc(xdtoa(fabs(-0.))));
  EXPECT_STREQ("0", gc(xdtoa(fabs(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(fabs(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(fabs(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(fabs(-INFINITY))));
}

TEST(fabsf, test) {
  EXPECT_STREQ("0", gc(xdtoaf(fabsf(-0.))));
  EXPECT_STREQ("0", gc(xdtoaf(fabsf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(fabsf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(fabsf(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(fabsf(-INFINITY))));
}

TEST(fabs, stuff) {
  EXPECT_LDBL_EQ(3.14, fabs(3.14));
  EXPECT_LDBL_EQ(3.14, fabs(-3.14));
  EXPECT_EQ(1, !!isnan(fabs(NAN)));
  EXPECT_EQ(1, !!isnan(fabs(-NAN)));
  EXPECT_EQ(0, !!signbit(fabs(-NAN)));
}
