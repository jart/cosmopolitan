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
#include "libc/unicode/unicode.h"

unsigned n;
size_t size;
wchar_t *str;
char16_t *b, *buf;

TEST(pututf16, testEmpty) {
  EXPECT_EQ(0, pututf16(NULL, 0, u'j', false));
  EXPECT_EQ(0, (pututf16)(NULL, 0, u'j', false));
}

TEST(pututf16, testNul) {
  size = 1;
  buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(1, pututf16(buf, size, u'\0', false));
  EXPECT_EQ(u'\0', buf[0]);
  buf[0] = '\7';
  EXPECT_EQ(1, (pututf16)(buf, size, u'\0', false));
  EXPECT_EQ(u'\0', buf[0]);
  tfree(buf);
}

TEST(pututf16, testAscii) {
  size = 1;
  buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(1, pututf16(buf, size, u'j', false));
  EXPECT_EQ(u'j', buf[0]);
  EXPECT_EQ(1, (pututf16)(buf, size, u't', false));
  EXPECT_EQ(u't', buf[0]);
  tfree(buf);
}

TEST(pututf16, testGothicSupplementaryPlane) {
  size = 2;
  buf = tmalloc(size * sizeof(char16_t));
  EXPECT_EQ(2, pututf16(buf, size, L'𐌰', false));
  EXPECT_STREQN(u"𐌰", buf, 1);
  EXPECT_EQ(2, (pututf16)(buf, size, L'𐌱', false));
  EXPECT_STREQN(u"𐌱", buf, 1);
  tfree(buf);
}

TEST(pututf16, testEmojiAndEmojiPresentationModifier_areBothInAstralPlanes) {
  n = 8;
  b = tgc(tmalloc(sizeof(char16_t) * n));
  str = L"\U0001F466\U0001F3FF";
  memset(b, 0, n * sizeof(char16_t));
  EXPECT_EQ(2, pututf16(b, n, str[0], false));
  EXPECT_BINEQ(u"=╪f▄    ", b);
  memset(b, 0, n * sizeof(char16_t));
  EXPECT_EQ(2, pututf16(b, n, str[1], false));
  EXPECT_BINEQ(u"<╪λ▀    ", b);
}
