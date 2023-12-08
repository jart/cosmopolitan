/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/mem.h"
#include "libc/mem/sortedints.internal.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

struct SortedInts T;

void TearDown(void) {
  free(T.p);
  bzero(&T, sizeof(T));
}

TEST(sortedints, test) {
  EXPECT_TRUE(InsertInt(&T, 3, false));
  EXPECT_TRUE(InsertInt(&T, 1, false));
  EXPECT_TRUE(InsertInt(&T, -1, false));
  EXPECT_TRUE(InsertInt(&T, 2, false));
  EXPECT_EQ(4, T.n);
  EXPECT_EQ(-1, T.p[0]);
  EXPECT_EQ(+1, T.p[1]);
  EXPECT_EQ(+2, T.p[2]);
  EXPECT_EQ(+3, T.p[3]);
  EXPECT_FALSE(ContainsInt(&T, -2));
  EXPECT_TRUE(ContainsInt(&T, -1));
  EXPECT_FALSE(ContainsInt(&T, 0));
  EXPECT_TRUE(ContainsInt(&T, 1));
  EXPECT_TRUE(ContainsInt(&T, 2));
  EXPECT_TRUE(ContainsInt(&T, 3));
  EXPECT_FALSE(ContainsInt(&T, 4));
}

TEST(sortedints, unique) {
  EXPECT_TRUE(InsertInt(&T, INT_MAX, true));
  EXPECT_TRUE(InsertInt(&T, 1, true));
  EXPECT_FALSE(InsertInt(&T, INT_MAX, true));
  EXPECT_TRUE(InsertInt(&T, INT_MIN, true));
  EXPECT_FALSE(InsertInt(&T, 1, true));
  EXPECT_TRUE(InsertInt(&T, 2, true));
  EXPECT_EQ(4, T.n);
  EXPECT_EQ(INT_MIN, T.p[0]);
  EXPECT_EQ(+1, T.p[1]);
  EXPECT_EQ(+2, T.p[2]);
  EXPECT_EQ(INT_MAX, T.p[3]);
  EXPECT_FALSE(ContainsInt(&T, -2));
  EXPECT_TRUE(ContainsInt(&T, INT_MIN));
  EXPECT_FALSE(ContainsInt(&T, 0));
  EXPECT_TRUE(ContainsInt(&T, 1));
  EXPECT_TRUE(ContainsInt(&T, 2));
  EXPECT_TRUE(ContainsInt(&T, INT_MAX));
  EXPECT_FALSE(ContainsInt(&T, 4));
  EXPECT_EQ(1, CountInt(&T, 1));
  EXPECT_EQ(0, CountInt(&T, -5));
}

TEST(sortedints, bag) {
  EXPECT_TRUE(InsertInt(&T, INT_MAX, false));
  EXPECT_TRUE(InsertInt(&T, 1, false));
  EXPECT_TRUE(InsertInt(&T, INT_MAX, false));
  EXPECT_TRUE(InsertInt(&T, INT_MIN, false));
  EXPECT_TRUE(InsertInt(&T, 1, false));
  EXPECT_TRUE(InsertInt(&T, 2, false));
  EXPECT_EQ(6, T.n);
  EXPECT_EQ(INT_MIN, T.p[0]);
  EXPECT_EQ(1, T.p[1]);
  EXPECT_EQ(1, T.p[2]);
  EXPECT_EQ(2, T.p[3]);
  EXPECT_EQ(INT_MAX, T.p[4]);
  EXPECT_EQ(INT_MAX, T.p[5]);
  EXPECT_FALSE(ContainsInt(&T, -2));
  EXPECT_TRUE(ContainsInt(&T, INT_MIN));
  EXPECT_FALSE(ContainsInt(&T, 0));
  EXPECT_TRUE(ContainsInt(&T, 1));
  EXPECT_TRUE(ContainsInt(&T, 2));
  EXPECT_TRUE(ContainsInt(&T, INT_MAX));
  EXPECT_FALSE(ContainsInt(&T, 4));
  EXPECT_EQ(1, CountInt(&T, INT_MIN));
  EXPECT_EQ(2, CountInt(&T, 1));
  EXPECT_EQ(0, CountInt(&T, -5));
}

TEST(sortedints, fuzz) {
  for (int i = 0; i < 10000; ++i) {
    volatile bool b;
    volatile int y, x = lemur64();
    InsertInt(&T, x, x & 1);
    b = ContainsInt(&T, x);
    b = ContainsInt(&T, -x);
    y = CountInt(&T, x);
    (void)b;
    (void)y;
  }
  for (int i = 1; i < T.n; ++i) {
    ASSERT_GE(T.p[i], T.p[i - 1]);
  }
}

BENCH(sortedints, bench) {
  volatile int x;
  EZBENCH2("overhead", donothing, x = lemur64());
  EZBENCH2("insert small", donothing, InsertInt(&T, lemur64(), true));
  EZBENCH2("contains small", donothing, ContainsInt(&T, lemur64()));
  for (int i = 0; i < 20000; ++i) {
    InsertInt(&T, lemur64(), true);
  }
  EZBENCH2("insert big", donothing, InsertInt(&T, lemur64(), true));
  EZBENCH2("contains big", donothing, ContainsInt(&T, lemur64()));
  (void)x;
}
