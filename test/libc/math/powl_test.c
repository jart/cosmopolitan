/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/testlib/testlib.h"

long double x, y, z;

COMBO(x, pos1) { x = +1.3L; }
COMBO(x, pos2) { x = +2.3L; }
COMBO(y, pos1) { y = +1.3L; }
COMBO(y, pos2) { y = +2.3L; }
COMBO(z, pi) { z = M_PI; }
COMBO(z, e) { z = M_E; }

TEST(powl, lolgebra) {
  /* EXPECT_LDBL_EQ(1, powl(x, 0)); */
  /* EXPECT_LDBL_EQ(x, powl(x, 1)); */
  /* EXPECT_LDBL_EQ(powl(x, -1), 1.0L / x); */
  /* EXPECT_LDBL_EQ(powl(x, 0.5), sqrtl(x)); */
  /* EXPECT_LDBL_EQ(powl(x, y) * powl(x, z), powl(x, y + z)); */
  /* EXPECT_LDBL_EQ(powl(x, y) * powl(z, y), powl(x * z, y)); */
  /* EXPECT_LDBL_EQ(powl(x, y) / x, powl(x, y - 1)); */
  /* EXPECT_LDBL_EQ(x / powl(y, z), x * powl(y, -z)); */
  /* EXPECT_LDBL_EQ(powi(x, 0), 1); */
  /* EXPECT_LDBL_EQ(powi(x, 1), x); */
  /* EXPECT_LDBL_EQ(powi(x, -1), 1 / x); */
  /* EXPECT_LDBL_EQ(powi(1, x), 1); */
  /* EXPECT_LDBL_EQ(powi(x, y) * powi(z, y), powi(x * z, y)); */
}

TEST(powl, testConstants) {
  EXPECT_LDBL_EQ(16, powl(2, 4));
  /* TODO(jart): We need a better AlmostEqual() */
  /* EXPECT_LDBL_EQ(3.347061799072891e+62, powl(1337.31337, 20)); */
}
