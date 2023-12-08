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
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

TEST(ilogb, yolo) {
  EXPECT_EQ(0, ilogb(1));
  EXPECT_EQ(1, ilogb(2));
  EXPECT_EQ(2, ilogb(4));
  EXPECT_EQ(63, ilogb(1e19));
  EXPECT_EQ(0, ilogbf(1));
  EXPECT_EQ(1, ilogbf(2));
  EXPECT_EQ(2, ilogbf(4));
  EXPECT_EQ(63, ilogb(1e19));
  EXPECT_EQ(0, ilogbl(1));
  EXPECT_EQ(1, ilogbl(2));
  EXPECT_EQ(2, ilogbl(4));
  EXPECT_EQ(63, ilogbl(1e19));
}

TEST(logb, yolo) {
  EXPECT_EQ(0, (int)logb(1));
  EXPECT_EQ(1, (int)logb(2));
  EXPECT_EQ(2, (int)logb(4));
  EXPECT_EQ(63, (int)logb(1e19));
  EXPECT_EQ(0, (int)logbf(1));
  EXPECT_EQ(1, (int)logbf(2));
  EXPECT_EQ(2, (int)logbf(4));
  EXPECT_EQ(63, (int)logb(1e19));
  EXPECT_EQ(0, (int)logbl(1));
  EXPECT_EQ(1, (int)logbl(2));
  EXPECT_EQ(2, (int)logbl(4));
  EXPECT_EQ(63, (int)logbl(1e19));
}

TEST(ilogb, NANandZero) {
  EXPECT_EQ(FP_ILOGB0, ilogb(0.0));
  EXPECT_EQ(FP_ILOGBNAN, ilogb(NAN));
  EXPECT_EQ(FP_ILOGB0, (int)ilogb(0));
  EXPECT_EQ(FP_ILOGBNAN, (int)ilogb(NAN));
  EXPECT_EQ(FP_ILOGB0, ilogbf(0));
  EXPECT_EQ(FP_ILOGBNAN, ilogbf(NAN));
  EXPECT_EQ(FP_ILOGB0, ilogbl(0));
  EXPECT_EQ(FP_ILOGBNAN, ilogbl(NAN));
}
