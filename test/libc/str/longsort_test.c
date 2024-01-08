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
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/vqsort/vqsort.h"

void InsertionSort(int *A, int n) {
  for (int i = 1; i < n; i++) {
    int key = A[i];
    int j = i - 1;
    while (j >= 0 && A[j] > key) {
      A[j + 1] = A[j];
      j--;
    }
    A[j + 1] = key;
  }
}

int CompareLong(const void *a, const void *b) {
  const long *x = a;
  const long *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

TEST(_longsort, test) {
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  _longsort(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

#ifdef __x86_64__

TEST(vqsort_int64_avx2, test) {
  if (!X86_HAVE(AVX2)) return;
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  vqsort_int64_avx2(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

TEST(vqsort_int64_sse4, test) {
  if (!X86_HAVE(SSE4_2)) return;
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  vqsort_int64_sse4(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

TEST(vqsort_int64_ssse3, test) {
  if (!X86_HAVE(SSSE3)) return;
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  vqsort_int64_ssse3(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

TEST(vqsort_int64_sse2, test) {
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  vqsort_int64_sse2(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

#endif /* __x86_64__ */

TEST(radix_sort_int64, test) {
  size_t n = 5000;
  long *a = gc(calloc(n, sizeof(long)));
  long *b = gc(calloc(n, sizeof(long)));
  rngset(a, n * sizeof(long), 0, 0);
  memcpy(b, a, n * sizeof(long));
  qsort(a, n, sizeof(long), CompareLong);
  radix_sort_int64(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(long)));
}

BENCH(_longsort, bench) {
  printf("\n");
  size_t n = 5000;
  long *p1 = gc(malloc(n * sizeof(long)));
  long *p2 = gc(malloc(n * sizeof(long)));
  rngset(p1, n * sizeof(long), 0, 0);
  EZBENCH2("_longsort", memcpy(p2, p1, n * sizeof(long)), _longsort(p2, n));
#ifdef __x86_64__
  if (X86_HAVE(AVX2)) {
    EZBENCH2("vqsort_int64_avx2", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_avx2(p2, n));
  }
  if (X86_HAVE(SSE4_2)) {
    EZBENCH2("vqsort_int64_sse4", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_sse4(p2, n));
  }
  if (X86_HAVE(SSSE3)) {
    EZBENCH2("vqsort_int64_ssse3", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_ssse3(p2, n));
  }
  EZBENCH2("vqsort_int64_sse2", memcpy(p2, p1, n * sizeof(long)),
           vqsort_int64_sse2(p2, n));
#endif /* __x86_64__ */
  EZBENCH2("radix_sort_int64", memcpy(p2, p1, n * sizeof(long)),
           radix_sort_int64(p2, n));
  EZBENCH2("qsort(long)", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
}

int CompareInt(const void *a, const void *b) {
  const int *x = a;
  const int *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

TEST(InsertionSort, test) {
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  InsertionSort(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

#ifdef __x86_64__

TEST(vqsort_int32_avx2, test) {
  if (!X86_HAVE(AVX2)) return;
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  vqsort_int32_avx2(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

TEST(vqsort_int32_sse4, test) {
  if (!X86_HAVE(SSE4_2)) return;
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  vqsort_int32_sse4(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

TEST(vqsort_int32_ssse3, test) {
  if (!X86_HAVE(SSSE3)) return;
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  vqsort_int32_ssse3(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

TEST(vqsort_int32_sse2, test) {
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  vqsort_int32_sse2(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

#endif /* __x86_64__ */

TEST(radix_sort_int32, test) {
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  radix_sort_int32(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

BENCH(InsertionSort, bench) {
  printf("\n");
  size_t n = 10000;
  int *p1 = gc(malloc(n * sizeof(int)));
  int *p2 = gc(malloc(n * sizeof(int)));
  rngset(p1, n * sizeof(int), 0, 0);
  EZBENCH2("InsertionSort", memcpy(p2, p1, n * sizeof(int)),
           InsertionSort(p2, n));
#ifdef __x86_64__
  if (X86_HAVE(AVX2)) {
    EZBENCH2("vqsort_int32_avx2", memcpy(p2, p1, n * sizeof(int)),
             vqsort_int32_avx2(p2, n));
  }
  if (X86_HAVE(SSE4_2)) {
    EZBENCH2("vqsort_int32_sse4", memcpy(p2, p1, n * sizeof(int)),
             vqsort_int32_sse4(p2, n));
  }
  if (X86_HAVE(SSSE3)) {
    EZBENCH2("vqsort_int32_ssse3", memcpy(p2, p1, n * sizeof(int)),
             vqsort_int32_ssse3(p2, n));
  }
  EZBENCH2("vqsort_int32_sse2", memcpy(p2, p1, n * sizeof(int)),
           vqsort_int32_sse2(p2, n));
#endif /* __x86_64__ */
  EZBENCH2("djbsort", memcpy(p2, p1, n * sizeof(int)), djbsort(p2, n));
  EZBENCH2("radix_sort_int32", memcpy(p2, p1, n * sizeof(int)),
           radix_sort_int32(p2, n));
  EZBENCH2("qsort(int)", memcpy(p2, p1, n * sizeof(int)),
           qsort(p2, n, sizeof(int), CompareInt));
}
