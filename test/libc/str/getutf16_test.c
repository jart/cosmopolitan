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
#include "libc/str/tpdecode.internal.h"
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
