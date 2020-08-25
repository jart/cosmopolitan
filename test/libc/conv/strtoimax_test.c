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
#include "libc/conv/conv.h"
#include "libc/testlib/testlib.h"

TEST(strtoimax, testZero) { EXPECT_EQ(0, strtoimax("0", NULL, 0)); }
TEST(strtoimax, testDecimal) { EXPECT_EQ(-123, strtoimax("-123", NULL, 0)); }
TEST(strtoimax, testHex) { EXPECT_EQ(-255, strtoimax("-0xff", NULL, 0)); }
TEST(strtoimax, testOctal) { EXPECT_EQ(-123, strtoimax("-0173", NULL, 0)); }

TEST(strtoimax, testLimits) {
  EXPECT_EQ(
      ((uintmax_t)0xffffffffffffffff) << 64 | (uintmax_t)0xffffffffffffffff,
      strtoimax("-1", NULL, 0));
  EXPECT_EQ(
      ((uintmax_t)0x7fffffffffffffff) << 64 | (uintmax_t)0xffffffffffffffff,
      strtoimax("0x7fffffffffffffffffffffffffffffff", NULL, 0));
}

TEST(strtoimax, testZeroExtend) {
  EXPECT_EQ(-1, strtoimax("-1", NULL, 0));
  EXPECT_EQ(0xffffffff, strtoimax("-1u", NULL, 0));
  EXPECT_EQ(0xffffffffffffffff, strtoimax("-1ul", NULL, 0));
}

TEST(strtoimax, testTwosBane) {
  EXPECT_EQ(((uintmax_t)0x8000000000000000) << 64 | 0x0000000000000000,
            strtoimax("0x80000000000000000000000000000000", NULL, 0));
}
