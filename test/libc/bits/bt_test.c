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
#include "libc/testlib/testlib.h"

TEST(bt_uint16x4, test) {
  uint16_t v[4] = {0};
  EXPECT_FALSE(bt(v, 0));
  EXPECT_FALSE(bt(v, 63));
  EXPECT_FALSE(bts(v, 63));
  EXPECT_TRUE(bt(v, 63));
  EXPECT_TRUE(bts(v, 63));
}

TEST(bt_uint32x2, test) {
  uint32_t v[2] = {0};
  EXPECT_FALSE(bt(v, 0));
  EXPECT_FALSE(bt(v, 63));
  EXPECT_FALSE(bts(v, 63));
  EXPECT_TRUE(bt(v, 63));
  EXPECT_TRUE(bts(v, 63));
}

TEST(bt_uint64x1, test) {
  uint64_t v = 0;
  EXPECT_FALSE(bt(&v, 0));
  EXPECT_FALSE(bt(&v, 63));
  EXPECT_FALSE(bts(&v, 63));
  EXPECT_TRUE(bt(&v, 63));
  EXPECT_TRUE(bts(&v, 63));
}

TEST(bt_uint64, testPresent) {
  uint64_t v = 1;
  EXPECT_TRUE(bt(&v, 0));
  EXPECT_FALSE(bt(&v, 63));
  v = 0x8000000000000001;
  EXPECT_TRUE(bt(&v, 0));
  EXPECT_TRUE(bt(&v, 63));
}

TEST(bt_uint64, testPresent_avoidingDeadCodeElimination) {
  volatile uint64_t v = 1;
  EXPECT_TRUE(bt(&v, 0));
  EXPECT_FALSE(bt(&v, 63));
  v = 0x8000000000000001;
  EXPECT_TRUE(bt(&v, 0));
  EXPECT_TRUE(bt(&v, 63));
}
