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
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/errno.h"
#include "libc/fmt/bing.internal.h"
#include "libc/limits.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/str/tpdecode.internal.h"
#include "libc/testlib/testlib.h"

wint_t wc;

TEST(tpdecode, testEmptyString_consumesNulTerminator) {
  wc = 123;
  EXPECT_EQ(1, tpdecode("", &wc));
  EXPECT_EQ(0, wc);
}

TEST(tpdecode, testGlyph) {
  EXPECT_EQ(u'→', PROGN(ASSERT_EQ(3, tpdecode("→", &wc)), wc));
  EXPECT_EQ(L'𐌰', PROGN(ASSERT_EQ(4, tpdecode("𐌰𐌱𐌲𐌳", &wc)), wc));
  EXPECT_EQ(u'ち', PROGN(ASSERT_EQ(3, tpdecode("ち", &wc)), wc));
  EXPECT_EQ(u'‱', PROGN(ASSERT_EQ(3, tpdecode("‱", &wc)), wc));
}

TEST(tpdecode, testNul_canonicalizesWithFiniteOverlongConsumption) {
  EXPECT_EQ('\0', PROGN(ASSERT_EQ(1, tpdecode("\0\0\0\0", &wc)), wc));
  EXPECT_EQ('\0',
            PROGN(ASSERT_EQ(2, tpdecode(gc(unbingstr(u"└ÇÇÇÇÇ")), &wc)), wc));
  EXPECT_EQ('\0',
            PROGN(ASSERT_EQ(3, tpdecode(gc(unbingstr(u"αÇÇÇÇÇ")), &wc)), wc));
  EXPECT_EQ('\0',
            PROGN(ASSERT_EQ(4, tpdecode(gc(unbingstr(u"≡ÇÇÇÇÇ")), &wc)), wc));
  EXPECT_EQ('\0',
            PROGN(ASSERT_EQ(5, tpdecode(gc(unbingstr(u"°ÇÇÇÇÇ")), &wc)), wc));
  EXPECT_EQ('\0',
            PROGN(ASSERT_EQ(6, tpdecode(gc(unbingstr(u"ⁿÇÇÇÇÇ")), &wc)), wc));
}

TEST(tpdecode, testSynchronization_skipsLeadingContinuations) {
  EXPECT_EQ('a',
            PROGN(EXPECT_EQ(7, tpdecode(gc(unbingstr(u"Ç╗╝╜╛┐a")), &wc)), wc));
}

TEST(tpdecode, testSpace) {
  EXPECT_EQ(0x20, PROGN(ASSERT_EQ(1, tpdecode(" ", &wc)), wc));
}

TEST(tpdecode, testNegativeNumbers) {
  EXPECT_EQ(-1, PROGN(ASSERT_EQ(6, tpdecode(gc(unbingstr(u"λ┐┐┐┐┐")), &wc)),
                      (wchar_t)wc));
  EXPECT_EQ(INT_MIN,
            PROGN(ASSERT_EQ(6, tpdecode(gc(unbingstr(u"■ÇÇÇÇÇ")), &wc)),
                  (wchar_t)wc));
  EXPECT_EQ(0x80000000u,
            PROGN(ASSERT_EQ(6, tpdecode(gc(unbingstr(u"■ÇÇÇÇÇ")), &wc)), wc));
  EXPECT_EQ(0xC0000000u,
            PROGN(ASSERT_EQ(6, tpdecode(gc(unbingstr(u"λÇÇÇÇÇ")), &wc)), wc));
}

TEST(tpdecode, testInvalidEncoding_endOfString) {
  EXPECT_EQ(u'�', PROGN(EXPECT_EQ(-1, tpdecode(gc(unbingstr(u"≡")), &wc)), wc));
}

TEST(tpdecode, testInvalidEncoding_tooFewContinuations) {
  EXPECT_EQ(u'�', PROGN(EXPECT_EQ(-1, tpdecode(gc(unbingstr(u"≡")), &wc)), wc));
}
