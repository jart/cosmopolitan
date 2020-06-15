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

TEST(pow10, testLdbl) {
  EXPECT_LDBL_EQ(1, pow10l(0));
  EXPECT_LDBL_EQ(10, pow10l(1));
  EXPECT_LDBL_EQ(100, pow10l(2));
}

TEST(pow10, testDouble) {
  EXPECT_DOUBLE_EQ(1, pow10(0));
  EXPECT_DOUBLE_EQ(10, pow10(1));
  EXPECT_DOUBLE_EQ(100, pow10(2));
}

TEST(pow10, testFloat) {
  EXPECT_FLOAT_EQ(1, pow10f(0));
  EXPECT_FLOAT_EQ(10, pow10f(1));
  EXPECT_FLOAT_EQ(100, pow10f(2));
}
