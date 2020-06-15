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
#include "libc/intrin/palignr.h"
#include "libc/testlib/testlib.h"

const int A[4] = {1, 2, 3, 4};
const int B[4] = {5, 6, 7, 8};

TEST(palignr, testLeftpad) {
  int C[4] = {0};
  palignr(C, B, A, 12);
  EXPECT_EQ(4, C[0]);
  EXPECT_EQ(5, C[1]);
  EXPECT_EQ(6, C[2]);
  EXPECT_EQ(7, C[3]);
}

TEST(palignr, test0) {
  int C[4];
  palignr(C, B, A, 0);
  EXPECT_EQ(1, C[0]);
  EXPECT_EQ(2, C[1]);
  EXPECT_EQ(3, C[2]);
  EXPECT_EQ(4, C[3]);
}

TEST(palignr, test4) {
  int C[4];
  palignr(C, B, A, 4);
  EXPECT_EQ(2, C[0]);
  EXPECT_EQ(3, C[1]);
  EXPECT_EQ(4, C[2]);
  EXPECT_EQ(5, C[3]);
}

TEST(palignr, test12) {
  int C[4];
  palignr(C, B, A, 12);
  EXPECT_EQ(4, C[0]);
  EXPECT_EQ(5, C[1]);
  EXPECT_EQ(6, C[2]);
  EXPECT_EQ(7, C[3]);
}

TEST(palignr, test16) {
  int C[4];
  palignr(C, B, A, 16);
  EXPECT_EQ(5, C[0]);
  EXPECT_EQ(6, C[1]);
  EXPECT_EQ(7, C[2]);
  EXPECT_EQ(8, C[3]);
}

TEST(palignr, test20) {
  int C[4] = {-1, -1, -1, -1};
  palignr(C, B, A, 20);
  EXPECT_EQ(6, C[0]);
  EXPECT_EQ(7, C[1]);
  EXPECT_EQ(8, C[2]);
  EXPECT_EQ(0, C[3]);
}

TEST(palignr, test32) {
  int C[4] = {-1, -1, -1, -1};
  palignr(C, B, A, 32);
  EXPECT_EQ(0, C[0]);
  EXPECT_EQ(0, C[1]);
  EXPECT_EQ(0, C[2]);
  EXPECT_EQ(0, C[3]);
}
