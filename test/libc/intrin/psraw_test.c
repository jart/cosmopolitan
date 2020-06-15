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
#include "libc/intrin/psraw.h"
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(psraw, testPositive) {
  short A[8] = {1, 2, SHRT_MAX};
  psraw(A, A, 1);
  EXPECT_EQ(0, A[0]);
  EXPECT_EQ(1, A[1]);
  EXPECT_EQ(SHRT_MAX / 2, A[2]);
}

TEST(psraw, testNegative) {
  short A[8] = {-1, -2, SHRT_MIN};
  psraw(A, A, 1);
  EXPECT_EQ(-1, A[0]);
  EXPECT_EQ(-1, A[1]);
  EXPECT_EQ(SHRT_MIN / 2, A[2]);
}
