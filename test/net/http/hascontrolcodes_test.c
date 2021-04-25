/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

TEST(HasControlCodes, test) {
  EXPECT_FALSE(
      HasControlCodes(kHyperion, kHyperionSize, kControlC0 | kControlC1));
  EXPECT_TRUE(HasControlCodes("hi\1", -1, kControlC0));
  EXPECT_FALSE(HasControlCodes("hi\1", -1, kControlC1));
  EXPECT_FALSE(HasControlCodes("hi there", -1, 0));
  EXPECT_TRUE(HasControlCodes("hi\tthere", -1, kControlWs));
}

TEST(HasControlCodes, testDoesUtf8) {
  EXPECT_FALSE(HasControlCodes(u8"→", -1, kControlC0 | kControlC1));
  EXPECT_FALSE(HasControlCodes("\304\200", -1, kControlC0 | kControlC1));
  EXPECT_TRUE(HasControlCodes("\300\200", -1, kControlC0 | kControlC1));
  EXPECT_FALSE(HasControlCodes("\300\200", -1, kControlC1));
  EXPECT_TRUE(HasControlCodes("\302\202", -1, kControlC0 | kControlC1));
  EXPECT_TRUE(HasControlCodes("\302\202", -1, kControlC1));
  EXPECT_FALSE(HasControlCodes("\302\202", -1, kControlC0));
}

TEST(HasControlCodes, testHasLatin1FallbackBehavior) {
  EXPECT_TRUE(HasControlCodes("\202", -1, kControlWs | kControlC1));
  EXPECT_FALSE(HasControlCodes("\202", -1, kControlC0));
}

BENCH(HasControlCodes, bench) {
  EZBENCH2("HasControlCodes", donothing,
           HasControlCodes(kHyperion, kHyperionSize, kControlWs));
}
