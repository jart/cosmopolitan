/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

TEST(atan2l, test) {
  volatile double a = -.9816175436063843;
  volatile double b = -.1908585813741899;
  EXPECT_STREQ("-2.95", gc(xasprintf("%.2f", atan2f(b, a))));
  EXPECT_STREQ("-2.95", gc(xasprintf("%.2f", atan2(b, a))));
#ifndef __aarch64__
  // TODO: implement quad floating point into printf
  EXPECT_STREQ("-2.95", gc(xasprintf("%.2Lf", atan2l(b, a))));
#endif
}

TEST(atan2, testSpecialCases) {
  ASSERT_STREQ("NAN", gc(xdtoa(atan2(NAN, 0))));
  ASSERT_STREQ("NAN", gc(xdtoa(atan2(0, NAN))));
  ASSERT_STREQ("0", gc(xdtoa(atan2(+0., +0.))));
  ASSERT_STREQ("0", gc(xdtoa(atan2(+0., +1.))));
  ASSERT_STREQ("0", gc(xdtoa(atan2(+0., +2.))));
  ASSERT_STREQ("0", gc(xdtoa(atan2(1, INFINITY))));
  ASSERT_STREQ("3.141592653589793", gc(xdtoal(atan2(+0., -0.))));
  ASSERT_STREQ("3.141592653589793", gc(xdtoal(atan2(+0., -1.))));
  ASSERT_STREQ("3.141592653589793", gc(xdtoal(atan2(+0., -2.))));
  ASSERT_STREQ("-1.570796326794897", gc(xdtoal(atan2(-1., -0.))));
  ASSERT_STREQ("-1.570796326794897", gc(xdtoal(atan2(-1., +0.))));
  ASSERT_STREQ("-1.570796326794897", gc(xdtoal(atan2(-2., -0.))));
  ASSERT_STREQ("-1.570796326794897", gc(xdtoal(atan2(-2., +0.))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(+1., -0.))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(+1., +0.))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(+2., -0.))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(+2., +0.))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(INFINITY, 1))));
  ASSERT_STREQ("1.570796326794897", gc(xdtoal(atan2(INFINITY, -1))));
  ASSERT_STREQ("3.141592653589793", gc(xdtoal(atan2(1, -INFINITY))));
  ASSERT_STREQ("2.356194490192345", gc(xdtoal(atan2(INFINITY, -INFINITY))));
  ASSERT_STREQ(".7853981633974483", gc(xdtoal(atan2(INFINITY, INFINITY))));
}
