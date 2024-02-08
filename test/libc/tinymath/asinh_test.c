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
#include "libc/x/xasprintf.h"

double _asinh(double) asm("asinh");
float _asinhf(float) asm("asinhf");
long double _asinhl(long double) asm("asinhl");

TEST(asinh, test) {
  EXPECT_STREQ(".481211825059603", gc(xdtoa(_asinh(+.5))));
  EXPECT_STREQ("-.481211825059603", gc(xdtoa(_asinh(-.5))));
  EXPECT_STREQ("0", gc(xdtoa(_asinh(0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_asinh(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_asinh(INFINITY))));
  EXPECT_STREQ("-2.1073424255447e-08",
               gc(xasprintf("%.15g", _asinh(-2.1073424255447e-08))));
}

TEST(asinhf, test) {
  EXPECT_STREQ(".481212", gc(xdtoaf(_asinhf(+.5))));
  EXPECT_STREQ("-.481212", gc(xdtoaf(_asinhf(-.5))));
  EXPECT_STREQ("0", gc(xdtoaf(_asinhf(0))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_asinhf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_asinhf(INFINITY))));
}

TEST(asinhl, test) {
  EXPECT_STREQ(".4812118250596034", gc(xdtoal(_asinhl(+.5))));
  EXPECT_STREQ("-.4812118250596034", gc(xdtoal(_asinhl(-.5))));
  EXPECT_STREQ("0", gc(xdtoal(_asinhl(0))));
  EXPECT_STREQ("NAN", gc(xdtoal(_asinhl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_asinhl(INFINITY))));
}

BENCH(asinh, bench) {
  EZBENCH2("asinh", donothing, _asinh(.7));    // ~26ns
  EZBENCH2("asinhf", donothing, _asinhf(.7));  // ~17ns
  EZBENCH2("asinhl", donothing, _asinhl(.7));  // ~48ns
}
