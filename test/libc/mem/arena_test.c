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
#include "libc/log/libfatal.internal.h"
#include "libc/mem/arena.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(arena, test) {
  EXPECT_STREQ("hello", gc(strdup("hello")));
  __arena_push();
  EXPECT_STREQ("hello", strdup("hello"));
  __arena_push();
  EXPECT_STREQ("hello", strdup("hello"));
  for (int i = 0; i < 5000; ++i) {
    EXPECT_STREQ("hello", strdup("hello"));
  }
  free(strdup("hello"));
  __arena_pop();
  EXPECT_STREQ("", calloc(1, 16));
  EXPECT_STREQ("hello", strdup("hello"));
  __arena_pop();
}

TEST(arena, testRealloc) {
  char *b = 0;
  size_t i, n = 0;
  __arena_push();
  for (i = 0; i < kHyperionSize; ++i) {
    b = realloc(b, ++n * sizeof(*b));
    b[n - 1] = kHyperion[i];
  }
  ASSERT_EQ(0, memcmp(b, kHyperion, kHyperionSize));
  __arena_pop();
}

void *memalign_(size_t, size_t) asm("memalign");
void *calloc_(size_t, size_t) asm("calloc");

void Ca(size_t n) {
  __arena_push();
  for (int i = 0; i < n; ++i) {
    memalign_(1, 1);
  }
  __arena_pop();
}

void Cb(size_t n) {
  void **P;
  P = malloc(n * sizeof(void *));
  for (int i = 0; i < n; ++i) {
    P[i] = calloc_(1, 1);
  }
  bulk_free(P, n);
  free(P);
}

BENCH(arena, benchMalloc) {
  EZBENCH2("arena    calloc(1)", donothing, Ca(100));
  EZBENCH2("dlmalloc calloc(1)", donothing, Cb(100));
}

void Ra(void) {
  long *b = 0;
  size_t i, n = 0;
  __arena_push();
  for (i = 0; i < kHyperionSize; ++i) {
    b = realloc(b, ++n * sizeof(*b));
    b[n - 1] = kHyperion[i];
  }
  __arena_pop();
}

void Rb(void) {
  long *b = 0;
  size_t i, n = 0;
  for (i = 0; i < kHyperionSize; ++i) {
    b = realloc(b, ++n * sizeof(*b));
    b[n - 1] = kHyperion[i];
  }
  free(b);
}

BENCH(arena, benchRealloc) {
  EZBENCH2("arena    realloc", donothing, Ra());
  EZBENCH2("dlmalloc realloc", donothing, Rb());
}
