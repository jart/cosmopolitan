/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/fmt/fmt.h"
#include "libc/fmt/leb128.h"
#include "libc/limits.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

char p[19];

TEST(sleb128, testZero) {
  EXPECT_EQ(1, sleb128(p, 0) - p);
  EXPECT_EQ(0, p[0]);
}

TEST(sleb128, testOne) {
  EXPECT_EQ(1, sleb128(p, 1) - p);
  EXPECT_EQ(1, p[0]);
}

TEST(sleb128, test255) {
  EXPECT_EQ(2, sleb128(p, 255) - p);
  EXPECT_EQ(255, p[0] & 255);
  EXPECT_EQ(1, p[1]);
}

TEST(sleb128, testNeg1) {
  EXPECT_EQ(1, sleb128(p, -1) - p);
  EXPECT_EQ(127, p[0]);
}

TEST(sleb128, testNeg255) {
  EXPECT_EQ(2, sleb128(p, -255) - p);
  EXPECT_EQ(129, p[0] & 255);
  EXPECT_EQ(126, p[1]);
}

TEST(sleb128, testMax) {
  EXPECT_EQ(19, sleb128(p, INT128_MAX) - p);
  EXPECT_EQ(255, p[0x00] & 255);
  EXPECT_EQ(255, p[0x01] & 255);
  EXPECT_EQ(255, p[0x02] & 255);
  EXPECT_EQ(255, p[0x03] & 255);
  EXPECT_EQ(255, p[0x04] & 255);
  EXPECT_EQ(255, p[0x05] & 255);
  EXPECT_EQ(255, p[0x06] & 255);
  EXPECT_EQ(255, p[0x07] & 255);
  EXPECT_EQ(255, p[0x08] & 255);
  EXPECT_EQ(255, p[0x09] & 255);
  EXPECT_EQ(255, p[0x0a] & 255);
  EXPECT_EQ(255, p[0x0b] & 255);
  EXPECT_EQ(255, p[0x0c] & 255);
  EXPECT_EQ(255, p[0x0d] & 255);
  EXPECT_EQ(255, p[0x0e] & 255);
  EXPECT_EQ(255, p[0x0f] & 255);
  EXPECT_EQ(255, p[0x10] & 255);
  EXPECT_EQ(255, p[0x11] & 255);
  EXPECT_EQ(001, p[0x12] & 255);
}

TEST(sleb128, testMin) {
  EXPECT_EQ(19, sleb128(p, INT128_MIN) - p);
  EXPECT_EQ(128, p[0x00] & 255);
  EXPECT_EQ(128, p[0x01] & 255);
  EXPECT_EQ(128, p[0x02] & 255);
  EXPECT_EQ(128, p[0x03] & 255);
  EXPECT_EQ(128, p[0x04] & 255);
  EXPECT_EQ(128, p[0x05] & 255);
  EXPECT_EQ(128, p[0x06] & 255);
  EXPECT_EQ(128, p[0x07] & 255);
  EXPECT_EQ(128, p[0x08] & 255);
  EXPECT_EQ(128, p[0x09] & 255);
  EXPECT_EQ(128, p[0x0a] & 255);
  EXPECT_EQ(128, p[0x0b] & 255);
  EXPECT_EQ(128, p[0x0c] & 255);
  EXPECT_EQ(128, p[0x0d] & 255);
  EXPECT_EQ(128, p[0x0e] & 255);
  EXPECT_EQ(128, p[0x0f] & 255);
  EXPECT_EQ(128, p[0x10] & 255);
  EXPECT_EQ(128, p[0x11] & 255);
  EXPECT_EQ(126, p[0x12] & 255);
}

BENCH(sleb128, bench) {
  EZBENCH2("uleb64 INT64_MAX", donothing, uleb64(p, INT64_MAX));
  EZBENCH2("zleb64 INT64_MAX", donothing, zleb64(p, INT64_MAX));
  EZBENCH2("sleb64 INT64_MAX", donothing, sleb64(p, INT64_MAX));
  EZBENCH2("uleb128 INT64_MAX", donothing, uleb128(p, INT64_MAX));
  EZBENCH2("zleb128 INT64_MAX", donothing, zleb128(p, INT64_MAX));
  EZBENCH2("sleb128 INT64_MAX", donothing, sleb128(p, INT64_MAX));
  EZBENCH2("zleb64 INT64_MIN", donothing, zleb64(p, INT64_MIN));
  EZBENCH2("sleb64 INT64_MIN", donothing, sleb64(p, INT64_MIN));
  EZBENCH2("zleb128 INT64_MIN", donothing, zleb128(p, INT64_MIN));
  EZBENCH2("sleb128 INT64_MIN", donothing, sleb128(p, INT64_MIN));
}
