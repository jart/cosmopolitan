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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"

TEST(sidiv, smoke) {
  EXPECT_EQ(13373133731337 / 10, div10int64(13373133731337));
  EXPECT_EQ(13373133731337 / 100, div100int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000, div1000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 10000, div10000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000000, div1000000int64(13373133731337));
  EXPECT_EQ(13373133731337 / 1000000000, div1000000000int64(13373133731337));
}

TEST(sirem, smoke) {
  EXPECT_EQ(13373133731337 % 10, rem10int64(13373133731337));
  EXPECT_EQ(13373133731337 % 100, rem100int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000, rem1000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 10000, rem10000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000000, rem1000000int64(13373133731337));
  EXPECT_EQ(13373133731337 % 1000000000, rem1000000000int64(13373133731337));
}

TEST(rem, euclid) {
  ASSERT_EQ(-2, rem10int64(-12));
  ASSERT_EQ(-1, rem10int64(-1));
  ASSERT_EQ(0, rem10int64(0));
  ASSERT_EQ(1, rem10int64(1));
  ASSERT_EQ(9, rem10int64(9));
  ASSERT_EQ(1, rem10int64(11));
}
