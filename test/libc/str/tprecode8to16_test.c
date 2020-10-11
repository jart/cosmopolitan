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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(tprecode8to16, test) {
  size_t size = 8;
  char16_t *buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(7, tprecode8to16(buf, size, "hello☻♥"));
  EXPECT_STREQ(u"hello☻♥", buf);
  tfree(buf);
}

TEST(tprecode8to16, test2) {
  char16_t b[128];
  EXPECT_EQ(34, tprecode8to16(b, 128, "(╯°□°)╯︵L┻━┻  𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹"));
  EXPECT_STREQ(u"(╯°□°)╯︵L┻━┻  𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷𐌸𐌹", b);
}
