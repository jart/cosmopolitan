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
#include "libc/dce.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/testlib.h"

static void *golden(void *p, int c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i)
    ((char *)p)[i] = c;
  return p;
}

TEST(memset, hug) {
  char *a, *b;
  int i, j, c;
  a = gc(malloc(1025 * 2));
  b = gc(malloc(1025 * 2));
  for (i = 0; i < 1025; ++i) {
    for (j = 0; j < 1025 - i; ++j) {
      c = lemur64();
      rngset(a, i + j, 0, 0);
      memcpy(b, a, i + j);
      ASSERT_EQ(a + i, golden(a + i, c, j));
      ASSERT_EQ(b + i, memset(b + i, c, j));
      ASSERT_EQ(0, timingsafe_bcmp(a, b, i + j));
    }
  }
}

TEST(bzero, hug) {
  char *a, *b;
  int i, j;
  a = gc(malloc(1025 * 2));
  b = gc(malloc(1025 * 2));
  for (i = 0; i < 1025; ++i) {
    for (j = 0; j < 1025 - i; ++j) {
      rngset(a, i + j, 0, 0);
      memcpy(b, a, i + j);
      golden(a + i, 0, j);
      bzero(b + i, j);
      ASSERT_EQ(0, timingsafe_bcmp(a, b, i + j));
    }
  }
}

#define N (256 * 1024 * 1024)

BENCH(memset, bench) {
  void *memset_(void *, int, size_t) asm("memset");
  printf("\n");
  static char A[N];
  memset(A, 2, N);
  for (int n = 1; n <= N; n *= 2)
    BENCHMARK(100, n, X(memset_(V(A), 0, n)));
}
