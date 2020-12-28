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
