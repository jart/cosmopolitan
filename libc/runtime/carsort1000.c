/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/runtime/carsort.h"
#include "libc/str/str.h"

static textstartup size_t HeapSortMax(size_t n, int32_t A[n][2], long i, long j,
                                      long k) {
  if (j < n && A[j][0] > A[i][0]) i = j;
  if (k < n && A[k][0] > A[i][0]) i = k;
  return i;
}

static textstartup void HeapSortDown(size_t n, int32_t A[n][2], long i) {
  size_t j;
  int32_t t[2];
  for (;;) {
    j = HeapSortMax(n, A, i, 2 * i + 1, 2 * i + 2);
    if (j == i) break;
    memcpy(t, A[i], sizeof(t));
    memcpy(A[i], A[j], sizeof(t));
    memcpy(A[j], t, sizeof(t));
    i = j;
  }
}

/**
 * Sorts int32 key-value arrays of nontrivial size.
 *
 * @see test/libc/alg/carsort_test.c
 * @see carsort100() if smaller
 */
textstartup void carsort1000(size_t n, int32_t A[n][2]) {
  size_t i;
  int32_t t[2];
  for (i = ((n - 2) / 2) + 1; i > 0; i--) {
    HeapSortDown(n, A, i - 1);
  }
  for (i = 0; i < n; i++) {
    memcpy(t, A[n - i - 1], sizeof(t));
    memcpy(A[n - i - 1], A[0], sizeof(t));
    memcpy(A[0], t, sizeof(t));
    HeapSortDown(n - i - 1, A, 0);
  }
}
