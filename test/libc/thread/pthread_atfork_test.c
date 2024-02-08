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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "third_party/nsync/mu.h"

int Ai;
const char *A[16];

// clang-format off
void prepare1(void) { A[Ai++] = "prepare1"; }
void prepare2(void) { A[Ai++] = "prepare2"; }
void parent1(void) { A[Ai++] = "parent1"; }
void parent2(void) { A[Ai++] = "parent2"; }
void child1(void) { A[Ai++] = "child1"; }
void child2(void) { A[Ai++] = "child2"; }
// clang-format on

void *ForceThreadingMode(void *arg) {
  return 0;
}

TEST(pthread_atfork, test) {
  __enable_threads();
  SPAWN(fork);
  ASSERT_EQ(0, pthread_atfork(prepare1, parent1, child1));
  ASSERT_EQ(0, pthread_atfork(prepare2, parent2, child2));
  flockfile(stdout);
  SPAWN(fork);
  flockfile(stdout);
  ASSERT_STREQ("prepare2", A[0]);
  ASSERT_STREQ("prepare1", A[1]);
  ASSERT_STREQ("child1", A[2]);
  ASSERT_STREQ("child2", A[3]);
  funlockfile(stdout);
  EXITS(0);
  funlockfile(stdout);
  ASSERT_STREQ("prepare2", A[0]);
  ASSERT_STREQ("prepare1", A[1]);
  ASSERT_STREQ("parent1", A[2]);
  ASSERT_STREQ("parent2", A[3]);
  EXITS(0);
}

pthread_mutex_t mu;

void mu_lock(void) {
  pthread_mutex_lock(&mu);
}

void mu_unlock(void) {
  pthread_mutex_unlock(&mu);
}

void mu_wipe(void) {
  pthread_mutex_init(&mu, 0);
}

void *Worker(void *arg) {
  for (int i = 0; i < 20; ++i) {
    mu_lock();
    usleep(20);
    mu_unlock();
    SPAWN(fork);
    mu_lock();
    usleep(1);
    mu_unlock();
    mu_lock();
    mu_unlock();
    EXITS(0);
    mu_lock();
    mu_unlock();
  }
  return 0;
}

TEST(pthread_atfork, fork_exit_torture) {
  mu_wipe();
  pthread_atfork(mu_lock, mu_unlock, mu_wipe);
  int i, n = 4;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  }
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_join(t[i], 0));
  }
}
