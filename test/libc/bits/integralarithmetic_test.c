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
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/testlib/testlib.h"

TEST(TwosComplementBane, LiteralsThatAreLiterallyTheSameNumber) {
  EXPECT_EQ(4, sizeof(INT_MIN));
  EXPECT_EQ(8, sizeof(-2147483648));
  EXPECT_TRUE(TYPE_SIGNED(-2147483648));
  EXPECT_FALSE(TYPE_SIGNED(0x80000000));
  EXPECT_FALSE(TYPE_SIGNED(-0x80000000));
}

TEST(ShiftArithmeticRight, DeclassifiedByGuySteele) {
  EXPECT_EQ(-1u, SAR(-2u, 1u));
  EXPECT_EQ(-1u, SAR(-1u, 1u));
  EXPECT_EQ(0xc0000000u, SAR(0x80000000u, 1u));
}

TEST(RotateRight, Test) {
  EXPECT_EQ(0x41122334u, ROR(0x11223344u, 4));
  EXPECT_EQ(0x44112233u, ROR(0x11223344u, 8));
  EXPECT_EQ(0x34411223u, ROR(0x11223344u, 12));
  EXPECT_EQ(0x33441122u, ROR(0x11223344u, 16));
}

TEST(RotateLeft, Test) {
  EXPECT_EQ(0x12233441u, ROL(0x11223344u, 4));
  EXPECT_EQ(0x22334411u, ROL(0x11223344u, 8));
  EXPECT_EQ(0x23344112u, ROL(0x11223344u, 12));
  EXPECT_EQ(0x33441122u, ROL(0x11223344u, 16));
}
