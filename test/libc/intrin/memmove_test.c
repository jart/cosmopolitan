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
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

static void *golden(void *a, const void *b, size_t n) {
  size_t i;
  char *d = a;
  const char *s = b;
  if (d > s) {
    for (i = n; i--;) {
      d[i] = s[i];
    }
  } else {
    for (i = 0; i < n; ++i) {
      d[i] = s[i];
    }
  }
  return d;
}

TEST(memmove, hug) {
  char *a, *b;
  int i, o1, o2;
  int N[] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,   13,   14,
             15, 16, 17, 18, 31, 32, 33, 63, 64, 65, 80, 81, 1000, 1024, 1025};
  a = gc(malloc(1104));
  b = gc(malloc(1104));
  for (o1 = 0; o1 < 48; ++o1) {
    for (o2 = 0; o2 < 48; ++o2) {
      for (i = 0; i < ARRAYLEN(N); ++i) {
        rngset(a, 1104, 0, 0);
        memcpy(b, a, 1104);
        ASSERT_EQ(a + o2, golden(a + o2, a + o1, N[i]));
        ASSERT_EQ(b + o2, memmove(b + o2, b + o1, N[i]));
        ASSERT_EQ(0, timingsafe_bcmp(a, b, 1104), "%d %d %d", o1, o2, i);
      }
    }
  }
}

TEST(memmove, bighug) {
  char *a, *b;
  int i, o1, o2;
  int N[] = {5 * 1024 * 1024};
  a = gc(malloc(6291456));
  b = gc(malloc(6291456));
  for (o1 = 0; o1 < 40; o1 += 20) {
    for (o2 = 0; o2 < 40; o2 += 20) {
      for (i = 0; i < ARRAYLEN(N); ++i) {
        rngset(a, 6291456, 0, 0);
        memcpy(b, a, 6291456);
        ASSERT_EQ(a + o2, golden(a + o2, a + o1, N[i]));
        ASSERT_EQ(b + o2, memmove(b + o2, b + o1, N[i]), "%d", N[i]);
        ASSERT_EQ(0, timingsafe_bcmp(a, b, 6291456), "%d %d %d", o1, o2, i);
      }
    }
  }
}

TEST(memmove, pageOverlapTorture) {
  long pagesz = sysconf(_SC_PAGESIZE);
  char *map = mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  char *map2 = mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  ASSERT_SYS(0, 0, mprotect(map + pagesz, pagesz, PROT_NONE));
  ASSERT_SYS(0, 0, mprotect(map2 + pagesz, pagesz, PROT_NONE));
  strcpy(map + pagesz - 9, "12345678");
  strcpy(map2 + pagesz - 9, "12345679");
  __expropriate(memmove(map + pagesz - 9, map2 + pagesz - 9, 9));
  EXPECT_SYS(0, 0, munmap(map2, pagesz * 2));
  EXPECT_SYS(0, 0, munmap(map, pagesz * 2));
}

BENCH(memmove, bench) {
  int n, max = 128 * 1024 * 1024;
  char *volatile p = gc(calloc(max, 1));
  char *volatile q = gc(calloc(max, 1));
  EZBENCH_N("memmove", 0, memmove(p, q, 0));
  for (n = 0; n < 127; ++n) {
    EZBENCH_N("memmove", n, memmove(p, q, n));
  }
  for (n = 128; n <= max; n *= 2) {
    EZBENCH_N("memmove", n - 1, memmove(p, q, n - 1));
    EZBENCH_N("memmove", n, memmove(p, q, n));
  }
  for (n = 500; n <= 1000; n += 100) {
    EZBENCH_N("memmove", n, memmove(p, q, n));
  }
}
