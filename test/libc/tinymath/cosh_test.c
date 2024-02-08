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

double _cosh(double) asm("cosh");
float _coshf(float) asm("coshf");
long double _coshl(long double) asm("coshl");

TEST(coshl, test) {
  EXPECT_STREQ("1.127625965206381", gc(xdtoal(_coshl(+.5))));
  EXPECT_STREQ("1.127625965206381", gc(xdtoal(_coshl(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_coshl(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_coshl(-30000))));
  EXPECT_STREQ("1", gc(xdtoal(_coshl(+0.))));
  EXPECT_STREQ("1", gc(xdtoal(_coshl(-0.))));
  EXPECT_TRUE(isnan(_coshl(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_coshl(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_coshl(-INFINITY))));
}

TEST(cosh, test) {
  EXPECT_STREQ("1.12762596520638", gc(xdtoa(_cosh(+.5))));
  EXPECT_STREQ("1.12762596520638", gc(xdtoa(_cosh(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_cosh(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_cosh(-30000))));
  EXPECT_STREQ("1", gc(xdtoa(_cosh(+0.))));
  EXPECT_STREQ("1", gc(xdtoa(_cosh(-0.))));
  EXPECT_TRUE(isnan(_cosh(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_cosh(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_cosh(-INFINITY))));
}

TEST(coshf, test) {
  EXPECT_STREQ("1.12763", gc(xdtoaf(_coshf(+.5))));
  EXPECT_STREQ("1.12763", gc(xdtoaf(_coshf(-.5))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_coshf(30000))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_coshf(-30000))));
  EXPECT_STREQ("1", gc(xdtoaf(_coshf(+0.))));
  EXPECT_STREQ("1", gc(xdtoaf(_coshf(-0.))));
  EXPECT_TRUE(isnan(_coshf(NAN)));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_coshf(INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_coshf(-INFINITY))));
}

BENCH(cosh, bench) {
  EZBENCH2("cosh", donothing, _cosh(.7));    //  ~6ns
  EZBENCH2("coshf", donothing, _coshf(.7));  //  ~5ns
  EZBENCH2("coshl", donothing, _coshl(.7));  // ~25ns
}
