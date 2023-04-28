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
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/libcxx/learned_sort.h"
#include "third_party/libcxx/pdqsort.h"
#include "third_party/libcxx/ska_sort.h"
#include "third_party/vqsort/vqsort.h"

void InsertionSort(long *A, long n) {
  long i, j, t;
  for (i = 1; i < n; i++) {
    t = A[i];
    j = i - 1;
    while (j >= 0 && A[j] > t) {
      A[j + 1] = A[j];
      j = j - 1;
    }
    A[j + 1] = t;
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
  EZBENCH2("_longsort rand", memcpy(p2, p1, n * sizeof(long)),
           _longsort(p2, n));
  if (X86_HAVE(AVX2)) {
    EZBENCH2("vq_int64_avx2 rand", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_avx2(p2, n));
  }
  if (X86_HAVE(SSE4_2)) {
    EZBENCH2("vq_int64_sse4 rand", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_sse4(p2, n));
  }
  if (X86_HAVE(SSSE3)) {
    EZBENCH2("vq_int64_ssse3 rand", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_ssse3(p2, n));
  }
  EZBENCH2("vq_int64_sse2 rand", memcpy(p2, p1, n * sizeof(long)),
           vqsort_int64_sse2(p2, n));
  EZBENCH2("radix_int64 rand", memcpy(p2, p1, n * sizeof(long)),
           radix_sort_int64(p2, n));
  EZBENCH2("ska_sort_int64 rand", memcpy(p2, p1, n * sizeof(long)),
           ska_sort_int64(p2, n));
  EZBENCH2("pdq_int64 rand", memcpy(p2, p1, n * sizeof(long)),
           pdqsort_int64(p2, n));
  EZBENCH2("pdq_branchless rand", memcpy(p2, p1, n * sizeof(long)),
           pdqsort_branchless_int64(p2, n));
  EZBENCH2("learned_int64 rand", memcpy(p2, p1, n * sizeof(long)),
           learned_sort_int64(p2, n));
  EZBENCH2("qsort(long) rand", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("InsertionSort rand", memcpy(p2, p1, n * sizeof(long)),
           InsertionSort(p2, n));
}

BENCH(_longsort, benchNearlySorted) {
  printf("\n");
  long i, j, k, t;
  long n = 5000;  // total items
  long m = 500;   // misplaced items
  long d = 5;     // maximum drift
  long *p1 = gc(malloc(n * sizeof(long)));
  long *p2 = gc(malloc(n * sizeof(long)));
  for (i = 0; i < n; ++i) {
    p1[i] = i;
  }
  for (i = 0; i < m; ++i) {
    j = rand() % n;
    k = rand() % (d * 2) - d + j;
    k = MIN(MAX(0, k), n - 1);
    t = p1[j];
    p1[j] = p1[k];
    p1[k] = t;
  }
  EZBENCH2("_longsort near", memcpy(p2, p1, n * sizeof(long)),
           _longsort(p2, n));
  if (X86_HAVE(AVX2)) {
    EZBENCH2("vq_int64_avx2 near", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_avx2(p2, n));
  }
  if (X86_HAVE(SSE4_2)) {
    EZBENCH2("vq_int64_sse4 near", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_sse4(p2, n));
  }
  if (X86_HAVE(SSSE3)) {
    EZBENCH2("vq_int64_ssse3 near", memcpy(p2, p1, n * sizeof(long)),
             vqsort_int64_ssse3(p2, n));
  }
  EZBENCH2("vq_int64_sse2 near", memcpy(p2, p1, n * sizeof(long)),
           vqsort_int64_sse2(p2, n));
  EZBENCH2("radix_int64 near", memcpy(p2, p1, n * sizeof(long)),
           radix_sort_int64(p2, n));
  EZBENCH2("ska_sort_int64 near", memcpy(p2, p1, n * sizeof(long)),
           ska_sort_int64(p2, n));
  EZBENCH2("pdq_int64 near", memcpy(p2, p1, n * sizeof(long)),
           pdqsort_int64(p2, n));
  EZBENCH2("pdq_branchless near", memcpy(p2, p1, n * sizeof(long)),
           pdqsort_branchless_int64(p2, n));
  EZBENCH2("learned_int64 near", memcpy(p2, p1, n * sizeof(long)),
           learned_sort_int64(p2, n));
  EZBENCH2("qsort(long) near", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("InsertionSort near", memcpy(p2, p1, n * sizeof(long)),
           InsertionSort(p2, n));
}

int CompareInt(const void *a, const void *b) {
  const int *x = a;
  const int *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

TEST(_intsort, test) {
  size_t n = 5000;
  int *a = gc(calloc(n, sizeof(int)));
  int *b = gc(calloc(n, sizeof(int)));
  rngset(a, n * sizeof(int), 0, 0);
  memcpy(b, a, n * sizeof(int));
  qsort(a, n, sizeof(int), CompareInt);
  _intsort(b, n);
  ASSERT_EQ(0, memcmp(b, a, n * sizeof(int)));
}

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

BENCH(_intsort, bench) {
  printf("\n");
  size_t n = 10000;
  int *p1 = gc(malloc(n * sizeof(int)));
  int *p2 = gc(malloc(n * sizeof(int)));
  rngset(p1, n * sizeof(int), 0, 0);
  EZBENCH2("_intsort", memcpy(p2, p1, n * sizeof(int)), _intsort(p2, n));
  EZBENCH2("djbsort", memcpy(p2, p1, n * sizeof(int)), djbsort(p2, n));
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
  EZBENCH2("radix_sort_int32", memcpy(p2, p1, n * sizeof(int)),
           radix_sort_int32(p2, n));
  EZBENCH2("qsort(int)", memcpy(p2, p1, n * sizeof(int)),
           qsort(p2, n, sizeof(int), CompareInt));
}
