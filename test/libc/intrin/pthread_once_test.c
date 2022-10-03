/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"

int i, n;
struct spawn *t;
atomic_int x, y;
pthread_barrier_t b;
static pthread_once_t once = PTHREAD_ONCE_INIT;

void InitFactory(void) {
  ASSERT_EQ(0, atomic_load(&x));
  atomic_fetch_add(&y, 1);
}

int Worker(void *arg, int tid) {
  pthread_barrier_wait(&b);
  ASSERT_EQ(0, pthread_once(&once, InitFactory));
  ASSERT_EQ(1, atomic_load(&y));
  atomic_fetch_add(&x, 1);
  return 0;
}

TEST(pthread_once, test) {
  n = 32;
  x = y = 0;
  ASSERT_EQ(0, pthread_barrier_init(&b, 0, n));
  t = gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, _spawn(Worker, 0, t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
  ASSERT_EQ(n, atomic_load(&x));
  ASSERT_EQ(1, atomic_load(&y));
  ASSERT_EQ(0, pthread_barrier_destroy(&b));
}

__attribute__((__constructor__)) static void init(void) {
  // try to test both the nsync and non-nsync versions with regular builds
  if (!IsTiny()) {
    pthread_cond_t c = {0};
    pthread_cond_broadcast(&c);
  }
}
