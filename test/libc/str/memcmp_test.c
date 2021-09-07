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
#include "libc/rand/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/fastrandomstring.h"
#include "libc/testlib/testlib.h"

int unfancy_memcmp(const void *a, const void *b, size_t n) {
  int c;
  size_t i;
  const unsigned char *x, *y;
  for (x = a, y = b, i = 0; i < n; ++i) {
    if ((c = x[i] - y[i])) {
      return c;
    }
  }
  return 0;
}

TEST(memcmp, test) {
  EXPECT_EQ(0, memcmp("hi", "hi", 2));
  EXPECT_NE(0, memcmp("hi", "HI", 2));
  EXPECT_EQ(-1, memcmp("a", "b", 1));
  EXPECT_EQ(+1, memcmp("b", "a", 1));
}

TEST(memcmp, fuzz) {
  int i, n;
  char a[32], b[32];
  for (i = 0; i < 1000; ++i) {
    n = rand() & 31;
    rngset(a, n, rand64, -1);
    rngset(b, n, rand64, -1);
    ASSERT_EQ(unfancy_memcmp(a, b, n), memcmp(a, b, n), "n=%d", n);
  }
}

BENCH(memcmp, bench) {
  extern int funcmp(const void *a, const void *b, size_t n) asm("memcmp");
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  EZBENCH2("memcmp same", donothing, v = funcmp(a, b, 7));
  b = gc(strdup(b));
  EZBENCH2("memcmp 1", donothing, v = funcmp(a, b, 1));
  EZBENCH2("memcmp 2", donothing, v = funcmp(a, b, 2));
  EZBENCH2("memcmp 3", donothing, v = funcmp(a, b, 3));
  EZBENCH2("memcmp 4", donothing, v = funcmp(a, b, 4));
  EZBENCH2("memcmp 5", donothing, v = funcmp(a, b, 5));
  EZBENCH2("memcmp 6", donothing, v = funcmp(a, b, 6));
  EZBENCH2("memcmp 7", donothing, v = funcmp(a, b, 7));
  EZBENCH2("memcmp 8", donothing, v = funcmp(a, b, 8));
  EZBENCH2("memcmp 9", donothing, v = funcmp(a, b, 9));
  EZBENCH2("memcmp 16", donothing, v = funcmp(a, b, 16));
  EZBENCH2("memcmp 32", donothing, v = funcmp(a, b, 32));
}
