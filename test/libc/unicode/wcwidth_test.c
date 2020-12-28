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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/unicode/unicode.h"

TEST(strwidth, testCjkWidesAndCombiningLowLines_withThompsonPikeEncoding) {
  /*───────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, strwidth(/**/ "𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strwidth(/**/ "(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strwidth(/**/ "ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strclen(/*─*/ "𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(22, strclen(/*─*/ "(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, strclen(/*─*/ "ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(68, strlen(/*──*/ "𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(56, strlen(/*──*/ "(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(42, strlen(/*──*/ "ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*───────────────────────────────────────────────────┴─*/
}

TEST(strwidth16, testCjkWidesAndCombiningLowLines_lengthIsNotShorts) {
  /*──────────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, strwidth16(/**/ u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strwidth16(/**/ u"(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strwidth16(/**/ u"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, strclen16(/*─*/ u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(22, strclen16(/*─*/ u"(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, strclen16(/*─*/ u"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(28, strlen16(/*──*/ u"𐌰𐌱𐌲𐌳𐌴𐌵𐌶𐌷▒▒▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(23, strlen16(/*──*/ u"(╯°□°)╯𐄻︵ ̲┻̲━̲┻▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, strlen16(/*──*/ u"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*──────────────────────────────────────────────────────┴─*/
}

TEST(wcwidth, testCjkWidesAndCombiningLowLines_widthIsNotLength) {
  /*────────────────────────────────────────────────────┬─*/
  EXPECT_EQ(20, wcswidth(/**/ L"Table flip▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, wcswidth(/**/ L"(╯°□°)╯︵ ̲┻̲━̲┻▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, wcswidth(/**/ L"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(20, wcslen(/*──*/ L"Table flip▒▒▒▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(22, wcslen(/*──*/ L"(╯°□°)╯︵ ̲┻̲━̲┻▒▒▒▒▒▒▒" /*│*/));
  EXPECT_EQ(14, wcslen(/*──*/ L"ちゃぶ台返し▒▒▒▒▒▒▒▒" /*│*/));
  /*────────────────────────────────────────────────────┴─*/
}

TEST(wcwidth, block) {
  EXPECT_EQ(1, wcwidth(u'▄'));
}

TEST(strwidth, testTextDelimitingControlCodes_dontHaveSubstance) {
  EXPECT_EQ(0, strwidth("\0"));
  EXPECT_EQ(0, strwidth("\1"));
}

BENCH(wcwidth, bench) {
  volatile int x;
  EZBENCH2("wcwidth", donothing, x = wcwidth(VEIL("r", u'→')));
}
