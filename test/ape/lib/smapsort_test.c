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
#include "ape/lib/pc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(smapsort, testEmpty_doesntOverrunBuffer) {
  struct SmapEntry *smap = tmalloc(sizeof(struct SmapEntry));
  memset(smap, 0, sizeof(struct SmapEntry));
  smapsort(smap);
  EXPECT_EQ(0, smap[0].addr);
  EXPECT_EQ(0, smap[0].size);
  tfree(smap);
}

/* TEST(smapsort, testSorted_doesNothing) { */
/*   struct SmapEntry *smap = tmalloc(4 * sizeof(struct SmapEntry)); */
/*   memset(smap, 0, 4 * sizeof(struct SmapEntry)); */
/*   smap[0].addr = 0; */
/*   smap[0].size = 0x7000; */
/*   smap[0].type = kMemoryUsable; */
/*   smap[1].addr = 0x7000; */
/*   smap[1].size = 0x1000; */
/*   smap[1].type = kMemoryUnusable; */
/*   smap[2].addr = 0x14000; */
/*   smap[2].size = 0x1000; */
/*   smap[2].type = kMemoryBad; */
/*   smapsort(smap); */
/*   EXPECT_EQ(0, smap[0].addr); */
/*   EXPECT_EQ(0x7000, smap[0].size); */
/*   EXPECT_EQ(kMemoryUsable, smap[0].type); */
/*   EXPECT_EQ(0x7000, smap[1].addr); */
/*   EXPECT_EQ(0x1000, smap[1].size); */
/*   EXPECT_EQ(kMemoryUnusable, smap[1].type); */
/*   EXPECT_EQ(0x14000, smap[2].addr); */
/*   EXPECT_EQ(0x1000, smap[2].size); */
/*   EXPECT_EQ(kMemoryBad, smap[2].type); */
/*   tfree(smap); */
/* } */

/* TEST(smapsort, testUnsorted_sortsByAddress) { */
/*   struct SmapEntry *smap = tmalloc(4 * sizeof(struct SmapEntry)); */
/*   memset(smap, 0, 4 * sizeof(struct SmapEntry)); */
/*   smap[2].addr = 0; */
/*   smap[2].size = 0x7000; */
/*   smap[2].type = kMemoryUsable; */
/*   smap[0].addr = 0x7000; */
/*   smap[0].size = 0x1000; */
/*   smap[0].type = kMemoryUnusable; */
/*   smap[1].addr = 0x14000; */
/*   smap[1].size = 0x1000; */
/*   smap[1].type = kMemoryBad; */
/*   smapsort(smap); */
/*   EXPECT_EQ(0, smap[0].addr); */
/*   EXPECT_EQ(0x7000, smap[0].size); */
/*   EXPECT_EQ(kMemoryUsable, smap[0].type); */
/*   EXPECT_EQ(0x7000, smap[1].addr); */
/*   EXPECT_EQ(0x1000, smap[1].size); */
/*   EXPECT_EQ(kMemoryUnusable, smap[1].type); */
/*   EXPECT_EQ(0x14000, smap[2].addr); */
/*   EXPECT_EQ(0x1000, smap[2].size); */
/*   EXPECT_EQ(kMemoryBad, smap[2].type); */
/*   tfree(smap); */
/* } */
