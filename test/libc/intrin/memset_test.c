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
#include "libc/intrin/asan.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

static dontasan void *golden(void *p, int c, size_t n) {
  size_t i;
  if (IsAsan()) __asan_verify(p, n);
  for (i = 0; i < n; ++i) ((char *)p)[i] = c;
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

BENCH(memset, bench) {
  int n, max = 8 * 1024 * 1024;
  char *volatile p = gc(malloc(max));

  EZBENCH_N("memset", 0, memset(p, -1, 0));
  for (n = 2; n <= max; n *= 2) {
    EZBENCH_N("memset", n - 1, memset(p, -1, n - 1));
    EZBENCH_N("memset", n, memset(p, -1, n));
  }

  EZBENCH_N("memset16", 0, memset16((char16_t *)p, -1, 0));
  for (n = 2; n <= max; n *= 2) {
    EZBENCH_N("memset16", n, memset16((char16_t *)p, -1, n / 2));
  }

  EZBENCH_N("bzero", 0, bzero(p, 0));
  for (n = 2; n <= max; n *= 2) {
    EZBENCH_N("bzero", n - 1, bzero(p, n - 1));
    EZBENCH_N("bzero", n, bzero(p, n));
  }
}

BENCH(strlen, bench) {
  int n, max = 8 * 1024 * 1024;
  char *volatile p = gc(calloc(max + 1, 1));
  EZBENCH_N("strlen", 0, strlen(p));
  for (n = 2; n <= max; n *= 2) {
    memset(p, -1, n - 1);
    EZBENCH_N("strlen", n - 1, strlen(p));
    p[n - 1] = -1;
    EZBENCH_N("strlen", n, strlen(p));
  }
}
