/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(memcpy, test) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1, memcpy(b1, b2, n), "%ld\n\t%#.*s\n\t%#.*s", n, n, b1, n, b2);
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
  for (unsigned n = kHalfCache3 - 1; n < kHalfCache3 + 2; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1, memcpy(b1, b2, n), "%ld\n\t%#.*s\n\t%#.*s", n, n, b1, n, b2);
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
}

TEST(mempcpy, test) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1 + n, mempcpy(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
  for (unsigned n = kHalfCache3 - 1; n < kHalfCache3 + 2; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1 + n, mempcpy(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
}

TEST(memcpy, direct) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1, (memcpy)(b1, b2, n), "%ld\n\t%#.*s\n\t%#.*s", n, n, b1, n,
              b2);
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
  for (unsigned n = kHalfCache3 - 1; n < kHalfCache3 + 2; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1, (memcpy)(b1, b2, n), "%ld\n\t%#.*s\n\t%#.*s", n, n, b1, n,
              b2);
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
}

TEST(mempcpy, direct) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1 + n, (mempcpy)(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
  for (unsigned n = kHalfCache3 - 1; n < kHalfCache3 + 2; ++n) {
    b1 = malloc(n);
    b2 = malloc(n);
    rngset(b1, n, _rand64, -1);
    rngset(b2, n, _rand64, -1);
    ASSERT_EQ(b1 + n, (mempcpy)(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    free(b2);
    free(b1);
  }
}

TEST(memcpy, overlapping_isFineIfCopyingBackwards) {
  for (size_t i = 0; i < 32; ++i) {
    char *b1 = malloc(64 + i);
    char *b2 = malloc(64 + i);
    memcpy(b1, b2, 64);
    memcpy(b1, b1 + i, 64 - i);
    memmove(b2, b2 + i, 64 - i);
    ASSERT_EQ(0, memcmp(b1, b2, 64));
    free(b2);
    free(b1);
  }
}

#define B(F, N)                                                    \
  do {                                                             \
    char *d = rngset(malloc(N), N, _rand64, -1);                   \
    char *s = rngset(malloc(N), N, _rand64, -1);                   \
    EZBENCH2(#F " " #N, donothing,                                 \
             __expropriate(F(__veil("r", d), __veil("r", s), N))); \
    free(d);                                                       \
    free(s);                                                       \
  } while (0)

void BB(size_t N) {
  B(memcpy, N);
  (fprintf)(stderr, "\n");
}

BENCH(memcpy, bench) {
  BB(0);
  BB(1);
  BB(2);
  BB(3);
  BB(7);
  BB(8);
  BB(15);
  BB(16);
  BB(31);
  BB(32);
  BB(63);
  BB(64);
  BB(255);
  BB(256);
  BB(1023);
  BB(1024);
  BB(4096);
  BB(FRAMESIZE);
}
