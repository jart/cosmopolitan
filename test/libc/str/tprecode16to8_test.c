/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(tprecode16to8, test) {
  char b[128];
  EXPECT_EQ(69, tprecode16to8(b, 128, u"(╯°□°)╯︵L┻━┻ 𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹").ax);
  EXPECT_STREQ("(╯°□°)╯︵L┻━┻ 𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹", b);
}

TEST(tprecode16to8, testEmptyOut_doesNothingButStillCountsSrcLength) {
  axdx_t r;
  r = tprecode16to8(NULL, 0, u"hi");
  EXPECT_EQ(0, r.ax);
  EXPECT_EQ(3, r.dx);
}

TEST(tprecode16to8, testTooLittle_stillNulTerminates) {
  axdx_t r;
  char b[2] = {1, 2};
  r = tprecode16to8(b, 2, u"hi");
  EXPECT_EQ(1, r.ax);
  EXPECT_EQ(3, r.dx);
  EXPECT_EQ('h', b[0]);
  EXPECT_EQ(0, b[1]);
}

TEST(tprecode16to8, testAscii_vectorSpeedupWorks) {
  size_t size = 32;
  char *buf = malloc(size);
  EXPECT_EQ(31,
            tprecode16to8(buf, size, u"babaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").ax);
  EXPECT_STREQ("babaaaaaaaaaaaaaaaaaaaaaaaaaaaa", buf);
  free(buf);
}

BENCH(tprecode16to8, bench) {
  char *buf8 = malloc(kHyperionSize + 1);
  char16_t *buf16 = malloc((kHyperionSize + 1) * 2);
  tprecode8to16(buf16, kHyperionSize + 1, kHyperion);
  EZBENCH2("tprecode16to8", donothing,
           tprecode16to8(buf8, kHyperionSize + 1, buf16));
  free(buf16);
  free(buf8);
}
