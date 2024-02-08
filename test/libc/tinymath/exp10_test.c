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
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _exp10(double) asm("exp10");
float _exp10f(float) asm("exp10f");
long double _exp10l(long double) asm("exp10l");

TEST(exp10l, test) {
  EXPECT_STREQ("1", gc(xdtoal(_exp10l(0))));
  EXPECT_STREQ("1", gc(xdtoal(_exp10l(-0.))));
  EXPECT_STREQ(".01", gc(xdtoal(_exp10l(-2.))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_exp10l(INFINITY))));
  EXPECT_STREQ("0", gc(xdtoal(_exp10l(-INFINITY))));
  EXPECT_STREQ("NAN", gc(xdtoal(_exp10l(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(_exp10l(-132098844872390))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_exp10l(132098844872390))));
}

TEST(exp10, test) {
  EXPECT_STREQ("1", gc(xdtoa(_exp10(0))));
  EXPECT_STREQ("1", gc(xdtoa(_exp10(-0.))));
  EXPECT_STREQ(".01", gc(xdtoa(_exp10(-2.))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_exp10(INFINITY))));
  EXPECT_STREQ("0", gc(xdtoa(_exp10(-INFINITY))));
  EXPECT_STREQ("NAN", gc(xdtoa(_exp10(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(_exp10(-132098844872390))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_exp10(132098844872390))));
}

TEST(exp10f, test) {
  EXPECT_STREQ("1", gc(xdtoaf(_exp10f(0))));
  EXPECT_STREQ("1", gc(xdtoaf(_exp10f(-0.))));
  EXPECT_STREQ(".01", gc(xdtoaf(_exp10f(-2.))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_exp10f(INFINITY))));
  EXPECT_STREQ("0", gc(xdtoaf(_exp10f(-INFINITY))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_exp10f(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(_exp10f(-132098844872390))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_exp10f(132098844872390))));
}
