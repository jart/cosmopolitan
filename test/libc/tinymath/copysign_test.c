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

TEST(copysign, test) {
  EXPECT_STREQ("0", gc(xdtoa(copysign(0, +0))));
  EXPECT_STREQ("-0", gc(xdtoa(copysign(0, -0.))));
  EXPECT_STREQ("0", gc(xdtoa(copysign(0, +1))));
  EXPECT_STREQ("-0", gc(xdtoa(copysign(-0., -1))));
  EXPECT_STREQ("2", gc(xdtoa(copysign(2, +1))));
  EXPECT_STREQ("-2", gc(xdtoa(copysign(-2, -1))));
  EXPECT_STREQ("NAN", gc(xdtoa(copysign(NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoa(copysign(NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(copysign(INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(copysign(INFINITY, -1))));
  EXPECT_STREQ("NAN", gc(xdtoa(copysign(-NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoa(copysign(-NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(copysign(-INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(copysign(-INFINITY, -1))));
}

TEST(copysignl, test) {
  EXPECT_STREQ("0", gc(xdtoal(copysignl(0, +0))));
  EXPECT_STREQ("-0", gc(xdtoal(copysignl(0, -0.))));
  EXPECT_STREQ("0", gc(xdtoal(copysignl(0, +1))));
  EXPECT_STREQ("-0", gc(xdtoal(copysignl(-0., -1))));
  EXPECT_STREQ("2", gc(xdtoal(copysignl(2, +1))));
  EXPECT_STREQ("-2", gc(xdtoal(copysignl(-2, -1))));
  EXPECT_STREQ("NAN", gc(xdtoal(copysignl(NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoal(copysignl(NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(copysignl(INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(copysignl(INFINITY, -1))));
  EXPECT_STREQ("NAN", gc(xdtoal(copysignl(-NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoal(copysignl(-NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(copysignl(-INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(copysignl(-INFINITY, -1))));
}

TEST(copysignf, test) {
  EXPECT_STREQ("0", gc(xdtoaf(copysignf(0, +0))));
  EXPECT_STREQ("-0", gc(xdtoaf(copysignf(0, -0.))));
  EXPECT_STREQ("0", gc(xdtoaf(copysignf(0, +1))));
  EXPECT_STREQ("-0", gc(xdtoaf(copysignf(-0., -1))));
  EXPECT_STREQ("2", gc(xdtoaf(copysignf(2, +1))));
  EXPECT_STREQ("-2", gc(xdtoaf(copysignf(-2, -1))));
  EXPECT_STREQ("NAN", gc(xdtoaf(copysignf(NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(copysignf(NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(copysignf(INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(copysignf(INFINITY, -1))));
  EXPECT_STREQ("NAN", gc(xdtoaf(copysignf(-NAN, +1))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(copysignf(-NAN, -1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(copysignf(-INFINITY, +1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(copysignf(-INFINITY, -1))));
}
