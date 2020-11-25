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
#include "libc/bits/bits.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(bitreverse, test) {
  EXPECT_EQ(0xde, bitreverse8(123));
  EXPECT_EQ(0xde, (bitreverse8)(123));
  EXPECT_EQ(0xde00, bitreverse16(123));
  EXPECT_EQ(0xde00, (bitreverse16)(123));
  EXPECT_EQ(0xde000000u, bitreverse32(123));
  EXPECT_EQ(0xde000000u, (bitreverse32)(123));
  EXPECT_EQ(0xde00000000000000ul, bitreverse64(123));
  EXPECT_EQ(0xde00000000000000ul, (bitreverse64)(123));
  EXPECT_EQ(0x482d96c305f7c697ul, bitreverse64(0xe963efa0c369b412));
  EXPECT_EQ(0x482d96c305f7c697ul, (bitreverse64)(0xe963efa0c369b412));
}

BENCH(bitreverse, bench) {
  EZBENCH2("bitreverse8 mac", donothing,
           EXPROPRIATE(bitreverse8(CONCEAL("r", 123))));
  EZBENCH2("bitreverse8 fun", donothing,
           EXPROPRIATE((bitreverse8)(CONCEAL("r", 123))));
  EZBENCH2("bitreverse16 mac", donothing,
           EXPROPRIATE(bitreverse16(CONCEAL("r", 123))));
  EZBENCH2("bitreverse16 fun", donothing,
           EXPROPRIATE((bitreverse16)(CONCEAL("r", 123))));
  EZBENCH2("bitreverse32 mac", donothing,
           EXPROPRIATE(bitreverse32(CONCEAL("r", 123))));
  EZBENCH2("bitreverse32 fun", donothing,
           EXPROPRIATE((bitreverse32)(CONCEAL("r", (123)))));
}
