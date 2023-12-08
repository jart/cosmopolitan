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

TEST(tprecode8to16, test) {
  size_t size = 8;
  char16_t *buf = malloc(size * sizeof(char16_t));
  EXPECT_EQ(7, tprecode8to16(buf, size, "hello☻♥").ax);
  EXPECT_STREQ(u"hello☻♥", buf);
  free(buf);
}

TEST(tprecode8to16, testEmptyOut_doesNothingButStillCountsSrcLength) {
  axdx_t r;
  r = tprecode8to16(NULL, 0, "hi");
  EXPECT_EQ(0, r.ax);
  EXPECT_EQ(3, r.dx);
}

TEST(tprecode8to16, testOnlyRoomForNul_writesIt) {
  axdx_t r;
  char16_t b[1] = {1};
  r = tprecode8to16(b, 1, "hi");
  EXPECT_EQ(0, r.ax);
  EXPECT_EQ(3, r.dx);
  EXPECT_EQ(0, b[0]);
}

TEST(tprecode8to16, testTooLittle_stillNulTerminates) {
  axdx_t r;
  char16_t b[2] = {1, 2};
  r = tprecode8to16(b, 2, "hi");
  EXPECT_EQ(1, r.ax);
  EXPECT_EQ(3, r.dx);
  EXPECT_EQ('h', b[0]);
  EXPECT_EQ(0, b[1]);
}

TEST(tprecode8to16, test2) {
  char16_t b[128];
  EXPECT_EQ(34, tprecode8to16(b, 128, "(╯°□°)╯︵L┻━┻  𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹").ax);
  EXPECT_STREQ(u"(╯°□°)╯︵L┻━┻  𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹", b);
}

TEST(tprecode8to16, testAscii_vectorSpeedupWorks) {
  size_t size = 32;
  char16_t *buf = malloc(size * sizeof(char16_t));
  EXPECT_EQ(31,
            tprecode8to16(buf, size, "babaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").ax);
  EXPECT_STREQ(u"babaaaaaaaaaaaaaaaaaaaaaaaaaaaa", buf);
  free(buf);
}

BENCH(tprecode8to16, bench) {
  char16_t *buf = malloc((kHyperionSize + 1) * 2);
  EZBENCH2("tprecode8to16", donothing,
           tprecode8to16(buf, kHyperionSize, kHyperion));
  free(buf);
}
