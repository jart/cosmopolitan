/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Gabriel Ravier                                                │
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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(snprintf, testVeryLargePrecision) {
  char buf[512] = {};
  int i = snprintf(buf, sizeof(buf), "%.9999u", 10);

  ASSERT_EQ(i, 9999);
  ASSERT_EQ(strlen(buf), 511);
}

TEST(snprintf, testPlusFlagOnChar) {
  char buf[10] = {};
  int i = snprintf(buf, sizeof(buf), "%+c", '=');

  ASSERT_EQ(i, 1);
  ASSERT_STREQ(buf, "=");
}

TEST(snprintf, testStringWidth) {
  char buf[100] = {};
  int i = snprintf(buf, sizeof(buf), "<%9ls>", L"éée");

  ASSERT_EQ(i, 11);
  ASSERT_STREQ(buf, "<    éée>");

  i = snprintf(buf, sizeof(buf), "<%9s>", "éée");
  ASSERT_EQ(i, 11);
  ASSERT_STREQ(buf, "<    éée>");

  i = snprintf(buf, sizeof(buf), "<%9hs>", u"éée");
  ASSERT_EQ(i, 11);
  ASSERT_STREQ(buf, "<    éée>");
}

TEST(snprintf, testStringPrecision) {
  char buf[100] = {};
  int i = snprintf(buf, sizeof(buf), "%.4ls", L"eeée");

  ASSERT_EQ(i, 4);
  ASSERT_STREQ(buf, "eeé");

  i = snprintf(buf, sizeof(buf), "%.4s", "eeée");
  ASSERT_EQ(i, 4);
  ASSERT_STREQ(buf, "eeé");

  i = snprintf(buf, sizeof(buf), "%.4hs", u"eeée");
  ASSERT_EQ(i, 4);
  ASSERT_STREQ(buf, "eeé");
}

TEST(snprintf, testStringPrecisionPartialChar) {
  char buf[100] = {};
  int i = snprintf(buf, sizeof(buf), "%.4ls", L"eéée");

  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "eé");

  // Note that partial multibyte characters are fine on non-wide conversions
  // (whereas they are not written on wide conversions,
  // according to the standard)
  i = snprintf(buf, sizeof(buf), "%.4s", "eéée");
  ASSERT_EQ(i, 4);
  ASSERT_STREQ(buf, "eé\xc3");

  i = snprintf(buf, sizeof(buf), "%.4hs", u"eéée");
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "eé");
}

TEST(snprintf, testWideChar) {
  char buf[100] = { 'a', 'b', 'c' };
  int i = snprintf(buf, sizeof(buf), "%lc", L'\0');

  ASSERT_EQ(i, 1);
  ASSERT_EQ(buf[0], '\0');
  ASSERT_EQ(buf[1], '\0');
  ASSERT_EQ(buf[2], 'c');

  buf[0] = 'a';
  buf[1] = 'b';
  i = snprintf(buf, sizeof(buf), "%hc", u'\0');

  ASSERT_EQ(i, 1);
  ASSERT_EQ(buf[0], '\0');
  ASSERT_EQ(buf[1], '\0');
  ASSERT_EQ(buf[2], 'c');

  i = snprintf(buf, sizeof(buf), "%lc", L'þ');
  ASSERT_EQ(i, 2);
  ASSERT_STREQ(buf, "þ");

  i = snprintf(buf, sizeof(buf), "%hc", u'þ');
  ASSERT_EQ(i, 2);
  ASSERT_STREQ(buf, "þ");
}
