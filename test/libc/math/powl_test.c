/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
