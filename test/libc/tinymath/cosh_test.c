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

#define coshl(x) coshl(VEIL("t", (long double)(x)))
#define cosh(x)  cosh(VEIL("x", (double)(x)))
#define coshf(x) coshf(VEIL("x", (float)(x)))

TEST(coshl, test) {
  EXPECT_STREQ("1.127625965206381", gc(xdtoal(coshl(+.5))));
  EXPECT_STREQ("1.127625965206381", gc(xdtoal(coshl(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(coshl(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(coshl(-30000))));
  EXPECT_STREQ("1", gc(xdtoal(coshl(+0.))));
  EXPECT_STREQ("1", gc(xdtoal(coshl(-0.))));
  EXPECT_TRUE(isnan(coshl(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoal(coshl(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(coshl(-INFINITY))));
}

TEST(cosh, test) {
  EXPECT_STREQ("1.12762596520638", gc(xdtoa(cosh(+.5))));
  EXPECT_STREQ("1.12762596520638", gc(xdtoa(cosh(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(cosh(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(cosh(-30000))));
  EXPECT_STREQ("1", gc(xdtoa(cosh(+0.))));
  EXPECT_STREQ("1", gc(xdtoa(cosh(-0.))));
  EXPECT_TRUE(isnan(cosh(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoa(cosh(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(cosh(-INFINITY))));
}

TEST(coshf, test) {
  EXPECT_STREQ("1.12763", gc(xdtoaf(coshf(+.5))));
  EXPECT_STREQ("1.12763", gc(xdtoaf(coshf(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(coshf(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(coshf(-30000))));
  EXPECT_STREQ("1", gc(xdtoaf(coshf(+0.))));
  EXPECT_STREQ("1", gc(xdtoaf(coshf(-0.))));
  EXPECT_TRUE(isnan(coshf(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(coshf(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(coshf(-INFINITY))));
}
