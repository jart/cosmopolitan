/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/str/unicode.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(wcwidth, test) {
  ASSERT_EQ(0, wcwidth(0));
  ASSERT_EQ(-1, wcwidth(1));
  ASSERT_EQ(-1, wcwidth(-7));
  ASSERT_EQ(1, wcwidth(0x10FFFD));
  ASSERT_EQ(-1, wcwidth(0x10FFFD + 1));
}

TEST(strwidth, testCjkWidesAndCombiningLowLines_withThompsonPikeEncoding) {
  /*───────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, strwidth(/**/ "𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, strwidth(/**/ "(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, strwidth(/**/ "ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(68, strlen(/*──*/ "𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(56, strlen(/*──*/ "(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(42, strlen(/*──*/ "ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*───────────────────────────────────────────────────┴─*/
}

TEST(strwidth16, testCjkWidesAndCombiningLowLines_lengthIsNotShorts) {
  /*──────────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, strwidth16(/**/ u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, strwidth16(/**/ u"(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, strwidth16(/**/ u"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(28, strlen16(/*──*/ u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(23, strlen16(/*──*/ u"(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, strlen16(/*──*/ u"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*──────────────────────────────────────────────────────┴─*/
}

TEST(wcwidth, testCjkWidesAndCombiningLowLines_widthIsNotLength) {
  /*────────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, wcswidth(/**/ L"Table flip▒▒▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, wcswidth(/**/ L"(╯°□°)╯︵ ̲┻̲━̲┻▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, wcswidth(/**/ L"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/, 0));
  EXPECT_EQ(20, wcslen(/*──*/ L"Table flip▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(22, wcslen(/*──*/ L"(╯°□°)╯︵ ̲┻̲━̲┻▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, wcslen(/*──*/ L"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*────────────────────────────────────────────────────┴─*/
}

TEST(wcwidth, block) {
  EXPECT_EQ(1, wcwidth(u'▄'));
  EXPECT_EQ(0x3061, L'ち');
  EXPECT_EQ(2, wcwidth(L'ち'));
}

TEST(strwidth, testTextDelimitingControlCodes_dontHaveSubstance) {
  EXPECT_EQ(0, strwidth("\0", 0));
  EXPECT_EQ(0, strwidth("\1", 0));
}

BENCH(wcwidth, bench) {
  EZBENCH2("wcwidth", donothing, __expropriate(wcwidth(__veil("r", u'→'))));
}
