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

/* MODE=tiny makes these dependencies optional */
STATIC_YOINK("strnwidth");
STATIC_YOINK("strnwidth16");
STATIC_YOINK("wcsnwidth");

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
  EXPECT_STREQ("(╯°□°)╯︵ ┻━┻       ", Format("%-20ls", L"(╯°□°)╯︵ ┻━┻"));
  EXPECT_STREQ("ちゃぶ台返し        ", Format("%-20ls", L"ちゃぶ台返し"));
  EXPECT_STREQ("       (╯°□°)╯︵ ┻━┻", Format("%20ls", L"(╯°□°)╯︵ ┻━┻"));
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

TEST(SUITE(snprintf), testPrecision_usesCodepointCount) {
  EXPECT_STREQ("ちゃぶ台返し", Format("%.*s", 6, "ちゃぶ台返し"));
}
