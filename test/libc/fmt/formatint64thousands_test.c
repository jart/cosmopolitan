/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(FormatUint64Thousands, test) {
  char s[27];
  EXPECT_EQ(s + 1, FormatUint64Thousands(s, 0));
  EXPECT_STREQ("0", s);
  EXPECT_EQ(s + 1, FormatUint64Thousands(s, 1));
  EXPECT_STREQ("1", s);
  EXPECT_EQ(s + 3, FormatUint64Thousands(s, 999));
  EXPECT_STREQ("999", s);
  EXPECT_EQ(s + 5, FormatUint64Thousands(s, 1000));
  EXPECT_STREQ("1,000", s);
  EXPECT_EQ(s + 7, FormatUint64Thousands(s, 999999));
  EXPECT_STREQ("999,999", s);
  EXPECT_EQ(s + 9, FormatUint64Thousands(s, 1000000));
  EXPECT_STREQ("1,000,000", s);
  EXPECT_EQ(s + 7, FormatUint64Thousands(s, 104040));
  EXPECT_STREQ("104,040", s);
}

TEST(FormatInt64Thousands, testPositive) {
  char s[27];
  EXPECT_EQ(s + 1, FormatInt64Thousands(s, 0));
  EXPECT_STREQ("0", s);
  EXPECT_EQ(s + 1, FormatInt64Thousands(s, 1));
  EXPECT_STREQ("1", s);
  EXPECT_EQ(s + 3, FormatInt64Thousands(s, 999));
  EXPECT_STREQ("999", s);
  EXPECT_EQ(s + 5, FormatInt64Thousands(s, 1000));
  EXPECT_STREQ("1,000", s);
  EXPECT_EQ(s + 7, FormatInt64Thousands(s, 999999));
  EXPECT_STREQ("999,999", s);
  EXPECT_EQ(s + 9, FormatInt64Thousands(s, 1000000));
  EXPECT_STREQ("1,000,000", s);
}

TEST(FormatInt64Thousands, testNegative) {
  char s[27];
  EXPECT_EQ(s + 2, FormatInt64Thousands(s, -1));
  EXPECT_STREQ("-1", s);
  EXPECT_EQ(s + 4, FormatInt64Thousands(s, -999));
  EXPECT_STREQ("-999", s);
  EXPECT_EQ(s + 6, FormatInt64Thousands(s, -1000));
  EXPECT_STREQ("-1,000", s);
  EXPECT_EQ(s + 8, FormatInt64Thousands(s, -999999));
  EXPECT_STREQ("-999,999", s);
  EXPECT_EQ(s + 10, FormatInt64Thousands(s, -1000000));
  EXPECT_STREQ("-1,000,000", s);
}

BENCH(FormatInt64Thousands, bench) {
  char s[27];
  EZBENCH2("FormatInt64Thousands(MAX)", donothing,
           FormatInt64Thousands(s, INT64_MAX));
  EZBENCH2("FormatInt64Thousands(MIN)", donothing,
           FormatInt64Thousands(s, INT64_MIN));
  EZBENCH2("FormatUint64Thousands(MIN)", donothing,
           FormatUint64Thousands(s, UINT64_MAX));
}
