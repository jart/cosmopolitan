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
#include "libc/bits/bits.h"
#include "libc/fmt/conv.h"
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(strtoumax, testZero) {
  EXPECT_EQ(UINTMAX_MIN, strtoumax("0", NULL, 0));
}
TEST(strtoumax, testDecimal) {
  EXPECT_EQ(123, strtoumax("123", NULL, 0));
}
TEST(strtoumax, testHex) {
  EXPECT_EQ(255, strtoumax("0xff", NULL, 0));
}
TEST(strtoumax, testOctal) {
  EXPECT_EQ(123, strtoumax("0173", NULL, 0));
}

TEST(strtoumax, testMaximum) {
  EXPECT_EQ(UINTMAX_MAX,
            strtoumax("340282366920938463463374607431768211455", NULL, 0));
  EXPECT_EQ(UINTMAX_MAX,
            strtoumax("0xffffffffffffffffffffffffffffffff", NULL, 0));
}
