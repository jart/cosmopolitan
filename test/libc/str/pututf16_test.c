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
#include "libc/str/oldutf16.internal.h"
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
