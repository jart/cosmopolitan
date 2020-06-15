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
#include "libc/intrin/packsswb.h"
#include "libc/intrin/packuswb.h"
#include "libc/limits.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

const short S[8] = {0, 128, -128, 255, SHRT_MAX, SHRT_MIN, 0, 0};

TEST(packuswb, test) {
  unsigned char B[16] = {0};
  packuswb(B, S, S);
  EXPECT_EQ(0, B[0]);
  EXPECT_EQ(128, B[1]);
  EXPECT_EQ(0, B[2]);
  EXPECT_EQ(255, B[3]);
  EXPECT_EQ(255, B[4]);
  EXPECT_EQ(0, B[5]);
  EXPECT_EQ(0, B[6]);
  EXPECT_EQ(0, B[7]);
  EXPECT_EQ(0, B[8]);
  EXPECT_EQ(128, B[9]);
  EXPECT_EQ(0, B[10]);
  EXPECT_EQ(255, B[11]);
  EXPECT_EQ(255, B[12]);
  EXPECT_EQ(0, B[13]);
  EXPECT_EQ(0, B[14]);
  EXPECT_EQ(0, B[15]);
}

TEST(packsswb, test) {
  const short S[8] = {0, 128, -128, 255, SHRT_MAX, SHRT_MIN, 0, 0};
  signed char B[16] = {0};
  packsswb(B, S, S);
  EXPECT_EQ(0, B[0]);
  EXPECT_EQ(127, B[1]);
  EXPECT_EQ(-128, B[2]);
  EXPECT_EQ(127, B[3]);
  EXPECT_EQ(127, B[4]);
  EXPECT_EQ(-128, B[5]);
  EXPECT_EQ(0, B[6]);
  EXPECT_EQ(0, B[7]);
  EXPECT_EQ(0, B[8]);
  EXPECT_EQ(127, B[9]);
  EXPECT_EQ(-128, B[10]);
  EXPECT_EQ(127, B[11]);
  EXPECT_EQ(127, B[12]);
  EXPECT_EQ(-128, B[13]);
  EXPECT_EQ(0, B[14]);
  EXPECT_EQ(0, B[15]);
}
