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

TEST(snprintf, testInf) {
  char buf[10] = {};
  int i = snprintf(buf, sizeof(buf), "%f", 1.0 / 0.0);

  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  memset(buf, 0, 4);
  i = snprintf(buf, sizeof(buf), "%Lf", 1.0L / 0.0L);
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  memset(buf, 0, 4);
  i = snprintf(buf, sizeof(buf), "%e", 1.0 / 0.0);
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  memset(buf, 0, 4);
  i = snprintf(buf, sizeof(buf), "%Le", 1.0L / 0.0L);
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  memset(buf, 0, 4);
  i = snprintf(buf, sizeof(buf), "%g", 1.0 / 0.0);
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  memset(buf, 0, 4);
  i = snprintf(buf, sizeof(buf), "%Lg", 1.0L / 0.0L);
  ASSERT_EQ(i, 3);
  ASSERT_STREQ(buf, "inf");

  for (i = 4; i < 10; ++i)
    ASSERT_EQ(buf[i], '\0');
}
