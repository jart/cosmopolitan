/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

// Credit: Andrew Schein. 2009. Open-source C++ implementation of Radix
//         Sort for double-precision floating points. (2009).

#define HIST_SIZE               (size_t)2048
#define get_byte_0(v)           ((v)&0x7FF)
#define get_byte_1(v)           (((v) >> 11) & 0x7FF)
#define get_byte_2(v)           (((v) >> 22) & 0x7FF)
#define get_byte_3(v)           (((v) >> 33) & 0x7FF)
#define get_byte_4(v)           (((v) >> 44) & 0x7FF)
#define get_byte_5(v)           (((v) >> 55) & 0x7FF)
#define get_byte_2_flip_sign(v) (((unsigned)(v) >> 22) ^ 0x200)
#define get_byte_5_flip_sign(v) ((((v) >> 55) & 0x7FF) ^ 0x400)

int radix_sort_int64(int64_t *A, size_t n) {
  int64_t *T, *reader, *writer;
  size_t *b0, *b1, *b2, *b3, *b4, *b5;
  size_t i, pos, sum0, sum1, sum2, sum3, sum4, sum5, tsum;

  if (n < HIST_SIZE) {
    _longsort(A, n);
    return 0;
  }

  if (!(T = (int64_t *)malloc(n * sizeof(int64_t)))) {
    return -1;
  }

  if (!(b0 = (size_t *)calloc(HIST_SIZE * 6, sizeof(size_t)))) {
    free(T);
    return -1;
  }

  b1 = b0 + HIST_SIZE;
  b2 = b1 + HIST_SIZE;
  b3 = b2 + HIST_SIZE;
  b4 = b3 + HIST_SIZE;
  b5 = b4 + HIST_SIZE;

  for (i = 0; i < n; i++) {
    b0[get_byte_0(A[i])]++;
    b1[get_byte_1(A[i])]++;
    b2[get_byte_2(A[i])]++;
    b3[get_byte_3(A[i])]++;
    b4[get_byte_4(A[i])]++;
    b5[get_byte_5_flip_sign(A[i])]++;
  }

  sum0 = sum1 = sum2 = sum3 = sum4 = sum5 = tsum = 0;

  for (i = 0; i < HIST_SIZE; i++) {
    tsum = b0[i] + sum0;
    b0[i] = sum0 - 1;
    sum0 = tsum;

    tsum = b1[i] + sum1;
    b1[i] = sum1 - 1;
    sum1 = tsum;

    tsum = b2[i] + sum2;
    b2[i] = sum2 - 1;
    sum2 = tsum;

    tsum = b3[i] + sum3;
    b3[i] = sum3 - 1;
    sum3 = tsum;

    tsum = b4[i] + sum4;
    b4[i] = sum4 - 1;
    sum4 = tsum;

    tsum = b5[i] + sum5;
    b5[i] = sum5 - 1;
    sum5 = tsum;
  }

  writer = T;
  reader = A;
  for (i = 0; i < n; i++) {
    pos = get_byte_0(reader[i]);
    writer[++b0[pos]] = reader[i];
  }

  writer = A;
  reader = T;
  for (i = 0; i < n; i++) {
    pos = get_byte_1(reader[i]);
    writer[++b1[pos]] = reader[i];
  }

  writer = T;
  reader = A;
  for (i = 0; i < n; i++) {
    pos = get_byte_2(reader[i]);
    writer[++b2[pos]] = reader[i];
  }

  writer = A;
  reader = T;
  for (i = 0; i < n; i++) {
    pos = get_byte_3(reader[i]);
    writer[++b3[pos]] = reader[i];
  }

  writer = T;
  reader = A;
  for (i = 0; i < n; i++) {
    pos = get_byte_4(reader[i]);
    writer[++b4[pos]] = reader[i];
  }

  writer = A;
  reader = T;
  for (i = 0; i < n; i++) {
    pos = get_byte_5_flip_sign(reader[i]);
    writer[++b5[pos]] = reader[i];
  }

  free(b0);
  free(T);
  return 0;
}
