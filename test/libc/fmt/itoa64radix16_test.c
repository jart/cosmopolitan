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
#include "libc/fmt/itoa.h"
#include "libc/testlib/testlib.h"

TEST(itoa64radix16, test) {
  char buf[21];
  EXPECT_EQ(5, uint64toarray_radix16(0x31337, buf));
  EXPECT_STREQ("31337", buf);
  EXPECT_EQ(2, uint64toarray_radix16(0x13, buf));
  EXPECT_STREQ("13", buf);
  EXPECT_EQ(3, uint64toarray_radix16(0x113, buf));
  EXPECT_STREQ("113", buf);
}

TEST(itoa64fixed16, test) {
  char buf[21];
  EXPECT_EQ(8, uint64toarray_fixed16(0x31337, buf, 32));
  EXPECT_STREQ("00031337", buf);
}
