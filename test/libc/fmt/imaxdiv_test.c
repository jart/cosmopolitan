/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Hugues Morisset                                               │
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
#include "libc/limits.h"
#include "libc/testlib/testlib.h"

TEST(imaxdiv, test) {
  imaxdiv_t res;

  res = imaxdiv(-5, 3);
  EXPECT_EQ(-1, res.quot);
  EXPECT_EQ(-2, res.rem);

  res = imaxdiv(1, 1);
  EXPECT_EQ(1, res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(20, 10);
  EXPECT_EQ(2, res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(20, 11);
  EXPECT_EQ(1, res.quot);
  EXPECT_EQ(9, res.rem);

  res = imaxdiv(-20, -11);
  EXPECT_EQ(1, res.quot);
  EXPECT_EQ(-9, res.rem);

  res = imaxdiv(0, -11);
  EXPECT_EQ(0, res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(0, 11);
  EXPECT_EQ(0, res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(INT64_MIN, 1);
  EXPECT_EQ(INT64_MIN, res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(INT64_MAX, -1);
  EXPECT_EQ(-(INT64_MAX), res.quot);
  EXPECT_EQ(0, res.rem);

  res = imaxdiv(2387, 348);
  EXPECT_EQ(6, res.quot);
  EXPECT_EQ(299, res.rem);
}
