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
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/fastrandomstring.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

int golden(const void *p, const void *q, size_t n) {
  int c;
  size_t i;
  for (i = 0; i < n; ++i) {
    if ((c = ((const unsigned char *)p)[i] - ((const unsigned char *)q)[i])) {
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

TEST(memcmp, hug) {
  int i, j, g;
  char a[128] = {0};
  char b[128] = {0};
  for (j = 0; j < 128; ++j) {
    for (i = 0; i < j; ++i) {
      a[i] = 1;
      g = golden(a, b, j);
      ASSERT_EQ(g, memcmp(a, b, j), "i=%d j=%d", i, j);
      ASSERT_EQ(!!g, !!bcmp(a, b, j), "i=%d j=%d", i, j);
      ASSERT_EQ(!!g, !!timingsafe_bcmp(a, b, j), "i=%d", i);
      ASSERT_EQ(MAX(-1, MIN(1, g)), timingsafe_memcmp(a, b, j), "i=%d", i);
      a[i] = 0;
    }
  }
}

TEST(memcmp, fuzz) {
  int i, o, n, g;
  char a[256], b[256];
  for (i = 0; i < 100000; ++i) {
    rngset(a, sizeof(a), _rand64, -1);
    memcpy(b, a, sizeof(a));
    if (rand() & 1) {
      a[rand() % sizeof(a)] += rand();
    } else {
      b[rand() % sizeof(a)] += rand();
    }
    if (rand() & 1) {
      a[rand() % sizeof(a)] += rand();
    } else {
      b[rand() % sizeof(a)] += rand();
    }
    o = rand() & 31;
    n = rand() % (sizeof(a) - o);
    g = golden(a + o, b + o, n);
    ASSERT_EQ(g, memcmp(a + o, b + o, n), "n=%d o=%d", n, o);
    ASSERT_EQ(!!g, !!bcmp(a + o, b + o, n), "n=%d o=%d", n, o);
    ASSERT_EQ(!!g, !!timingsafe_bcmp(a + o, b + o, n), "n=%d o=%d", n, o);
    ASSERT_EQ(MAX(-1, MIN(1, g)), timingsafe_memcmp(a + o, b + o, n),
              "n=%d o=%d", n, o);
  }
}

int buncmp(const void *, const void *, size_t) asm("bcmp");
int funcmp(const void *, const void *, size_t) asm("memcmp");

#if 0
BENCH(bcmp, bench) {
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  b = gc(strdup(b));
  EZBENCH_N("bcmp", 0, v = buncmp(a, b, 0));
  EZBENCH_N("bcmp", 1, v = buncmp(a, b, 1));
  EZBENCH_N("bcmp", 2, v = buncmp(a, b, 2));
  EZBENCH_N("bcmp", 3, v = buncmp(a, b, 3));
  EZBENCH_N("𝗯𝗰𝗺𝗽", 4, v = buncmp(a, b, 4));
  EZBENCH_N("bcmp", 5, v = buncmp(a, b, 5));
  EZBENCH_N("bcmp", 6, v = buncmp(a, b, 6));
  EZBENCH_N("bcmp", 7, v = buncmp(a, b, 7));
  EZBENCH_N("𝗯𝗰𝗺𝗽", 8, v = buncmp(a, b, 8));
  EZBENCH_N("bcmp", 9, v = buncmp(a, b, 9));
  EZBENCH_N("bcmp", 15, v = buncmp(a, b, 15));
  EZBENCH_N("𝗯𝗰𝗺𝗽", 16, v = buncmp(a, b, 16));
  EZBENCH_N("bcmp", 17, v = buncmp(a, b, 17));
  EZBENCH_N("bcmp", 31, v = buncmp(a, b, 31));
  EZBENCH_N("bcmp", 32, v = buncmp(a, b, 32));
  a = kHyperion;
  b = gc(strdup(kHyperion));
  EZBENCH_N("bcmp", 33, v = buncmp(a, b, 33));
  EZBENCH_N("bcmp", 79, v = buncmp(a, b, 79));
  EZBENCH_N("𝗯𝗰𝗺𝗽", 80, v = buncmp(a, b, 80));
  EZBENCH_N("bcmp", 128, v = buncmp(a, b, 128));
  EZBENCH_N("bcmp", 256, v = buncmp(a, b, 256));
  a = gc(malloc(16 * 1024));
  b = gc(malloc(16 * 1024));
  rngset(a, 16 * 1024, lemur64, -1);
  memcpy(b, a, 16 * 1024);
  EZBENCH_N("bcmp", 16384, v = buncmp(a, b, 16384));
  a = gc(malloc(32 * 1024));
  b = gc(malloc(32 * 1024));
  rngset(a, 32 * 1024, lemur64, -1);
  memcpy(b, a, 32 * 1024);
  EZBENCH_N("bcmp", 32768, v = buncmp(a, b, 32768));
  a = gc(malloc(128 * 1024));
  b = gc(malloc(128 * 1024));
  rngset(a, 128 * 1024, lemur64, -1);
  memcpy(b, a, 128 * 1024);
  EZBENCH_N("bcmp", 131072, v = buncmp(a, b, 131072));
}
#endif

#if 0
BENCH(memcmp, bench) {
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  b = gc(strdup(b));
  EZBENCH_N("memcmp", 0, v = funcmp(a, b, 0));
  EZBENCH_N("memcmp", 1, v = funcmp(a, b, 1));
  EZBENCH_N("memcmp", 2, v = funcmp(a, b, 2));
  EZBENCH_N("memcmp", 3, v = funcmp(a, b, 3));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗺𝗽", 4, v = funcmp(a, b, 4));
  EZBENCH_N("memcmp", 5, v = funcmp(a, b, 5));
  EZBENCH_N("memcmp", 6, v = funcmp(a, b, 6));
  EZBENCH_N("memcmp", 7, v = funcmp(a, b, 7));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗺𝗽", 8, v = funcmp(a, b, 8));
  EZBENCH_N("memcmp", 9, v = funcmp(a, b, 9));
  EZBENCH_N("memcmp", 15, v = funcmp(a, b, 15));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗺𝗽", 16, v = funcmp(a, b, 16));
  EZBENCH_N("memcmp", 17, v = funcmp(a, b, 17));
  EZBENCH_N("memcmp", 31, v = funcmp(a, b, 31));
  EZBENCH_N("memcmp", 32, v = funcmp(a, b, 32));
  a = kHyperion;
  b = gc(strdup(kHyperion));
  EZBENCH_N("memcmp", 33, v = funcmp(a, b, 33));
  EZBENCH_N("memcmp", 79, v = funcmp(a, b, 79));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗺𝗽", 80, v = funcmp(a, b, 80));
  EZBENCH_N("memcmp", 128, v = funcmp(a, b, 128));
  EZBENCH_N("memcmp", 256, v = funcmp(a, b, 256));
  a = gc(malloc(16 * 1024));
  b = gc(malloc(16 * 1024));
  rngset(a, 16 * 1024, lemur64, -1);
  memcpy(b, a, 16 * 1024);
  EZBENCH_N("memcmp", 16384, v = funcmp(a, b, 16384));
  a = gc(malloc(32 * 1024));
  b = gc(malloc(32 * 1024));
  rngset(a, 32 * 1024, lemur64, -1);
  memcpy(b, a, 32 * 1024);
  EZBENCH_N("memcmp", 32768, v = funcmp(a, b, 32768));
  a = gc(malloc(128 * 1024));
  b = gc(malloc(128 * 1024));
  rngset(a, 128 * 1024, lemur64, -1);
  memcpy(b, a, 128 * 1024);
  EZBENCH_N("memcmp", 131072, v = funcmp(a, b, 131072));
}
#endif

#if 0
BENCH(timingsafe_memcmp, bench) {
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  b = gc(strdup(b));
  EZBENCH_N("timingsafe_memcmp", 0, v = timingsafe_memcmp(a, b, 0));
  EZBENCH_N("timingsafe_memcmp", 1, v = timingsafe_memcmp(a, b, 1));
  EZBENCH_N("timingsafe_memcmp", 2, v = timingsafe_memcmp(a, b, 2));
  EZBENCH_N("timingsafe_memcmp", 3, v = timingsafe_memcmp(a, b, 3));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗺𝗲𝗺𝗰𝗺𝗽", 4, v = timingsafe_memcmp(a, b, 4));
  EZBENCH_N("timingsafe_memcmp", 5, v = timingsafe_memcmp(a, b, 5));
  EZBENCH_N("timingsafe_memcmp", 6, v = timingsafe_memcmp(a, b, 6));
  EZBENCH_N("timingsafe_memcmp", 7, v = timingsafe_memcmp(a, b, 7));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗺𝗲𝗺𝗰𝗺𝗽", 8, v = timingsafe_memcmp(a, b, 8));
  EZBENCH_N("timingsafe_memcmp", 9, v = timingsafe_memcmp(a, b, 9));
  EZBENCH_N("timingsafe_memcmp", 15, v = timingsafe_memcmp(a, b, 15));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗺𝗲𝗺𝗰𝗺𝗽", 16, v = timingsafe_memcmp(a, b, 16));
  EZBENCH_N("timingsafe_memcmp", 17, v = timingsafe_memcmp(a, b, 17));
  EZBENCH_N("timingsafe_memcmp", 31, v = timingsafe_memcmp(a, b, 31));
  EZBENCH_N("timingsafe_memcmp", 32, v = timingsafe_memcmp(a, b, 32));
  a = kHyperion;
  b = gc(strdup(kHyperion));
  EZBENCH_N("timingsafe_memcmp", 33, v = timingsafe_memcmp(a, b, 33));
  EZBENCH_N("timingsafe_memcmp", 79, v = timingsafe_memcmp(a, b, 79));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗺𝗲𝗺𝗰𝗺𝗽", 80, v = timingsafe_memcmp(a, b, 80));
  EZBENCH_N("timingsafe_memcmp", 128, v = timingsafe_memcmp(a, b, 128));
  EZBENCH_N("timingsafe_memcmp", 256, v = timingsafe_memcmp(a, b, 256));
  a = gc(malloc(16 * 1024));
  b = gc(malloc(16 * 1024));
  rngset(a, 16 * 1024, lemur64, -1);
  memcpy(b, a, 16 * 1024);
  EZBENCH_N("timingsafe_memcmp", 16384, v = timingsafe_memcmp(a, b, 16384));
  a = gc(malloc(32 * 1024));
  b = gc(malloc(32 * 1024));
  rngset(a, 32 * 1024, lemur64, -1);
  memcpy(b, a, 32 * 1024);
  EZBENCH_N("timingsafe_memcmp", 32768, v = timingsafe_memcmp(a, b, 32768));
  a = gc(malloc(128 * 1024));
  b = gc(malloc(128 * 1024));
  rngset(a, 128 * 1024, lemur64, -1);
  memcpy(b, a, 128 * 1024);
  EZBENCH_N("timingsafe_memcmp", 131072, v = timingsafe_memcmp(a, b, 131072));
}
#endif

#if 0
BENCH(timingsafe_bcmp, bench) {
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  b = gc(strdup(b));
  EZBENCH_N("timingsafe_bcmp", 0, v = timingsafe_bcmp(a, b, 0));
  EZBENCH_N("timingsafe_bcmp", 1, v = timingsafe_bcmp(a, b, 1));
  EZBENCH_N("timingsafe_bcmp", 2, v = timingsafe_bcmp(a, b, 2));
  EZBENCH_N("timingsafe_bcmp", 3, v = timingsafe_bcmp(a, b, 3));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗯𝗰𝗺𝗽", 4, v = timingsafe_bcmp(a, b, 4));
  EZBENCH_N("timingsafe_bcmp", 5, v = timingsafe_bcmp(a, b, 5));
  EZBENCH_N("timingsafe_bcmp", 6, v = timingsafe_bcmp(a, b, 6));
  EZBENCH_N("timingsafe_bcmp", 7, v = timingsafe_bcmp(a, b, 7));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗯𝗰𝗺𝗽", 8, v = timingsafe_bcmp(a, b, 8));
  EZBENCH_N("timingsafe_bcmp", 9, v = timingsafe_bcmp(a, b, 9));
  EZBENCH_N("timingsafe_bcmp", 15, v = timingsafe_bcmp(a, b, 15));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗯𝗰𝗺𝗽", 16, v = timingsafe_bcmp(a, b, 16));
  EZBENCH_N("timingsafe_bcmp", 17, v = timingsafe_bcmp(a, b, 17));
  EZBENCH_N("timingsafe_bcmp", 31, v = timingsafe_bcmp(a, b, 31));
  EZBENCH_N("timingsafe_bcmp", 32, v = timingsafe_bcmp(a, b, 32));
  a = kHyperion;
  b = gc(strdup(kHyperion));
  EZBENCH_N("timingsafe_bcmp", 33, v = timingsafe_bcmp(a, b, 33));
  EZBENCH_N("timingsafe_bcmp", 79, v = timingsafe_bcmp(a, b, 79));
  EZBENCH_N("𝘁𝗶𝗺𝗶𝗻𝗴𝘀𝗮𝗳𝗲_𝗯𝗰𝗺𝗽", 80, v = timingsafe_bcmp(a, b, 80));
  EZBENCH_N("timingsafe_bcmp", 128, v = timingsafe_bcmp(a, b, 128));
  EZBENCH_N("timingsafe_bcmp", 256, v = timingsafe_bcmp(a, b, 256));
  a = gc(malloc(16 * 1024));
  b = gc(malloc(16 * 1024));
  rngset(a, 16 * 1024, lemur64, -1);
  memcpy(b, a, 16 * 1024);
  EZBENCH_N("timingsafe_bcmp", 16384, v = timingsafe_bcmp(a, b, 16384));
  a = gc(malloc(32 * 1024));
  b = gc(malloc(32 * 1024));
  rngset(a, 32 * 1024, lemur64, -1);
  memcpy(b, a, 32 * 1024);
  EZBENCH_N("timingsafe_bcmp", 32768, v = timingsafe_bcmp(a, b, 32768));
  a = gc(malloc(128 * 1024));
  b = gc(malloc(128 * 1024));
  rngset(a, 128 * 1024, lemur64, -1);
  memcpy(b, a, 128 * 1024);
  EZBENCH_N("timingsafe_bcmp", 131072, v = timingsafe_bcmp(a, b, 131072));
}
#endif

#if 0
BENCH(memcasecmp, bench) {
  volatile int v;
  const char *volatile a;
  const char *volatile b;
  b = a = "123456789123456789123456789123456789123456789123456789";
  b = gc(strdup(b));
  EZBENCH_N("memcasecmp", 0, v = memcasecmp(a, b, 0));
  EZBENCH_N("memcasecmp", 1, v = memcasecmp(a, b, 1));
  EZBENCH_N("memcasecmp", 2, v = memcasecmp(a, b, 2));
  EZBENCH_N("memcasecmp", 3, v = memcasecmp(a, b, 3));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗮𝘀𝗲𝗰𝗺𝗽", 4, v = memcasecmp(a, b, 4));
  EZBENCH_N("memcasecmp", 5, v = memcasecmp(a, b, 5));
  EZBENCH_N("memcasecmp", 6, v = memcasecmp(a, b, 6));
  EZBENCH_N("memcasecmp", 7, v = memcasecmp(a, b, 7));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗮𝘀𝗲𝗰𝗺𝗽", 8, v = memcasecmp(a, b, 8));
  EZBENCH_N("memcasecmp", 9, v = memcasecmp(a, b, 9));
  EZBENCH_N("memcasecmp", 15, v = memcasecmp(a, b, 15));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗮𝘀𝗲𝗰𝗺𝗽", 16, v = memcasecmp(a, b, 16));
  EZBENCH_N("memcasecmp", 17, v = memcasecmp(a, b, 17));
  EZBENCH_N("memcasecmp", 31, v = memcasecmp(a, b, 31));
  EZBENCH_N("memcasecmp", 32, v = memcasecmp(a, b, 32));
  a = kHyperion;
  b = gc(strdup(kHyperion));
  EZBENCH_N("memcasecmp", 33, v = memcasecmp(a, b, 33));
  EZBENCH_N("memcasecmp", 79, v = memcasecmp(a, b, 79));
  EZBENCH_N("𝗺𝗲𝗺𝗰𝗮𝘀𝗲𝗰𝗺𝗽", 80, v = memcasecmp(a, b, 80));
  EZBENCH_N("memcasecmp", 128, v = memcasecmp(a, b, 128));
  EZBENCH_N("memcasecmp", 256, v = memcasecmp(a, b, 256));
  a = gc(malloc(16 * 1024));
  b = gc(malloc(16 * 1024));
  rngset(a, 16 * 1024, lemur64, -1);
  memcpy(b, a, 16 * 1024);
  EZBENCH_N("memcasecmp", 16384, v = memcasecmp(a, b, 16384));
  a = gc(malloc(32 * 1024));
  b = gc(malloc(32 * 1024));
  rngset(a, 32 * 1024, lemur64, -1);
  memcpy(b, a, 32 * 1024);
  EZBENCH_N("memcasecmp", 32768, v = memcasecmp(a, b, 32768));
  a = gc(malloc(128 * 1024));
  b = gc(malloc(128 * 1024));
  rngset(a, 128 * 1024, lemur64, -1);
  memcpy(b, a, 128 * 1024);
  EZBENCH_N("memcasecmp", 131072, v = memcasecmp(a, b, 131072));
}
#endif

#if 0
BENCH(timingsafe_memcmp, demonstration) {
  int bcmp_(const void *, const void *, size_t) asm("bcmp");
  int memcmp_(const void *, const void *, size_t) asm("memcmp");
  char a[256], b[256];
  rngset(a, 256, lemur64, -1);
  memcpy(b, a, 256);
  ++a[0];
  EZBENCH_N("bcmp ne", 256, bcmp_(a, b, 256));
  a[0] = b[0];
  EZBENCH_N("bcmp eq", 256, bcmp_(a, b, 256));
  ++a[0];
  EZBENCH_N("memcmp ne", 256, memcmp_(a, b, 256));
  a[0] = b[0];
  EZBENCH_N("memcmp eq", 256, memcmp_(a, b, 256));
  ++a[0];
  EZBENCH_N("timingsafe_bcmp ne", 256, timingsafe_bcmp(a, b, 256));
  a[0] = b[0];
  EZBENCH_N("timingsafe_bcmp eq", 256, timingsafe_bcmp(a, b, 256));
  ++a[0];
  EZBENCH_N("timingsafe_memcmp ne", 256, timingsafe_memcmp(a, b, 256));
  a[0] = b[0];
  EZBENCH_N("timingsafe_memcmp eq", 256, timingsafe_memcmp(a, b, 256));
}
#endif
