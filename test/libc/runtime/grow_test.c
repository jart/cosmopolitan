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
#include "libc/intrin/pushpop.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/version.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

__static_yoink("realloc");

TEST(grow, testNull_hasAllocatingBehavior) {
  void *p = NULL;
  size_t capacity = 0;
  EXPECT_TRUE(__grow(&p, &capacity, 1, 0));
  EXPECT_NE(NULL, p);
  EXPECT_EQ(32, capacity);
  free(p);
}

TEST(grow, testCapacity_isInUnits_withTerminatorGuarantee) {
  void *p = NULL;
  size_t capacity = 0;
  EXPECT_TRUE(__grow(&p, &capacity, 8, 0));
  EXPECT_NE(NULL, p);
  EXPECT_EQ(32 / 8 + 1, capacity);
  free(p);
}

TEST(grow, testStackMemory_convertsToDynamic) {
  int A[] = {1, 2, 3};
  int *p = A;
  size_t capacity = ARRAYLEN(A);
  if (!_isheap(p)) {
    EXPECT_TRUE(__grow(&p, &capacity, sizeof(int), 0));
    EXPECT_TRUE(_isheap(p));
    EXPECT_GT(capacity, ARRAYLEN(A));
    EXPECT_EQ(1, p[0]);
    EXPECT_EQ(2, p[1]);
    EXPECT_EQ(3, p[2]);
    p[0] = 7;
    EXPECT_EQ(1, A[0]);
    free(p);
  }
}

TEST(grow, testGrowth_clearsNewMemory) {
  size_t i, capacity = 123;
  char *p = malloc(capacity);
  memset(p, 'a', capacity);
  EXPECT_TRUE(__grow(&p, &capacity, 1, 0));
  EXPECT_GT(capacity, 123);
  for (i = 0; i < 123; ++i) ASSERT_EQ('a', p[i]);
  for (i = 123; i < capacity; ++i) ASSERT_EQ(0, p[i]);
  free(p);
}

TEST(grow, testBonusParam_willGoAboveAndBeyond) {
  size_t capacity = 32;
  char *p = malloc(capacity);
  EXPECT_TRUE(__grow(&p, &capacity, 1, 0));
  EXPECT_LT(capacity, 1024);
  free(p);
  p = malloc((capacity = 32));
  EXPECT_TRUE(__grow(&p, &capacity, 1, 1024));
  EXPECT_GT(capacity, 1024);
  free(p);
}

TEST(grow, testOverflow_returnsFalseAndDoesNotFree) {
  int A[] = {1, 2, 3};
  int *p = A;
  size_t capacity = ARRAYLEN(A);
  if (!_isheap(p)) {
    EXPECT_FALSE(__grow(&p, &capacity, pushpop(SIZE_MAX), 0));
    EXPECT_FALSE(_isheap(p));
    EXPECT_EQ(capacity, ARRAYLEN(A));
    EXPECT_EQ(1, p[0]);
    EXPECT_EQ(2, p[1]);
    EXPECT_EQ(3, p[2]);
  }
}
