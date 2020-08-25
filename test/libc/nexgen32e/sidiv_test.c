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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"

TEST(sidiv, smoke) {
  EXPECT_EQ(13373133731337 / 10, div10int64(13373133731337));
  EXPECT_EQ(13373133731337 / 100, div100int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000, div1000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 10000, div10000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000000, div1000000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000000000, div1000000000int64(13373133731337));
}

TEST(sirem, smoke) {
  EXPECT_EQ(13373133731337 % 10, rem10int64(13373133731337));
  EXPECT_EQ(13373133731337 % 100, rem100int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000, rem1000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 10000, rem10000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000000, rem1000000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000000000, rem1000000000int64(13373133731337));
}

TEST(rem, euclid) {
  ASSERT_EQ(-2, rem10int64(-12));
  ASSERT_EQ(-1, rem10int64(-1));
  ASSERT_EQ(0, rem10int64(0));
  ASSERT_EQ(1, rem10int64(1));
  ASSERT_EQ(9, rem10int64(9));
  ASSERT_EQ(1, rem10int64(11));
}
