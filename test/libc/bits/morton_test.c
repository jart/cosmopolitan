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
#include "libc/bits/morton.h"
#include "libc/nexgen32e/kcpuids.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(morton, test) {
  EXPECT_EQ(0, morton(0, 0));
  EXPECT_EQ(1, morton(0, 1));
  EXPECT_EQ(2, morton(1, 0));
  EXPECT_EQ(3, morton(1, 1));
  EXPECT_EQ(4, morton(0, 2));
  EXPECT_EQ(~0ul, morton(~0ul, ~0ul));
  EXPECT_EQ(0x7ffffffffffffffdul, morton(0x7ffffffeul, 0xfffffffful));
  EXPECT_EQ(0b1010101000010101010, morton(0b0000001111, 0b1111000000));
}

TEST(unmorton, test) {
  EXPECT_EQ(0, unmorton(0).ax);
  EXPECT_EQ(0, unmorton(0).dx);
  EXPECT_EQ(0, unmorton(1).ax);
  EXPECT_EQ(1, unmorton(1).dx);
  EXPECT_EQ(1, unmorton(2).ax);
  EXPECT_EQ(0, unmorton(2).dx);
  EXPECT_EQ(1, unmorton(3).ax);
  EXPECT_EQ(1, unmorton(3).dx);
  EXPECT_EQ(0, unmorton(4).ax);
  EXPECT_EQ(2, unmorton(4).dx);
  EXPECT_EQ(0xffffffffu, unmorton(~0ul).ax);
  EXPECT_EQ(0xffffffffu, unmorton(~0ul).dx);
  EXPECT_EQ(0x7ffffffeul, unmorton(0x7ffffffffffffffdul).ax);
  EXPECT_EQ(0xfffffffful, unmorton(0x7ffffffffffffffdul).dx);
  EXPECT_EQ(0b0000001111000000, unmorton(0b010101010000001010101).ax);
  EXPECT_EQ(0b0000000000001111, unmorton(0b010101010000001010101).dx);
}

BENCH(morton, bench) {
  EZBENCH2("morton", donothing,
           EXPROPRIATE(morton(CONCEAL("r", 123), CONCEAL("r", 123))));
  EZBENCH2("unmorton", donothing, EXPROPRIATE(unmorton(CONCEAL("r", 123))));
}
