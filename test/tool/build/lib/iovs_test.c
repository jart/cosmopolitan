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
#include "libc/testlib/testlib.h"
#include "tool/build/lib/iovs.h"

TEST(iovs, testEmpty) {
  struct Iovs iv;
  InitIovs(&iv);
  EXPECT_EQ(0, iv.i);
  EXPECT_GE(iv.n, iv.i);
  FreeIovs(&iv);
}

TEST(iovs, testAppendEmptyString_wontCreateEmptyEntries) {
  struct Iovs iv;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, "", 0));
  EXPECT_NE(-1, AppendIovs(&iv, NULL, 0));
  EXPECT_EQ(0, iv.i);
  EXPECT_GE(iv.n, iv.i);
  FreeIovs(&iv);
}

TEST(iovs, testAppendContiguousVectors_coalescesAdjacentEntries) {
  struct Iovs iv;
  char buf[8], *b = buf;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, b + 0, 4));
  EXPECT_NE(-1, AppendIovs(&iv, b + 4, 4));
  EXPECT_EQ(1, iv.i);
  EXPECT_GE(iv.n, iv.i);
  EXPECT_EQ(8, iv.p[0].iov_len);
  EXPECT_EQ(b, iv.p[0].iov_base);
  FreeIovs(&iv);
}

TEST(iovs, testAppendNoncontiguousVectors_growsMemoryAndPreservesOrdering) {
  struct Iovs iv;
  char buf[8], *b = buf;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, b + 0, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 2, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 4, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 6, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 1, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 3, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 5, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 7, 1));
  EXPECT_EQ(8, iv.i);
  EXPECT_GE(iv.n, iv.i);
  EXPECT_EQ(b + 0, iv.p[0].iov_base);
  EXPECT_EQ(1, iv.p[0].iov_len);
  EXPECT_EQ(b + 2, iv.p[1].iov_base);
  EXPECT_EQ(1, iv.p[1].iov_len);
  EXPECT_EQ(b + 4, iv.p[2].iov_base);
  EXPECT_EQ(1, iv.p[2].iov_len);
  EXPECT_EQ(b + 6, iv.p[3].iov_base);
  EXPECT_EQ(1, iv.p[3].iov_len);
  EXPECT_EQ(b + 1, iv.p[4].iov_base);
  EXPECT_EQ(1, iv.p[4].iov_len);
  EXPECT_EQ(b + 3, iv.p[5].iov_base);
  EXPECT_EQ(1, iv.p[5].iov_len);
  EXPECT_EQ(b + 5, iv.p[6].iov_base);
  EXPECT_EQ(1, iv.p[6].iov_len);
  EXPECT_EQ(b + 7, iv.p[7].iov_base);
  EXPECT_EQ(1, iv.p[7].iov_len);
  FreeIovs(&iv);
}
