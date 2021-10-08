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
#include "libc/alg/alg.h"
#include "libc/bits/bits.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

int CompareLong(const void *a, const void *b) {
  const long *x = a;
  const long *y = b;
  if (*x < *y) return -1;
  if (*x > *y) return +1;
  return 0;
}

TEST(qsort, test) {
  const int32_t A[][2] = {{4, 'a'},   {65, 'b'}, {2, 'c'}, {-31, 'd'},
                          {0, 'e'},   {99, 'f'}, {2, 'g'}, {83, 'h'},
                          {782, 'i'}, {1, 'j'}};
  const int32_t B[][2] = {{-31, 'd'}, {0, 'e'},  {1, 'j'},  {2, 'c'},
                          {2, 'g'},   {4, 'a'},  {65, 'b'}, {83, 'h'},
                          {99, 'f'},  {782, 'i'}};
  int32_t(*M)[2] = malloc(sizeof(A));
  memcpy(M, B, sizeof(A));
  qsort(M, ARRAYLEN(A), sizeof(*M), cmpsl);
  EXPECT_EQ(0, memcmp(M, B, sizeof(B)));
  free(M);
}

BENCH(qsort, bench) {
  size_t n = 1000;
  long *p1 = gc(malloc(n * sizeof(long)));
  long *p2 = gc(malloc(n * sizeof(long)));
  rngset(p1, n * sizeof(long), 0, 0);
  EZBENCH2("qsort", memcpy(p2, p1, n * sizeof(long)),
           qsort(p2, n, sizeof(long), CompareLong));
  EZBENCH2("longsort", memcpy(p2, p1, n * sizeof(long)), longsort(p2, n));
}
