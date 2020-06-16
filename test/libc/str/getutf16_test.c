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
#include "libc/str/tpdecode.h"
#include "libc/testlib/testlib.h"

TEST(getutf16, testNul) {
  wint_t wc;
  EXPECT_EQ(1, getutf16(u"", &wc));
  EXPECT_EQ('\0', wc);
  EXPECT_EQ(1, (getutf16)(u"", &wc));
  EXPECT_EQ('\0', wc);
}

TEST(getutf16, testBasic) {
  wint_t wc;
  EXPECT_EQ(1, getutf16(u"h", &wc));
  EXPECT_EQ('h', wc);
  EXPECT_EQ(1, (getutf16)(u"h", &wc));
  EXPECT_EQ('h', wc);
}

TEST(getutf16, testAegeanNumberSupplementaryPlane) {
  wint_t wc;
  EXPECT_EQ(4, tpdecode("𐄷", &wc));
  EXPECT_EQ(4, tpdecode("\xF0\x90\x84\xB7", &wc));
  EXPECT_EQ(0x10137, wc);
  EXPECT_EQ(2, strlen16(u"𐄷"));
  EXPECT_EQ(2, getutf16(u"𐄷", &wc));
  EXPECT_EQ(0x10137, wc);
  EXPECT_EQ(2, (getutf16)(u"𐄷", &wc));
  EXPECT_EQ(0x10137, wc);
}
