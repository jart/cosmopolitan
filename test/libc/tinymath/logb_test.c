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
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/tinymath.h"

TEST(ilogb, yolo) {
  EXPECT_EQ(0, ilogb(1));
  EXPECT_EQ(1, ilogb(2));
  EXPECT_EQ(2, ilogb(4));
  EXPECT_EQ(63, ilogb(1e19));
  EXPECT_EQ(0, ilogbf(1));
  EXPECT_EQ(1, ilogbf(2));
  EXPECT_EQ(2, ilogbf(4));
  EXPECT_EQ(63, ilogb(1e19));
  EXPECT_EQ(0, ilogbl(1));
  EXPECT_EQ(1, ilogbl(2));
  EXPECT_EQ(2, ilogbl(4));
  EXPECT_EQ(63, ilogbl(1e19));
}

TEST(logb, yolo) {
  EXPECT_EQ(0, (int)logb(1));
  EXPECT_EQ(1, (int)logb(2));
  EXPECT_EQ(2, (int)logb(4));
  EXPECT_EQ(63, (int)logb(1e19));
  EXPECT_EQ(0, (int)logbf(1));
  EXPECT_EQ(1, (int)logbf(2));
  EXPECT_EQ(2, (int)logbf(4));
  EXPECT_EQ(63, (int)logb(1e19));
  EXPECT_EQ(0, (int)logbl(1));
  EXPECT_EQ(1, (int)logbl(2));
  EXPECT_EQ(2, (int)logbl(4));
  EXPECT_EQ(63, (int)logbl(1e19));
}
