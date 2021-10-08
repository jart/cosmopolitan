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
#include "libc/mem/arena.h"
#include "libc/mem/mem.h"
#include "libc/testlib/ezbench.h"
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

void *calloc_(size_t, size_t) asm("calloc");

void A(size_t n) {
  __arena_push();
  for (int i = 0; i < n; ++i) {
    calloc_(15, 1);
  }
  __arena_pop();
}

void B(size_t n) {
  void **P;
  P = malloc(n * sizeof(void *));
  for (int i = 0; i < n; ++i) {
    P[i] = calloc_(15, 1);
  }
  bulk_free(P, n);
  free(P);
}

BENCH(arena, bench) {
  EZBENCH2("A 100", donothing, A(100));
  EZBENCH2("B 100", donothing, B(100));
  EZBENCH2("A 5000", donothing, A(5000));
  EZBENCH2("B 5000", donothing, B(5000));
}
