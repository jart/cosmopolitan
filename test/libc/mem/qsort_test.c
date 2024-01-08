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
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

int CompareLow(const void *a, const void *b) {
  const int *x = a;
  const int *y = b;
  if ((char)*x < (char)*y) return -1;
  if ((char)*x > (char)*y) return +1;
  return 0;
}

void InsertionSortLow(int *A, int n) {
  int i, j, t;
  for (i = 1; i < n; i++) {
    t = A[i];
    j = i - 1;
    while (j >= 0 && (char)A[j] > (char)t) {
      A[j + 1] = A[j];
      j = j - 1;
    }
    A[j + 1] = t;
  }
}

bool IsStableSort(void sort(void *, size_t, size_t,
                            int (*)(const void *, const void *))) {
  int n = 256;
  int *A = gc(malloc(n * sizeof(int)));
  int *B = gc(malloc(n * sizeof(int)));
  rngset(A, n * sizeof(int), 0, 0);
  memcpy(B, A, n * sizeof(int));
  InsertionSortLow(A, n);
  sort(B, n, sizeof(int), CompareLow);
  return !memcmp(A, B, n * sizeof(int));
}

TEST(sort, stability) {
  ASSERT_FALSE(IsStableSort(qsort));
  ASSERT_FALSE(IsStableSort(smoothsort));
  ASSERT_FALSE(IsStableSort((void *)qsort_r));
  ASSERT_FALSE(IsStableSort((void *)heapsort));
  ASSERT_TRUE(IsStableSort((void *)mergesort));
}

