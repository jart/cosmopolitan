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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(tprecode8to16, test) {
  size_t size = 8;
  char16_t *buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(7, tprecode8to16(buf, size, "hello☻♥").ax);
  EXPECT_STREQ(u"hello☻♥", buf);
  tfree(buf);
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
  char16_t *buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(31,
            tprecode8to16(buf, size, "babaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").ax);
  EXPECT_STREQ(u"babaaaaaaaaaaaaaaaaaaaaaaaaaaaa", buf);
  tfree(buf);
}

BENCH(tprecode8to16, bench) {
  char16_t *buf = malloc((kHyperionSize + 1) * 2);
  EZBENCH2("tprecode8to16", donothing,
           tprecode8to16(buf, kHyperionSize, kHyperion));
  free(buf);
}
