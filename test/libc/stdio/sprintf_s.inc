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

/* MODE=tiny makes these dependencies optional */
__static_yoink("strnwidth");
__static_yoink("strnwidth16");
__static_yoink("wcsnwidth");

TEST(SUITE(sprintf), testStringLength) {
  ASSERT_STREQ("This", Format(FORMAT("%.4"), STRING("This is a test")));
  ASSERT_STREQ("test", Format(FORMAT("%.4"), STRING("test")));
  ASSERT_STREQ("123", Format(FORMAT("%.7"), STRING("123")));
  ASSERT_STREQ("", Format(FORMAT("%.7"), STRING("")));
  ASSERT_STREQ("1234ab", Format(FORMAT("%.4") FORMAT("%.2"), STRING("123456"),
                                STRING("abcdef")));
  ASSERT_STREQ(FORMAT(".2"), Format(FORMAT("%.4.2"), STRING("123456")));
  ASSERT_STREQ("123", Format(FORMAT("%.*"), 3, STRING("123456")));
}

TEST(SUITE(sprintf), testCharacterCounting) {
  ASSERT_STREQ("         ♥♦♣♠☺☻▲", Format(FORMAT("%16"), STRING("♥♦♣♠☺☻▲")));
}

TEST(SUITE(snprintf), testTableFlip) {
  EXPECT_STREQ("Table flip          ", Format("%-20ls", L"Table flip"));
  EXPECT_STREQ("(╯°□°)╯︵L┻━┻       ", Format("%-20ls", L"(╯°□°)╯︵L┻━┻"));
  EXPECT_STREQ("(╯°□°)╯︵u┻━┻       ", Format("%-20hs", u"(╯°□°)╯︵u┻━┻"));
  EXPECT_STREQ("ちゃぶ台返し        ", Format("%-20ls", L"ちゃぶ台返し"));
  EXPECT_STREQ("       (╯°□°)╯︵L┻━┻", Format("%20ls", L"(╯°□°)╯︵L┻━┻"));
  EXPECT_STREQ("        ちゃぶ台返し", Format("%20ls", L"ちゃぶ台返し"));
}

TEST(SUITE(snprintf), testCombiningWidth) {
  EXPECT_STREQ("H̲E̲L̲L̲O̲     ",
               Format("%-10ls", L"H\u0332E\u0332L\u0332L\u0332O\u0332"));
  EXPECT_STREQ("     H̲E̲L̲L̲O̲",
               Format("%10hs", u"H\u0332E\u0332L\u0332L\u0332O\u0332"));
}

TEST(SUITE(snprintf), testQuoting) {
  EXPECT_STREQ("\\\"hi┻\\'━┻", Format("%'s", "\"hi┻'━┻"));
  EXPECT_STREQ(STRINGIFY("\"hi┻\'━┻"), Format("%`'s", "\"hi┻'━┻"));
  EXPECT_STREQ(STRINGIFY("\177\"hi┻\'━┻"), Format("%`'s", "\x7f\"hi┻'━┻"));
}

TEST(SUITE(snprintf), testBing_cString_stopsAtNulTerminator) {
  EXPECT_STREQ("♥♦♣♠", Format("%#s", "\3\4\5\6\0\3\4\5\6"));
}

TEST(SUITE(snprintf), testBing_precisionString_showsTrueBinary) {
  EXPECT_STREQ("♥♦♣♠ ♥♦♣♠", Format("%#.*s", 9, "\3\4\5\6\0\3\4\5\6"));
}

TEST(SUITE(snprintf), testStringPrecision_showsTrueBinary) {
  EXPECT_STREQ("\3\4\0", Format("%.*s", 3, "\3\4\0"));
}

TEST(SUITE(snprintf), testPrecision_usesByteCount) {
  EXPECT_STREQ("ちゃ", Format("%.*s", 6, "ちゃぶ台返し"));
}

TEST(SUITE(snprintf), testReprChar16) {
  EXPECT_STREQ("u'♥'", Format("%`'hc", u'♥'));
}

TEST(SUITE(snprintf), testReprChar32) {
  EXPECT_STREQ("L'♥'", Format("%`'lc", L'♥'));
}

TEST(SUITE(snprintf), testReprUtf8) {
  EXPECT_STREQ("\"♥\"", Format("%`'s", u8"♥"));
}

TEST(SUITE(snprintf), testReprUtf8Precision_countsBytes) {
  EXPECT_STREQ("\"♥\"", Format("%`'.*s", 3, u8"♥"));
}
