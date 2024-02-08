/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define N 32

int i, n;
struct spawn *t;
atomic_int x, y;
pthread_barrier_t b;
static _Atomic(uint32_t) once;

void InitFactory(void) {
  ASSERT_EQ(0, atomic_load(&x));
  atomic_fetch_add(&y, 1);
}

void *Worker(void *arg) {
  pthread_barrier_wait(&b);
  ASSERT_EQ(0, cosmo_once(&once, InitFactory));
  ASSERT_EQ(1, atomic_load(&y));
  atomic_fetch_add(&x, 1);
  return 0;
}

TEST(cosmo_once, test) {
  pthread_t th[N];
  x = y = 0;
  ASSERT_EQ(0, pthread_barrier_init(&b, 0, N));
  for (i = 0; i < N; ++i) {
    ASSERT_EQ(0, pthread_create(th + i, 0, Worker, 0));
  }
  for (i = 0; i < N; ++i) {
    ASSERT_EQ(0, pthread_join(th[i], 0));
  }
  ASSERT_EQ(N, atomic_load(&x));
  ASSERT_EQ(1, atomic_load(&y));
  ASSERT_EQ(0, pthread_barrier_destroy(&b));
}
