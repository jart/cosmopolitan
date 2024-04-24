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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"

TEST(zleb64, testZero) {
  int64_t x;
  char *p = gc(malloc(10));
  EXPECT_EQ(1, zleb64(p, 0) - p);
  EXPECT_EQ(0, p[0] & 255);
  EXPECT_EQ(1, unzleb64(p, 10, &x));
  EXPECT_EQ(0, x);
}

TEST(zleb64, testNegOne) {
  int64_t x;
  char *p = gc(malloc(10));
  EXPECT_EQ(1, zleb64(p, -1) - p);
  EXPECT_EQ(1, p[0] & 255);
  EXPECT_EQ(1, unzleb64(p, 10, &x));
  EXPECT_EQ(-1, x);
}

TEST(zleb64, testOne) {
  int64_t x;
  char *p = gc(malloc(10));
  EXPECT_EQ(1, zleb64(p, 1) - p);
  EXPECT_EQ(2, p[0] & 255);
  EXPECT_EQ(1, unzleb64(p, 10, &x));
  EXPECT_EQ(1, x);
}

TEST(zleb64, testMax) {
  int64_t x;
  char *p = gc(malloc(10));
  EXPECT_EQ(10, zleb64(p, INT64_MAX) - p);
  EXPECT_EQ(254, p[0] & 255);
  EXPECT_EQ(255, p[1] & 255);
  EXPECT_EQ(255, p[2] & 255);
  EXPECT_EQ(255, p[3] & 255);
  EXPECT_EQ(255, p[4] & 255);
  EXPECT_EQ(255, p[5] & 255);
  EXPECT_EQ(255, p[6] & 255);
  EXPECT_EQ(255, p[7] & 255);
  EXPECT_EQ(255, p[8] & 255);
  EXPECT_EQ(1, p[9] & 255);
  EXPECT_EQ(10, unzleb64(p, 10, &x));
  EXPECT_EQ(INT64_MAX, x);
}
