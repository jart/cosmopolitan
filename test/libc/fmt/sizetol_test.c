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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/testlib/testlib.h"

TEST(sizetol, test) {
  EXPECT_EQ(-1, sizetol("", 1000));
  EXPECT_EQ(0, sizetol("0", 1000));
  EXPECT_EQ(0, sizetol("0kb", 1000));
  EXPECT_EQ(31337, sizetol("31337", 1000));
  EXPECT_EQ(+123, sizetol("+123", 1000));
  EXPECT_EQ(-123, sizetol("-123", 1000));
  EXPECT_EQ(1000, sizetol("1kb", 1000));
  EXPECT_EQ(-1000, sizetol("-1kb", 1000));
  EXPECT_EQ(2000, sizetol("2k", 1000));
  EXPECT_EQ(1024, sizetol("1kb", 1024));
  EXPECT_EQ(2048, sizetol("2k", 1024));
  EXPECT_EQ(100000000, sizetol("100mb", 1000));
  EXPECT_EQ(100000000000000, sizetol("100tb", 1000));
  EXPECT_EQ(104857600, sizetol("100mb", 1024));
  EXPECT_EQ(100000000, sizetol("100MB", 1000));
  EXPECT_EQ(104857600, sizetol("100MB", 1024));
  EXPECT_EQ(100000000, sizetol("100M", 1000));
  EXPECT_EQ(104857600, sizetol("100M", 1024));
  EXPECT_EQ(100000000, sizetol(" 100M", 1000));
  EXPECT_EQ(104857600, sizetol("\t100M", 1024));
  EXPECT_EQ(100, sizetol(" 100", 1000));
  EXPECT_EQ(100, sizetol("\t100", 1024));
  EXPECT_EQ(100, sizetol(" 100 ", 1000));
  EXPECT_EQ(100, sizetol("\t100\t", 1024));
}

TEST(sizetol, testOverflow_isDetected) {
  EXPECT_EQ(9000000000000000000, sizetol("9eb", 1000));
  EXPECT_SYS(EOVERFLOW, -1, sizetol("10eb", 1000));
}

TEST(sizetol, badSyntax) {
  EXPECT_SYS(EINVAL, -1, sizetol("- 1", 1000));
  EXPECT_SYS(EINVAL, -1, sizetol("- 1", 1000));
  EXPECT_SYS(EOVERFLOW, -1, sizetol("10eb", 1000));
}
