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
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"

static inline void InsertionSort(long *A, long n) {
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

static void LongSort(long *A, long n) {
  long t, p, i, j;
  if (n <= 32) {
    InsertionSort(A, n);
  } else {
    for (p = A[n >> 1], i = 0, j = n - 1;; i++, j--) {
      while (A[i] < p) i++;
      while (A[j] > p) j--;
      if (i >= j) break;
      t = A[i];
      A[i] = A[j];
      A[j] = t;
    }
    LongSort(A, i);
    LongSort(A + i, n - i);
  }
}

/**
 * Sorting algorithm for longs that doesn't take long.
 *
 *     "What disorder is this? Give me my long sort!"
 *                               -Lord Capulet
 *
 */
void _longsort(long *A, size_t n) {
  LongSort(A, n);
  if (n > 1000) {
    STRACE("_longsort(%p, %'zu)", A, n);
  }
}
