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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"

#define N 1024
#define M 20

TEST(malloc, zero) {
  char *p;
  ASSERT_NE(NULL, (p = malloc(0)));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p, 1));
  free(p);
}

TEST(realloc, bothAreZero_createsMinimalAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = realloc(0, 0)));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p, 1));
  free(p);
}

TEST(realloc, ptrIsZero_createsAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = realloc(0, 1)));
  if (IsAsan()) ASSERT_TRUE(__asan_is_valid(p, 1));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p + 1, 1));
  ASSERT_EQ(p, realloc(p, 0));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p, 1));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p + 1, 1));
  free(p);
}

TEST(realloc, sizeIsZero_shrinksAllocation) {
  char *p;
  ASSERT_NE(NULL, (p = malloc(1)));
  if (IsAsan()) ASSERT_TRUE(__asan_is_valid(p, 1));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p + 1, 1));
  ASSERT_EQ(p, realloc(p, 0));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p, 1));
  if (IsAsan()) ASSERT_FALSE(__asan_is_valid(p + 1, 1));
  free(p);
}

TEST(realloc_in_place, test) {
  char *x = malloc(16);
  EXPECT_EQ(x, realloc_in_place(x, 0));
  EXPECT_EQ(x, realloc_in_place(x, 1));
  *x = 2;
  free(x);
}

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
  rngset(buf, sizeof(buf), 0, 0);
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
        ASSERT_NE(-1, munmap(maps[k], mapsizes[k]));
        ASSERT_NE(-1, close(fds[k]));
        fds[k] = -1;
      }
    }
  }
  free(big);
  for (i = 0; i < ARRAYLEN(A); ++i) free(A[i]);
  for (i = 0; i < ARRAYLEN(maps); ++i) munmap(maps[i], mapsizes[i]);
  for (i = 0; i < ARRAYLEN(fds); ++i) close(fds[i]);
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
    bulk[i] = malloc(rand() % 64);
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

BENCH(bulk_free, bench) {
  EZBENCH2("free() bulk", BulkFreeBenchSetup(), FreeBulk());
  EZBENCH2("bulk_free()", BulkFreeBenchSetup(),
           bulk_free(bulk, ARRAYLEN(bulk)));
  EZBENCH2("free(malloc(16)) ST", donothing, MallocFree());
  __enable_threads();
  EZBENCH2("free(malloc(16)) MT", donothing, MallocFree());
}

#define ITERATIONS 10000

void *Worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
    char *p;
    ASSERT_NE(NULL, (p = malloc(lemur64() % 128)));
    ASSERT_NE(NULL, (p = realloc(p, max(lemur64() % 128, 1))));
    free(p);
  }
  return 0;
}

BENCH(malloc, torture) {
  int i, n = __get_cpu_count() * 2;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  if (!n) return;
  printf("\nmalloc torture test w/ %d threads and %d iterations\n", n,
         ITERATIONS);
  SPAWN(fork);
  struct timespec t1 = timespec_real();
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  }
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
  struct timespec t2 = timespec_real();
  printf("consumed %g wall and %g cpu seconds\n",
         timespec_tomicros(timespec_sub(t2, t1)) * 1e-6,
         (double)clock() / CLOCKS_PER_SEC);
  EXITS(0);
}
