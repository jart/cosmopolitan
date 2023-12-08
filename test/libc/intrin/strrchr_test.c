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
#include "libc/testlib/testlib.h"

TEST(strrchr, test) {
  EXPECT_EQ(NULL, strrchr("hello", 'z'));
  EXPECT_STREQ("lo", strrchr("hello", 'l'));
  EXPECT_STREQ("llo", strchr("hello", 'l'));
  EXPECT_STREQ("hello", strrchr("hello", 'h'));
  EXPECT_STREQ("ello", strrchr("hello", 'e'));
  EXPECT_STREQ("o", strrchr("hello", 'o'));
}

TEST(strrchr, simdVectorStuffIsntBroken) {
  EXPECT_EQ(NULL, strrchr("--------------------------------", 'x'));
  EXPECT_STREQ("x", strrchr("-------------------------------x", 'x'));
  EXPECT_STREQ("x-------------------------------",
               strrchr("x-------------------------------", 'x'));
  EXPECT_STREQ("x"
               "z-------------------------------",
               strrchr("x"
                       "z-------------------------------",
                       'x'));
  EXPECT_STREQ("x-------------------------------"
               "y-------------------------------",
               strrchr("x-------------------------------"
                       "y-------------------------------",
                       'x'));
  EXPECT_STREQ("x"
               "z-------------------------------"
               "y-------------------------------",
               strrchr("x"
                       "z-------------------------------"
                       "y-------------------------------",
                       'x'));
}

TEST(strrchr16, test) {
  EXPECT_EQ(NULL, strrchr16(u"hello", 'z'));
  EXPECT_STREQ(u"lo", strrchr16(u"hello", 'l'));
  EXPECT_STREQ(u"llo", strchr16(u"hello", 'l'));
  EXPECT_STREQ(u"hello", strrchr16(u"hello", 'h'));
  EXPECT_STREQ(u"ello", strrchr16(u"hello", 'e'));
  EXPECT_STREQ(u"o", strrchr16(u"hello", 'o'));
}

TEST(strrchr16, simdVectorStuffIsntBroken) {
  EXPECT_EQ(NULL, strrchr16(u"--------------------------------", 'x'));
  EXPECT_STREQ(u"x", strrchr16(u"-------------------------------x", 'x'));
  EXPECT_STREQ(u"x-------------------------------",
               strrchr16(u"x-------------------------------", 'x'));
  EXPECT_STREQ(u"x"
               u"z-------------------------------",
               strrchr16(u"x"
                         u"z-------------------------------",
                         'x'));
  EXPECT_STREQ(u"x-------------------------------"
               u"y-------------------------------",
               strrchr16(u"x-------------------------------"
                         u"y-------------------------------",
                         'x'));
  EXPECT_STREQ(u"x"
               u"z-------------------------------"
               u"y-------------------------------",
               strrchr16(u"x"
                         u"z-------------------------------"
                         u"y-------------------------------",
                         'x'));
}

TEST(wcsrchr, test) {
  EXPECT_EQ(NULL, wcsrchr(L"hello", 'z'));
  EXPECT_STREQ(L"lo", wcsrchr(L"hello", 'l'));
  EXPECT_STREQ(L"llo", wcschr(L"hello", 'l'));
  EXPECT_STREQ(L"hello", wcsrchr(L"hello", 'h'));
  EXPECT_STREQ(L"ello", wcsrchr(L"hello", 'e'));
  EXPECT_STREQ(L"o", wcsrchr(L"hello", 'o'));
}

TEST(wcsrchr, simdVectorStuffIsntBroken) {
  EXPECT_EQ(NULL, wcsrchr(L"--------------------------------", 'x'));
  EXPECT_STREQ(L"x", wcsrchr(L"-------------------------------x", 'x'));
  EXPECT_STREQ(L"x-------------------------------",
               wcsrchr(L"x-------------------------------", 'x'));
  EXPECT_STREQ(L"x"
               L"z-------------------------------",
               wcsrchr(L"x"
                       L"z-------------------------------",
                       'x'));
  EXPECT_STREQ(L"x-------------------------------"
               L"y-------------------------------",
               wcsrchr(L"x-------------------------------"
                       L"y-------------------------------",
                       'x'));
  EXPECT_STREQ(L"x"
               L"z-------------------------------"
               L"y-------------------------------",
               wcsrchr(L"x"
                       L"z-------------------------------"
                       L"y-------------------------------",
                       'x'));
}
