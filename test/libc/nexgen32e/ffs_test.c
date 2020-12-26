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
#include "libc/nexgen32e/ffs.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"

TEST(ffs, test) {
  EXPECT_EQ(__builtin_ffs(0), (ffs)(0));
  EXPECT_EQ(__builtin_ffs(1), (ffs)(1));
  EXPECT_EQ(__builtin_ffs(0x00100000), (ffs)(0x00100000));
  EXPECT_EQ(__builtin_ffs(-1), (ffs)(-1));
}

TEST(ffsl, test) {
  EXPECT_EQ(__builtin_ffsl(0), (ffsl)(0));
  EXPECT_EQ(__builtin_ffsl(1), (ffsl)(1));
  EXPECT_EQ(__builtin_ffsl(0x00100000), (ffsl)(0x00100000));
  EXPECT_EQ(__builtin_ffsl(0x0010000000000000), (ffsl)(0x0010000000000000));
  EXPECT_EQ(__builtin_ffsl(-1), (ffsl)(-1));
}

TEST(ffsll, test) {
  EXPECT_EQ(__builtin_ffsll(0), (ffsll)(0));
  EXPECT_EQ(__builtin_ffsll(1), (ffsll)(1));
  EXPECT_EQ(__builtin_ffsll(0x00100000), (ffsll)(0x00100000));
  EXPECT_EQ(__builtin_ffsll(0x0010000000000000), (ffsll)(0x0010000000000000));
  EXPECT_EQ(__builtin_ffsll(-1), (ffsll)(-1));
}
