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
