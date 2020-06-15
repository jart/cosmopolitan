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
#include "libc/bits/progn.h"
#include "libc/bits/safemacros.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

char buf[8];

TEST(tpencode, testNul) {
  ASSERT_BINEQ(u" ", PROGN(ASSERT_EQ(1, tpencode(buf, 8, 0, false)), buf));
  ASSERT_BINEQ(u" ", PROGN(ASSERT_EQ(1, (tpencode)(buf, 8, 0, false)), buf));
}

TEST(tpencode, testSpace) {
  ASSERT_BINEQ(u" ", PROGN(ASSERT_EQ(1, tpencode(buf, 8, 0x20, false)), buf));
  ASSERT_BINEQ(u" ", PROGN(ASSERT_EQ(1, (tpencode)(buf, 8, 0x20, false)), buf));
}

TEST(tpencode, testGlyph) {
  ASSERT_BINEQ(u"ΓåÆ", PROGN(ASSERT_EQ(3, tpencode(buf, 8, u'→', false)), buf));
  ASSERT_BINEQ(u"ΓåÆ",
               PROGN(ASSERT_EQ(3, (tpencode)(buf, 8, u'→', false)), buf));
}

TEST(tpencode, testMathematicalNotMuhPolicyDrivenBehavior_negativeOne) {
  ASSERT_BINEQ(u"λ┐┐┐┐┐",
               PROGN(ASSERT_EQ(6, tpencode(buf, 8, -1, false)), buf));
  ASSERT_BINEQ(u"λ┐┐┐┐┐",
               PROGN(ASSERT_EQ(6, (tpencode)(buf, 8, -1, false)), buf));
}

TEST(tpencode, testMathematicalNotMuhPolicyDrivenBehavior_twosComplementBane) {
  ASSERT_BINEQ(u"■ÇÇÇÇÇ",
               PROGN(ASSERT_EQ(6, tpencode(buf, 8, 0x80000000, false)), buf));
  ASSERT_BINEQ(u"■ÇÇÇÇÇ",
               PROGN(ASSERT_EQ(6, (tpencode)(buf, 8, 0x80000000, false)), buf));
}

TEST(tpencode, testMathematicalNotMuhPolicyDrivenBehavior_nonCanonicalNul) {
  ASSERT_BINEQ(u"└Ç", PROGN(ASSERT_EQ(2, tpencode(buf, 8, 0, true)), buf));
  ASSERT_BINEQ(u"└Ç", PROGN(ASSERT_EQ(2, (tpencode)(buf, 8, 0, true)), buf));
}
