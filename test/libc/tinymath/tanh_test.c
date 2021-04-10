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
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define tanhl(x) tanhl(VEIL("t", (long double)(x)))
#define tanh(x)  tanh(VEIL("x", (double)(x)))
#define tanhf(x) tanhf(VEIL("x", (float)(x)))

TEST(tanhl, test) {
  EXPECT_STREQ(".09966799462495582", gc(xdtoal(tanhl(+.1))));
  EXPECT_STREQ("-.09966799462495582", gc(xdtoal(tanhl(-.1))));
  EXPECT_STREQ("0", gc(xdtoal(tanhl(0))));
  EXPECT_STREQ("-0", gc(xdtoal(tanhl(-0.))));
  EXPECT_TRUE(isnan(tanhl(NAN)));
  EXPECT_STREQ("1", gc(xdtoal(tanhl(INFINITY))));
  EXPECT_STREQ("-1", gc(xdtoal(tanhl(-INFINITY))));
}

TEST(tanh, test) {
  EXPECT_STREQ(".0996679946249559", gc(xdtoa(tanh(+.1))));
  EXPECT_STREQ("-.0996679946249559", gc(xdtoa(tanh(-.1))));
  EXPECT_STREQ("0", gc(xdtoa(tanh(0))));
  EXPECT_STREQ("-0", gc(xdtoa(tanh(-0.))));
  EXPECT_TRUE(isnan(tanh(NAN)));
  EXPECT_STREQ("1", gc(xdtoa(tanh(INFINITY))));
  EXPECT_STREQ("-1", gc(xdtoa(tanh(-INFINITY))));
}

TEST(tanhf, test) {
  EXPECT_STREQ(".099668", gc(xdtoaf(tanhf(+.1))));
  EXPECT_STREQ("-.099668", gc(xdtoaf(tanhf(-.1))));
  EXPECT_STREQ("0", gc(xdtoaf(tanhf(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(tanhf(-0.))));
  EXPECT_TRUE(isnan(tanhf(NAN)));
  EXPECT_STREQ("1", gc(xdtoaf(tanhf(INFINITY))));
  EXPECT_STREQ("-1", gc(xdtoaf(tanhf(-INFINITY))));
}
