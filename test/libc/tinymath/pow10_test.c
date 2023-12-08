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
#include "libc/math.h"
#include "libc/testlib/testlib.h"

TEST(pow10, testLdbl) {
  EXPECT_LDBL_EQ(1, pow10l(0));
  EXPECT_LDBL_EQ(10, pow10l(1));
  EXPECT_LDBL_EQ(100, pow10l(2));
}

TEST(pow10, testDouble) {
  EXPECT_DOUBLE_EQ(1, pow10(0));
  EXPECT_DOUBLE_EQ(10, pow10(1));
  EXPECT_DOUBLE_EQ(100, pow10(2));
}

TEST(pow10, testFloat) {
  EXPECT_FLOAT_EQ(1, pow10f(0));
  EXPECT_FLOAT_EQ(10, pow10f(1));
  EXPECT_FLOAT_EQ(100, pow10f(2));
}
