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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(strtolower, testAligned) {
  EXPECT_STREQ("azcdabcdabcdabcd",
               strtolower(tgc(tstrdup("AZCDabcdABCDabcd"))));
  EXPECT_STREQ("azcdabcdabcdabcdabcdabcdabcdabcd",
               strtolower(tgc(tstrdup("AZCDabcdABCDabcdABCDabcdABCDabcd"))));
}

TEST(strtolower, testUnaligned) {
  EXPECT_STREQ("1", strtolower(tgc(tstrdup("1"))));
  EXPECT_STREQ(
      "zcdabcdabcdabcdabcdabcdabcdabc",
      strtolower((char *)tgc(tstrdup("AZCDabcdABCDabcdABCDabcdABCDabc")) + 1));
}

TEST(strtoupper, testAligned) {
  EXPECT_STREQ("AZCDABCDABCDABCD",
               strtoupper(tgc(tstrdup("AZCDabcdABCDabcd"))));
  EXPECT_STREQ("AZCDABCDABCDABCDABCDABCDABCDABCD",
               strtoupper(tgc(tstrdup("AZCDabcdABCDabcdABCDabcdABCDabcd"))));
}

TEST(strtoupper, testUnaligned) {
  EXPECT_STREQ("1", strtoupper(tgc(tstrdup("1"))));
  EXPECT_STREQ(
      "ZCDABCDABCDABCDABCDABCDABCDABC",
      strtoupper((char *)tgc(tstrdup("AZCDabcdABCDabcdABCDabcdABCDabc")) + 1));
}

BENCH(strtolower, bench) {
  size_t size = FRAMESIZE;
  char *data = tgc(tmalloc(size));
  EZBENCH2(
      "strtolower",
      {
        rngset(data, size, rand64, -1);
        data[size - 1] = 0;
      },
      strtolower(data));
}
