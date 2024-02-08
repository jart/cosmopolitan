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

TEST(log1p, test) {
  EXPECT_STREQ("1", gc(xdtoa(log1p(M_E - 1))));
  EXPECT_STREQ("2", gc(xdtoa(log1p(M_E * M_E - 1))));
  EXPECT_STREQ("NAN", gc(xdtoa(log1p(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(log1p(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log1p(-1))));
  EXPECT_TRUE(isnan(log1p(-2)));
}

TEST(log1pf, test) {
  EXPECT_STREQ("1", gc(xdtoaf(log1pf(M_E - 1))));
  EXPECT_STREQ("NAN", gc(xdtoaf(log1pf(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(log1pf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(log1pf(-1))));
  EXPECT_TRUE(isnan(log1pf(-2)));
}

TEST(log1pl, test) {
  EXPECT_STREQ("1", gc(xdtoal(log1pl(1.71828182845904523536L))));
  EXPECT_STREQ("NAN", gc(xdtoal(log1pl(NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(log1pl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(log1pl(-1))));
  EXPECT_TRUE(isnan(log1pl(-2)));
}

BENCH(log1p, bench) {
  double _log1p(double) asm("log1p");
  float _log1pf(float) asm("log1pf");
  long double _log1pl(long double) asm("log1pl");
  EZBENCH2("log1p", donothing, _log1p(.7));   /* ~17ns */
  EZBENCH2("log1pf", donothing, _log1pf(.7)); /* ~17ns */
  EZBENCH2("log1pl", donothing, _log1pl(.7)); /* ~28ns */
}
