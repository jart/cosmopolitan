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
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(llog10, test) {
  EXPECT_EQ(0, llog10(1));
  EXPECT_EQ(0, llog10(2));
  EXPECT_EQ(0, llog10(9));
  EXPECT_EQ(1, llog10(10));
  EXPECT_EQ(1, llog10(11));
  EXPECT_EQ(1, llog10(99));
  EXPECT_EQ(2, llog10(100));
  EXPECT_EQ(2, llog10(101));
  EXPECT_EQ(9, llog10(INT_MAX));
  EXPECT_EQ(12, llog10(1000000000000));
}
