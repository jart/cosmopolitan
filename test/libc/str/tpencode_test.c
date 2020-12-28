/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/progn.internal.h"
#include "libc/bits/safemacros.h"
#include "libc/str/str.h"
#include "libc/str/tpencode.internal.h"
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
  ASSERT_EQ(3, tpencode(buf, 8, u'→', false));
  ASSERT_BINEQ(u"ΓåÆ", buf);
  ASSERT_EQ(3, (tpencode)(buf, 8, u'→', false));
  ASSERT_BINEQ(u"ΓåÆ", buf);
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

TEST(tpencode, testC1Csi) {
  ASSERT_BINEQ(u"┬¢", PROGN(ASSERT_EQ(2, tpencode(buf, 8, 0x9B, false)), buf));
}
