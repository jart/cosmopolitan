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
#include "libc/runtime/gc.h"
#include "libc/calls/internal.h"
#include "libc/testlib/testlib.h"

char16_t p[PATH_MAX];

TEST(mkntpath, testEmpty) {
  EXPECT_EQ(0, mkntpath("", p));
  EXPECT_STREQ(u"", p);
}

TEST(mkntpath, testSlashes) {
  /*
   * The Windows command prompt works fine with all reasonable
   * unix-style paths. There only seems to be one exception, and that's
   * all it takes to make the feature entirely useless to us, similar to
   * the law of noncontradiction. We address the issue as follows:
   */
  EXPECT_EQ(9, mkntpath("o/foo.com", p));
  EXPECT_STREQ(u"o\\foo.com", p);
}

TEST(mkntpath, testUnicode) {
  EXPECT_EQ(20, mkntpath("C:\\𐌰𐌱𐌲𐌳\\𐌴𐌵𐌶𐌷", p));
  EXPECT_STREQ(u"C:\\𐌰𐌱𐌲𐌳\\𐌴𐌵𐌶𐌷", p);
}
