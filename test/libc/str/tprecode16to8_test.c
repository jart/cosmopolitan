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
  char *buf = tmalloc(size);
  EXPECT_EQ(31,
            tprecode16to8(buf, size, u"babaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").ax);
  EXPECT_STREQ("babaaaaaaaaaaaaaaaaaaaaaaaaaaaa", buf);
  tfree(buf);
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
