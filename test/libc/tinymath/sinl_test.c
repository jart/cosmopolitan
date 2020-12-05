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
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char buf[32];

TEST(sinl, testLongDouble) {
  EXPECT_STREQ(".479425538604203", gc(xdtoa(sinl(.5))));
  EXPECT_STREQ("-.479425538604203", gc(xdtoa(sinl(-.5))));
}

TEST(sinl, testDouble) {
  EXPECT_STREQ(".479425538604203", gc(xdtoa(sin(.5))));
  EXPECT_STREQ("-.479425538604203", gc(xdtoa(sin(-.5))));
}

TEST(sinl, testFloat) {
  EXPECT_STARTSWITH(".4794255", gc(xdtoa(sinf(.5f))));
  EXPECT_STARTSWITH("-.4794255", gc(xdtoa(sinf(-.5f))));
}

BENCH(sinl, bench) {
  EZBENCH(donothing, sinl(0.7));  /* ~30ns */
  EZBENCH(donothing, sin(0.7));   /* ~35ns */
  EZBENCH(donothing, sinf(0.7f)); /* ~35ns */
}
