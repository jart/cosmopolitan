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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(trunc, test) {
  EXPECT_STREQ("3", gc(xdtoa(trunc(3))));
  EXPECT_STREQ("3", gc(xdtoa(trunc(3.14))));
  EXPECT_STREQ("-3", gc(xdtoa(trunc(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoa(trunc(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoa(trunc(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(trunc(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(trunc(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(trunc(-INFINITY))));
}

TEST(truncf, test) {
  EXPECT_STREQ("3", gc(xdtoaf(truncf(3))));
  EXPECT_STREQ("3", gc(xdtoaf(truncf(3.14))));
  EXPECT_STREQ("-3", gc(xdtoaf(truncf(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoaf(truncf(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoaf(truncf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(truncf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(truncf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(truncf(-INFINITY))));
}

TEST(truncl, test) {
  EXPECT_STREQ("3", gc(xdtoal(truncl(3))));
  EXPECT_STREQ("3", gc(xdtoal(truncl(3.14))));
  EXPECT_STREQ("-3", gc(xdtoal(truncl(-3.14))));
  EXPECT_STREQ("-0", gc(xdtoal(truncl(-0.))));
  EXPECT_STREQ("NAN", gc(xdtoal(truncl(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoal(truncl(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(truncl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(truncl(-INFINITY))));
}

BENCH(truncl, bench) {
  double _trunc(double) asm("trunc");
  float _truncf(float) asm("truncf");
  long double _truncl(long double) asm("truncl");
  EZBENCH2("trunc", donothing, _trunc(.7));   /* ~2ns */
  EZBENCH2("truncf", donothing, _truncf(.7)); /* ~2ns */
  EZBENCH2("truncl", donothing, _truncl(.7)); /* ~9ns */
}
