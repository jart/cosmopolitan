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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

size_t todd(char *dst, const char *src, size_t dsize) {
  const char *osrc = src;
  size_t nleft = dsize;
  if (nleft != 0)
    while (--nleft != 0)
      if ((*dst++ = *src++) == '\0')
        break;
  if (nleft == 0) {
    if (dsize != 0)
      *dst = '\0';
    while (*src++)
      ;
  }
  return src - osrc - 1;
}

TEST(strlcpy, fuzz) {
  int pagesz = sysconf(_SC_PAGESIZE);
  char *map1 = (char *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map1 != MAP_FAILED);
  npassert(!mprotect(map1 + pagesz, pagesz, PROT_NONE));
  char *map2 = (char *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map2 != MAP_FAILED);
  npassert(!mprotect(map2 + pagesz, pagesz, PROT_NONE));
  char *map3 = (char *)mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map3 != MAP_FAILED);
  npassert(!mprotect(map3 + pagesz, pagesz, PROT_NONE));
  for (int dsize = 1; dsize < 128; ++dsize) {
    char *volatile dst1 = map1 + pagesz - dsize;
    char *volatile dst2 = map1 + pagesz - dsize;
    for (int i = 0; i < dsize; ++i)
      dst1[i] = dst2[i] = max(rand() & 255, 1);
    for (int ssize = 1; ssize < dsize * 2; ++ssize) {
      char *volatile src = map3 + pagesz - (ssize + 1);
      for (int i = 0; i < ssize; ++i)
        src[i] = max(rand() & 255, 1);
      src[ssize] = 0;
      ASSERT_EQ(todd(dst1, src, dsize), strlcpy(dst2, src, dsize));
      ASSERT_EQ(0, memcmp(dst1, dst2, dsize));
    }
  }
  npassert(!munmap(map3, pagesz * 2));
  npassert(!munmap(map2, pagesz * 2));
  npassert(!munmap(map1, pagesz * 2));
}

TEST(strlcpy, testEmptyBuffer_doesNothing) {
  EXPECT_EQ(5, strlcpy(NULL, "hello", 0));
}

TEST(strlcpy, testShortBuffer_nulTerminates) {
  char buf[1] = {1};
  EXPECT_EQ(5, strlcpy(buf, "hello", 1));
  EXPECT_STREQ("", buf);
}

TEST(strlcpy, testShortBuffer_copies) {
  char buf[2] = {1, 1};
  EXPECT_EQ(5, strlcpy(buf, "hello", 2));
  EXPECT_STREQ("h", buf);
}

#define N 4096

BENCH(strlcpy, bench) {
  char dst[N];
  char src[N + 1];

  printf("\n");
  for (int n = 1; n <= N; n *= 2) {
    for (int i = 0; i < n; ++i)
      src[i] = max(rand() & 255, 1);
    src[n] = 0;
    BENCHMARK(100, n, X(strlcpy(dst, src, V(N))));
  }

  printf("\n");
  for (int n = 1; n <= N; n *= 2) {
    for (int i = 0; i < n; ++i)
      src[i] = max(rand() & 255, 1);
    src[n] = 0;
    BENCHMARK(100, n, X(todd(dst, src, V(N))));
  }
}
