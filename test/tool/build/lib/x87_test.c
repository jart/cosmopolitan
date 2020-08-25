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
#include "tool/build/lib/x87.h"

TEST(x87, fprem) {
  ASSERT_LDBL_EQ(1, fprem(1, -1.5, NULL));
  ASSERT_LDBL_EQ(1.1766221079117338e-14L,
                 fprem(12300000000000000.L, .0000000000000123L, NULL));
}

TEST(x87, fprem1) {
  ASSERT_LDBL_EQ(-.5, fprem1(1, -1.5, NULL));
  ASSERT_LDBL_EQ(-5.337789208826618e-16,
                 fprem1(12300000000000000.L, .0000000000000123L, NULL));
}

TEST(x87, fpremFlags) {
  uint32_t sw = 0xffff;
  ASSERT_LDBL_EQ(1, fprem(1, -1.5, &sw));
  ASSERT_EQ(0b1011100011111111, sw);
}
