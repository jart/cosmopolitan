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
#include "libc/bsdstdlib.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define N 1024
#define M 20

TEST(malloc, zero) {
  char *p;
  ASSERT_NE(NULL, (p = malloc(0)));
  free(p);
}

TEST(realloc, bothAreZero_createsMinimalAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = realloc(0, 0)));
  free(p);
}

#ifndef COSMO_MEM_DEBUG
TEST(realloc, ptrIsZero_createsAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = realloc(0, 1)));
  ASSERT_EQ(p, realloc(p, 0));
  free(p);
}
#endif

#ifndef COSMO_MEM_DEBUG
TEST(realloc, sizeIsZero_shrinksAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = malloc(1)));
  ASSERT_EQ(p, realloc(p, 0));
  free(p);
}
#endif

#ifndef COSMO_MEM_DEBUG
TEST(realloc_in_place, test) {
  char *x = malloc(16);
  EXPECT_EQ(x, realloc_in_place(x, 0));
  EXPECT_EQ(x, realloc_in_place(x, 1));
  *x = 2;
  free(x);
}
#endif

BENCH(realloc_in_place, bench) {
  volatile int i = 1000;
  char *volatile x = malloc(i);
  EZBENCH2("malloc", donothing, free(malloc(i)));
  EZBENCH2("memalign", donothing, free(memalign(16, i)));
  EZBENCH2("memalign", donothing, free(memalign(32, i)));
  EZBENCH2("realloc", donothing, x = realloc(x, --i));
  EZBENCH2("realloc_in_place", donothing, realloc_in_place(x, --i));
  free(x);
}

void AppendStuff(char **p, size_t *n) {
  char buf[512];
  ASSERT_NE(NULL, (*p = realloc(*p, (*n += 512))));
  arc4random_buf(buf, sizeof(buf));
  memcpy(*p + *n - sizeof(buf), buf, sizeof(buf));
}

TEST(malloc, test) {
  size_t n;
  char *big = 0;
  static struct stat st;
  static int *A[4096], *maps[M];
  static int i, j, k, fds[M], mapsizes[M];
  memset(fds, -1, sizeof(fds));
  memset(maps, -1, sizeof(maps));
  for (i = 0; i < N * M; ++i) {
    /* AppendStuff(&big, &bigsize); */
    j = rand() % ARRAYLEN(A);
    if (A[j]) {
      ASSERT_EQ(j, A[j][0]);
      n = rand() % N;
      n = MAX(n, 1);
      n = ROUNDUP(n, sizeof(int));
      A[j] = realloc(A[j], n);
      ASSERT_NE(NULL, A[j]);
      ASSERT_EQ(j, A[j][0]);
      free(A[j]);
      A[j] = NULL;
    } else {
      n = rand() % N;
      n = MAX(n, 1);
      n = ROUNDUP(n, sizeof(int));
      A[j] = malloc(n);
      ASSERT_NE(NULL, A[j]);
      A[j][0] = j;
    }
    if (!(i % M)) {
      k = rand() % M;
      if (fds[k] == -1) {
        ASSERT_NE(-1, (fds[k] = open(program_invocation_name, O_RDONLY)));
        ASSERT_NE(-1, fstat(fds[k], &st));
        mapsizes[k] = st.st_size;
        ASSERT_NE(MAP_FAILED, (maps[k] = mmap(NULL, mapsizes[k], PROT_READ,
                                              MAP_SHARED, fds[k], 0)));
      } else {
        ASSERT_SYS(0, 0, munmap(maps[k], mapsizes[k]));
        maps[k] = MAP_FAILED;
        ASSERT_SYS(0, 0, close(fds[k]));
        fds[k] = -1;
      }
    }
  }
  free(big);
  for (i = 0; i < ARRAYLEN(A); ++i)
    free(A[i]);
  for (i = 0; i < ARRAYLEN(maps); ++i)
    if (maps[i] != MAP_FAILED)
      ASSERT_SYS(0, 0, munmap(maps[i], mapsizes[i]));
  for (i = 0; i < ARRAYLEN(fds); ++i)
    if (fds[i] != -1)
      ASSERT_SYS(0, 0, close(fds[i]));
  ASSERT_EQ(0, errno);
}

TEST(memalign, roundsUpAlignmentToTwoPower) {
  char *volatile p = memalign(129, 1);
  ASSERT_EQ(0, (intptr_t)p & 255);
  free(p);
}

void *bulk[1024];

void BulkFreeBenchSetup(void) {
  size_t i;
  for (i = 0; i < ARRAYLEN(bulk); ++i) {
    bulk[i] = rand() % 64 ? malloc(rand() % 64) : 0;
  }
}

void FreeBulk(void) {
  size_t i;
  for (i = 0; i < ARRAYLEN(bulk); ++i) {
    free(bulk[i]);
  }
}

void MallocFree(void) {
  char *volatile p;
  p = malloc(16);
  free(p);
}

void *worker(void *arg) {
  return 0;
}

TEST(again, dlmalloc) {

  /* // force __isthreaded to 2 */
  /* pthread_t th; */
  /* pthread_create(&th, 0, worker, 0); */
  /* pthread_join(th, 0); */

  volatile int i = 1;
  char *volatile x = malloc(10000);
  BENCHMARK(10000, 1, free(malloc(i)));
  BENCHMARK(10000, 1, free(memalign(16, i)));
  BENCHMARK(10000, 1, free(memalign(32, i)));
  i = 10000;
  BENCHMARK(10000, 1, x = realloc(x, --i));
  BENCHMARK(10000, 1, realloc_in_place(x, --i));
  free(x);
}
