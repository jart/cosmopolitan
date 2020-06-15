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
#include "libc/conv/conv.h"
#include "libc/conv/sizemultiply.h"
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

size_t out;

TEST(sizemultiply, testMultiplication) {
  EXPECT_TRUE(sizemultiply(&out, 7, 11));
  EXPECT_EQ(7 * 11, out);
  EXPECT_TRUE(sizemultiply(&out, 11, 7));
  EXPECT_EQ(7 * 11, out);
  EXPECT_TRUE(sizemultiply(&out, 0, 11));
  EXPECT_EQ(0 * 11, out);
  EXPECT_TRUE(sizemultiply(&out, 11, 0));
  EXPECT_EQ(0 * 11, out);
}

TEST(sizemultiply, testOverflow_alwaysReturnsSizeMax) {
  EXPECT_FALSE(sizemultiply(&out, 7, SIZE_MAX));
  EXPECT_EQ(SIZE_MAX, out);
  EXPECT_FALSE(sizemultiply(&out, SIZE_MAX, 7));
  EXPECT_EQ(SIZE_MAX, out);
}

TEST(sizemultiply, testOverflow_closeButNoCigar) {
  EXPECT_TRUE(sizemultiply(&out, SIZE_MAX, 1));
  EXPECT_EQ(SIZE_MAX, out);
  EXPECT_TRUE(sizemultiply(&out, 1, SIZE_MAX));
  EXPECT_EQ(SIZE_MAX, out);
}
