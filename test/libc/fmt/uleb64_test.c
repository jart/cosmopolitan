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
#include "libc/fmt/leb128.h"
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

char p[10];
uint64_t x;

TEST(uleb64, testZero) {
  EXPECT_EQ(1, uleb64(p, 0) - p);
  EXPECT_EQ(0, p[0]);
  EXPECT_EQ(1, unuleb64(p, 10, &x));
  EXPECT_EQ(0, x);
  EXPECT_EQ(-1, unuleb64(p, 0, &x));
}

TEST(uleb64, testOne) {
  EXPECT_EQ(1, uleb64(p, 1) - p);
  EXPECT_EQ(1, p[0]);
  EXPECT_EQ(1, unuleb64(p, 10, &x));
  EXPECT_EQ(1, x);
}

TEST(uleb64, test255) {
  EXPECT_EQ(2, uleb64(p, 255) - p);
  EXPECT_EQ(255, p[0] & 255);
  EXPECT_EQ(1, p[1]);
  EXPECT_EQ(2, unuleb64(p, 10, &x));
  EXPECT_EQ(255, x);
}

TEST(uleb64, testFFFF) {
  EXPECT_EQ(3, uleb64(p, 0xFFFF) - p);
  EXPECT_EQ(255, p[0] & 255);
  EXPECT_EQ(255, p[1] & 255);
  EXPECT_EQ(3, p[2] & 255);
}

TEST(uleb64, testMax) {
  EXPECT_EQ(10, uleb64(p, UINT64_MAX) - p);
  EXPECT_EQ(255, p[0x00] & 255);
  EXPECT_EQ(255, p[0x01] & 255);
  EXPECT_EQ(255, p[0x02] & 255);
  EXPECT_EQ(255, p[0x03] & 255);
  EXPECT_EQ(255, p[0x04] & 255);
  EXPECT_EQ(255, p[0x05] & 255);
  EXPECT_EQ(255, p[0x06] & 255);
  EXPECT_EQ(255, p[0x07] & 255);
  EXPECT_EQ(255, p[0x08] & 255);
  EXPECT_EQ(001, p[0x09] & 255);
  EXPECT_EQ(10, unuleb64(p, 10, &x));
  EXPECT_EQ(UINT64_MAX, x);
  EXPECT_EQ(-1, unuleb64(p, 7, &x));
}
