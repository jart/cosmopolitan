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
#include "libc/runtime/ring.h"
#include "libc/testlib/testlib.h"

TEST(ringalloc, testMagic) {
  char *p;
  size_t n;
  struct RingBuffer ring = {};
  n = FRAMESIZE * 2;
  EXPECT_NE(NULL, ringalloc(&ring, n));
  if ((p = ring.p)) {
    EXPECT_EQ(0, p[0]);
    EXPECT_EQ(0, p[7]);
    EXPECT_EQ(0, p[n + 0]);
    EXPECT_EQ(0, p[n + 7]);
    p[0] = 23;
    p[7] = 123;
    EXPECT_EQ(23, p[0]);
    EXPECT_EQ(123, p[7]);
    EXPECT_EQ(23, p[n + 0]);
    EXPECT_EQ(123, p[n + 7]);
  }
  EXPECT_NE(-1, ringfree(&ring));
}

TEST(ringalloc, testFrameSized) {
  struct RingBuffer ring = {};
  EXPECT_NE(NULL, ringalloc(&ring, FRAMESIZE));
  EXPECT_NE(-1, ringfree(&ring));
}
