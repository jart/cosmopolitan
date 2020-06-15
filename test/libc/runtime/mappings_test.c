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
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/runtime/mappings.h"
#include "libc/testlib/testlib.h"

#define ADDR

struct MemoryCoord stack = ADDRSIZE_TO_COORD(0x7fffffff0000L, 0x00010000);
struct MemoryCoord heap3 = ADDRSIZE_TO_COORD(0x200000020000L, 0x00010000);
struct MemoryCoord heap2 = ADDRSIZE_TO_COORD(0x200000010000L, 0x00010000);
struct MemoryCoord heap1 = ADDRSIZE_TO_COORD(0x200000000000L, 0x00010000);
struct MemoryCoord heapa = ADDRSIZE_TO_COORD(0x200000000000L, 0x00030000);
struct MemoryCoord progg = ADDRSIZE_TO_COORD(0x000000400000L, 0x00010000);
struct MemoryCoord bane = ADDRSIZE_TO_COORD(0xffff800000080000L, 0x00010000);

TEST(isoverlapping, test) {
  EXPECT_FALSE(ISOVERLAPPING(stack, heap3));
  EXPECT_FALSE(ISOVERLAPPING(heap1, heap2));
  EXPECT_FALSE(ISOVERLAPPING(heap2, heap3));
  EXPECT_FALSE(ISOVERLAPPING(heap1, heap3));
  EXPECT_TRUE(ISOVERLAPPING(heapa, heap1));
  EXPECT_TRUE(ISOVERLAPPING(heapa, heap3));
}

TEST(findmapping, limits) {
  ASSERT_EQ(INT_MAX, ADDR_TO_COORD(0x7fffffff0000L));
  ASSERT_EQ(INT_MIN, ADDR_TO_COORD(-0x800000000000L));
}

TEST(findmapping, test) {
  struct MemoryCoord c[] = {bane, progg, heap1, heap2, heap3, stack};
  EXPECT_EQ(6, ARRAYLEN(c));
  EXPECT_EQ(0, findmapping_(ADDR_TO_COORD(-0x800000000000UL), c, 6));
  EXPECT_EQ(1, findmapping_(ADDR_TO_COORD(-42), c, 6));
  EXPECT_EQ(1, findmapping_(ADDR_TO_COORD(0x000000300000L), c, 6));
  EXPECT_EQ(2, findmapping_(ADDR_TO_COORD(0x000000400000L), c, 6));
  EXPECT_EQ(6, findmapping_(ADDR_TO_COORD(0x7fffffffffffL), c, 6));
}