int CompareInt(const void *a, const void *b) {
  const int *x = a;
  const int *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

int CompareLong(const void *a, const void *b) {
  const long *x = a;
  const long *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

TEST(qsort, words) {
  const int32_t A[][2] = {{4, 'a'},   {65, 'b'}, {2, 'c'}, {-31, 'd'},
                          {0, 'e'},   {99, 'f'}, {2, 'g'}, {83, 'h'},
                          {782, 'i'}, {1, 'j'}};
  const int32_t B[][2] = {{-31, 'd'}, {0, 'e'},  {1, 'j'},  {2, 'c'},
                          {2, 'g'},   {4, 'a'},  {65, 'b'}, {83, 'h'},
                          {99, 'f'},  {782, 'i'}};
  int32_t(*M)[2] = malloc(sizeof(A));
  memcpy(M, B, sizeof(A));
  qsort(M, ARRAYLEN(A), sizeof(*M), CompareInt);
  EXPECT_EQ(0, memcmp(M, B, sizeof(B)));
  free(M);
}

struct Record {
  int x;
  int y;
  int z;
  int a[2];
  int b;
};

int CompareRecord(const void *a, const void *b) {
  const struct Record *x = a;
  const struct Record *y = b;
  if (x->z > y->z) return -1;
  if (x->z < y->z) return +1;
  return 0;
}

TEST(qsort, records) {
  int i, n = 256;
  struct Record *A = gc(calloc(n, sizeof(struct Record)));
  struct Record *B = gc(calloc(n, sizeof(struct Record)));
  for (i = 0; i < n; ++i) A[i].z = B[i].z = lemur64();
  qsort(A, n, sizeof(struct Record), CompareRecord);
  mergesort(B, n, sizeof(struct Record), CompareRecord);
  ASSERT_EQ(0, memcmp(A, B, n * sizeof(struct Record)));
}

TEST(qsort, equivalence_random) {
  size_t i;
  size_t n = 1000;
  long *a = gc(malloc(n * sizeof(long)));
  long *b = gc(malloc(n * sizeof(long)));
  long *c = gc(malloc(n * sizeof(long)));
  for (i = 0; i < n; ++i) a[i] = lemur64();
  memcpy(b, a, n * sizeof(long));
  memcpy(c, a, n * sizeof(long));
  qsort(b, n, sizeof(long), CompareLong);
  heapsort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  mergesort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  smoothsort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  _longsort(c, n);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
}

TEST(qsort, equivalence_reverse) {
  size_t i;
  size_t n = 1000;
  long *a = gc(malloc(n * sizeof(long)));
  long *b = gc(malloc(n * sizeof(long)));
  long *c = gc(malloc(n * sizeof(long)));
  for (i = 0; i < n; ++i) a[n - i - 1] = i;
  memcpy(b, a, n * sizeof(long));
  memcpy(c, a, n * sizeof(long));
  qsort(b, n, sizeof(long), CompareLong);
  heapsort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  mergesort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  smoothsort(c, n, sizeof(long), CompareLong);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
  memcpy(c, a, n * sizeof(long));
  _longsort(c, n);
  ASSERT_EQ(0, memcmp(b, c, n * sizeof(long)));
}

BENCH(qsort, bench) {
  size_t i;
  size_t n = 1000;
  long *p1 = gc(malloc(n * sizeof(long)));
  long *p2 = gc(malloc(n * sizeof(long)));

  printf("\n");
  for (i = 0; i < n; ++i) p1[i] = i + ((lemur64() % 3) - 1);
  EZBENCH2("qsort nearly", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("qsort_r nearly", memcpy(p2, p1, n * sizeof(long)),
           qsort_r(p2, n, sizeof(long), (void *)CompareLong, 0));
  EZBENCH2("heapsort nearly", memcpy(p2, p1, n * sizeof(long)),
           heapsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("mergesort nearly", memcpy(p2, p1, n * sizeof(long)),
           mergesort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("smoothsort nearly", memcpy(p2, p1, n * sizeof(long)),
           smoothsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("_longsort nearly", memcpy(p2, p1, n * sizeof(long)),
           _longsort(p2, n));

  printf("\n");
  for (i = 0; i < n; ++i) p1[i] = n - i;
  EZBENCH2("qsort reverse", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("qsort_r reverse", memcpy(p2, p1, n * sizeof(long)),
           qsort_r(p2, n, sizeof(long), (void *)CompareLong, 0));
  EZBENCH2("heapsort reverse", memcpy(p2, p1, n * sizeof(long)),
           heapsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("mergesort reverse", memcpy(p2, p1, n * sizeof(long)),
           mergesort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("smoothsort reverse", memcpy(p2, p1, n * sizeof(long)),
           smoothsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("_longsort reverse", memcpy(p2, p1, n * sizeof(long)),
           _longsort(p2, n));

  printf("\n");
  rngset(p1, n * sizeof(long), 0, 0);
  EZBENCH2("qsort random", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("qsort_r random", memcpy(p2, p1, n * sizeof(long)),
           qsort_r(p2, n, sizeof(long), (void *)CompareLong, 0));
  EZBENCH2("heapsort random", memcpy(p2, p1, n * sizeof(long)),
           heapsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("mergesort random", memcpy(p2, p1, n * sizeof(long)),
           mergesort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("smoothsort random", memcpy(p2, p1, n * sizeof(long)),
           smoothsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("_longsort random", memcpy(p2, p1, n * sizeof(long)),
           _longsort(p2, n));

  printf("\n");
  for (i = 0; i < n / 2; ++i) {
    p1[i] = i;
    p1[n - i - 1] = i;
  }
  EZBENCH2("qsort 2n", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("qsort_r 2n", memcpy(p2, p1, n * sizeof(long)),
           qsort_r(p2, n, sizeof(long), (void *)CompareLong, 0));
  EZBENCH2("heapsort 2n", memcpy(p2, p1, n * sizeof(long)),
           heapsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("mergesort 2n", memcpy(p2, p1, n * sizeof(long)),
           mergesort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("smoothsort 2n", memcpy(p2, p1, n * sizeof(long)),
           smoothsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("_longsort 2n", memcpy(p2, p1, n * sizeof(long)), _longsort(p2, n));
}
