/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

#define N 1024
#define M 20

void SetUp(void) {
  // TODO(jart): what is wrong?
  if (IsWindows()) exit(0);
}

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath", 0));
}

TEST(malloc, zeroMeansOne) {
  ASSERT_GE(malloc_usable_size(gc(malloc(0))), 1);
}

TEST(calloc, zerosMeansOne) {
  ASSERT_GE(malloc_usable_size(gc(calloc(0, 0))), 1);
}

void AppendStuff(char **p, size_t *n) {
  char buf[512];
  ASSERT_NE(NULL, (*p = realloc(*p, (*n += 512))));
  rngset(buf, sizeof(buf), 0, 0);
  memcpy(*p + *n - sizeof(buf), buf, sizeof(buf));
}

TEST(malloc, test) {
  char *big = 0;
  size_t n, bigsize = 0;
  static struct stat st;
  static volatile int i, j, k, *A[4096], fds[M], *maps[M], mapsizes[M];
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
  EZBENCH2("free(malloc(16))", donothing, MallocFree());
  EZBENCH2("free() bulk", BulkFreeBenchSetup(), FreeBulk());
  EZBENCH2("bulk_free()", BulkFreeBenchSetup(),
           bulk_free(bulk, ARRAYLEN(bulk)));
}
