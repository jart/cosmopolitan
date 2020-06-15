/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/alg/alg.h"
#include "libc/str/str.h"

static unsigned heapsortmax(int32_t (*A)[2], unsigned n, unsigned i, unsigned j,
                            unsigned k) {
  unsigned m = i;
  if (j < n && A[j][0] > A[m][0]) m = j;
  if (k < n && A[k][0] > A[m][0]) m = k;
  return m;
}

static void heapsortdown(int32_t (*A)[2], unsigned n, unsigned i) {
  unsigned j;
  int32_t t[2];
  for (;;) {
    unsigned j = heapsortmax(A, n, i, 2 * i + 1, 2 * i + 2);
    if (j == i) break;
    memcpy(t, A[i], sizeof(t));
    memcpy(A[i], A[j], sizeof(t));
    memcpy(A[j], t, sizeof(t));
    i = j;
  }
}

/**
 * Sorts key-values.
 *
 * heapsortcar is a linearithmic / constant-memory / non-stable sorting
 * function that's a tenth the size of the more generalized qsort() API.
 * It can only sort arrays of 64-bit values, and comparisons happen by
 * treating the lower 32-bits as a signed integer.
 *
 * @see test/libc/alg/heapsortcar_test.c
 */
void heapsortcar(int32_t (*A)[2], unsigned n) {
  unsigned i;
  int32_t t[2];
  for (i = ((n - 2) / 2) + 1; i > 0; i--) {
    heapsortdown(A, n, i - 1);
  }
  for (i = 0; i < n; i++) {
    memcpy(t, A[n - i - 1], sizeof(t));
    memcpy(A[n - i - 1], A[0], sizeof(t));
    memcpy(A[0], t, sizeof(t));
    heapsortdown(A, n - i - 1, 0);
  }
}
