/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/safemacros.h"
#include "libc/mem/gc.h"
#include "libc/mem/leaks.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define ITERATIONS 1000
#define THREADS    10
#define SIZE       1024

void *Worker(void *arg) {
  for (int i = 0; i < ITERATIONS; ++i) {
    char *p;
    ASSERT_NE(NULL, (p = malloc(rand() % SIZE)));
    ASSERT_NE(NULL, (p = realloc(p, rand() % SIZE)));
    free(p);
  }
  return 0;
}

TEST(malloc, torture) {
  int i, n = THREADS;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  if (!n)
    return;
  printf("\nmalloc torture test w/ %d threads and %d iterations\n", n,
         ITERATIONS);
  SPAWN(fork);
  AssertNoLocksAreHeld();
  struct timespec t1 = timespec_real();
  for (i = 0; i < n; ++i)
    ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  for (i = 0; i < n; ++i)
    ASSERT_EQ(0, pthread_join(t[i], 0));
  struct timespec t2 = timespec_real();
  printf("consumed %g wall and %g cpu seconds\n",
         timespec_tomicros(timespec_sub(t2, t1)) * 1e-6,
         (double)clock() / CLOCKS_PER_SEC);
  EXITS(0);
}
