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
  char s[128] = "AZCDabcdABCDabcdABCDabcdABCDabcdABCDabcd";
  EXPECT_STREQ("azcdabcdabcdabcdabcdabcdabcdabcdabcdabcd", strtolower(s));
}

TEST(strtolower, testUnaligned) {
  char s[128] = "AZCDabcdABCDabcdABCDabcdABCDabcdABCDabcd";
  strtolower(s + 1);
  EXPECT_STREQ("Azcdabcdabcdabcdabcdabcdabcdabcdabcdabcd", s);
}

TEST(strtoupper, testAligned) {
  char s[128] = "AZCDabcdABCDabcdA0CDabcdABCDabcdABCDabcd";
  EXPECT_STREQ("AZCDABCDABCDABCDA0CDABCDABCDABCDABCDABCD", strtoupper(s));
}

TEST(strtoupper, testUnaligned) {
  char s[128] = "aZCDabcdABCDabcdABCDabcdABCDabcdABCDabcd";
  strtoupper(s + 1);
  EXPECT_STREQ("aZCDABCDABCDABCDABCDABCDABCDABCDABCDABCD", s);
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
