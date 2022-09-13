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

TEST(log10l, test) {
  EXPECT_STREQ("1", gc(xdtoal(log10l(10))));
  EXPECT_STREQ("NAN", gc(xdtoal(log10l(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(log10l(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(log10l(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(log10l(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(log10l(-0.))));
  EXPECT_STREQ("-NAN", gc(xdtoal(log10l(-1))));
  EXPECT_STREQ("-NAN", gc(xdtoal(log10l(-2))));
}

TEST(log10, test) {
  EXPECT_STREQ(".301029995663981", gc(xdtoa(log10(2))));
  EXPECT_STREQ("2", gc(xdtoa(log10(100))));
  EXPECT_STREQ("NAN", gc(xdtoa(log10(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(log10(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(log10(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log10(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(log10(-0.))));
  EXPECT_STREQ("-NAN", gc(xdtoa(log10(-1))));
  EXPECT_STREQ("-NAN", gc(xdtoa(log10(-2))));
}

TEST(log10f, test) {
  EXPECT_STREQ("1", gc(xdtoaf(log10f(10))));
  EXPECT_STREQ("NAN", gc(xdtoaf(log10f(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(log10f(1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(log10f(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(log10f(0))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(log10f(-0.))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(log10f(-1))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(log10f(-2))));
}

BENCH(log10, bench) {
  double _log10(double) asm("log10");
  float _log10f(float) asm("log10f");
  long double _log10l(long double) asm("log10l");
  EZBENCH2("log10", donothing, _log10(.7));   /* ~20ns */
  EZBENCH2("log10f", donothing, _log10f(.7)); /* ~21ns */
  EZBENCH2("log10l", donothing, _log10l(.7)); /* ~21ns */
}
