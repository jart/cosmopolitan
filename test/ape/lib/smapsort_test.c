/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
