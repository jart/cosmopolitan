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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/errno.h"
#include "libc/intrin/atomic.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"
#include "libc/thread/thread.h"

int i, n;
atomic_int p, w;
pthread_barrier_t barrier;

int Worker(void *arg, int tid) {
  int rc;
  rc = pthread_barrier_wait(&barrier);
  atomic_fetch_add(&w, 1);
  ASSERT_GE(rc, 0);
  if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
    atomic_fetch_add(&p, 1);
  }
  return 0;
}

TEST(pthread_barrier_init, test0_isInvalid) {
  __assert_disable = true;
  ASSERT_EQ(EINVAL, pthread_barrier_init(&barrier, 0, 0));
  __assert_disable = false;
}

TEST(pthread_barrier_wait, test1) {
  struct spawn t;
  p = 0;
  w = 0;
  n = 1;
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, n));
  ASSERT_SYS(0, 0, _spawn(Worker, 0, &t));
  EXPECT_SYS(0, 0, _join(&t));
  ASSERT_EQ(1, p);
  ASSERT_EQ(n, w);
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}

TEST(pthread_barrier_wait, test32) {
  struct spawn *t;
  p = 0;
  w = 0;
  n = 32;
  t = gc(malloc(sizeof(struct spawn) * n));
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, n));
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, w);
    ASSERT_SYS(0, 0, _spawn(Worker, 0, t + i));
  }
  for (i = 0; i < n; ++i) {
    EXPECT_SYS(0, 0, _join(t + i));
  }
  ASSERT_EQ(1, p);
  ASSERT_EQ(n, w);
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}
