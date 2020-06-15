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
#include "libc/intrin/paddsw.h"
#include "libc/intrin/paddw.h"
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(paddw, test) {
  short A[8] = {7};
  short B[8] = {11};
  short C[8];
  paddw(C, A, B);
  EXPECT_EQ(18, C[0]);
}

TEST(paddsw, test) {
  short A[8] = {7};
  short B[8] = {11};
  short C[8];
  paddsw(C, A, B);
  EXPECT_EQ(18, C[0]);
}

TEST(paddw, testOverflow_wrapsAround) {
  short A[8] = {SHRT_MAX, SHRT_MIN};
  short B[8] = {1, -1};
  paddw(A, A, B);
  EXPECT_EQ(SHRT_MIN, A[0]);
  EXPECT_EQ(SHRT_MAX, A[1]);
}

TEST(paddsw, testOverflow_saturates) {
  short A[8] = {SHRT_MAX, SHRT_MIN};
  short B[8] = {1, -1};
  paddsw(A, A, B);
  EXPECT_EQ(SHRT_MAX, A[0]);
  EXPECT_EQ(SHRT_MIN, A[1]);
}
