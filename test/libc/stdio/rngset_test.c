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
#include "libc/serialize.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/testlib.h"

uint64_t counter(void) {
  static uint8_t t;
  return 0x0101010101010101ull * t++;
}

TEST(rngset, testZeroReseedInterval_doesntApplyPrng) {
  char buf[32];
  EXPECT_EQ(buf, rngset(buf, sizeof(buf), counter, 0));
  EXPECT_EQ(0x0000000000000000, READ64LE(buf + 0));
  EXPECT_EQ(0x0101010101010101, READ64LE(buf + 8));
  EXPECT_EQ(0x0202020202020202, READ64LE(buf + 16));
  EXPECT_EQ(0x0303030303030303, READ64LE(buf + 24));
}

uint64_t eleet(void) {
  return 0x31337;
}

TEST(rngset, testReseedIsNeg_usesInternalVignaPrng) {
  char buf[32];
  svigna(0x31337);
  EXPECT_EQ(buf, rngset(buf, sizeof(buf), eleet, -1));
  EXPECT_EQ(vigna(), READ64LE(buf + 0));
  EXPECT_EQ(vigna(), READ64LE(buf + 8));
  EXPECT_EQ(vigna(), READ64LE(buf + 16));
  EXPECT_EQ(vigna(), READ64LE(buf + 24));
}

TEST(rngset, testNullSeedFunction_reseedBecomesVignaSeed) {
  char buf[32];
  svigna(123);
  EXPECT_EQ(buf, rngset(buf, sizeof(buf), 0, 123));
  EXPECT_EQ(vigna(), READ64LE(buf + 0));
  EXPECT_EQ(vigna(), READ64LE(buf + 8));
  EXPECT_EQ(vigna(), READ64LE(buf + 16));
  EXPECT_EQ(vigna(), READ64LE(buf + 24));
}

TEST(rngset, testWeirdlyShaped_doesntCrash) {
  char buf[7];
  rngset(buf, sizeof(buf), 0, 0);
  rngset(buf, sizeof(buf), vigna, 0);
  rngset(buf, sizeof(buf), vigna, 9);
  rngset(buf, sizeof(buf), vigna, 8);
}
