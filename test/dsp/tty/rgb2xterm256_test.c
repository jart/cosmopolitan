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
#include "dsp/tty/rgb2xterm256.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(rgb2xterm256, test) {
  EXPECT_EQ(196, rgb2xterm256v2(0xff, 0x00, 0x00)); /* red */
  EXPECT_EQ(46, rgb2xterm256v2(0x00, 0xff, 0x00));  /* green */
  EXPECT_EQ(21, rgb2xterm256v2(0x00, 0x00, 0xff));  /* blue */
  EXPECT_EQ(226, rgb2xterm256v2(0xff, 0xff, 0x00)); /* yellow */
  EXPECT_EQ(208, rgb2xterm256v2(0xff, 0x80, 0x00)); /* orange */
}

TEST(rgb2xterm256, testRedBlack) {
  EXPECT_EQ(16, rgb2xterm256v2(0, 0, 0));
  EXPECT_EQ(16, rgb2xterm256v2(12, 0, 0));
  EXPECT_EQ(232, rgb2xterm256v2(13, 0, 0));
  EXPECT_EQ(233, rgb2xterm256v2(39, 0, 0));
  EXPECT_EQ(233, rgb2xterm256v2(40, 0, 0));
  EXPECT_EQ(52, rgb2xterm256v2(53, 0, 0));
  EXPECT_EQ(88, rgb2xterm256v2(115, 0, 0));
  EXPECT_EQ(88, rgb2xterm256v2(116, 0, 0));
}

////////////////////////////////////////////////////////////////////////////////

BENCH(rgb2xterm256v2, bench) {
  EZBENCH(donothing, rgb2xterm256v2(0xff, 0x80, 0x00));
}
