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
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/wait0.internal.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define THREADS    8
#define ITERATIONS 512

char *stack[THREADS];
char tls[THREADS][64];

__attribute__((__constructor__)) void init(void) {
  __enable_tls();
  __enable_threads();
}

TEST(pthread_mutex_lock, normal) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutex_init(&lock, 0));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
}

TEST(pthread_mutex_lock, recursive) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

TEST(pthread_mutex_lock, errorcheck) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(EPERM, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(EDEADLK, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(EPERM, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

int count;
_Atomic(int) finished;
_Alignas(64) char slock;
pthread_mutex_t lock;

int MutexWorker(void *p) {
  int i;
  for (i = 0; i < ITERATIONS; ++i) {
    pthread_mutex_lock(&lock);
    ++count;
    pthread_mutex_unlock(&lock);
  }
  return 0;
}

TEST(pthread_mutex_lock, contention) {
  int i;
  count = 0;
  for (i = 0; i < THREADS; ++i) {
    clone(MutexWorker,
          (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                           MAP_STACK | MAP_ANONYMOUS, -1, 0)),
          GetStackSize(),
          CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
              CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS,
          0, 0, __initialize_tls(tls[i]), sizeof(tls[i]),
          (int *)(tls[i] + 0x38));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
  }
  ASSERT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    munmap(stack[i], GetStackSize());
  }
}

int SpinlockWorker(void *p) {
  int i;
  for (i = 0; i < ITERATIONS; ++i) {
    _spinlock(&slock);
    ++count;
    _spunlock(&slock);
  }
  ++finished;
  return 0;
}

TEST(_spinlock, contention) {
  int i;
  count = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    clone(SpinlockWorker,
          (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                           MAP_STACK | MAP_ANONYMOUS, -1, 0)),
          GetStackSize(),
          CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
              CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS,
          0, 0, __initialize_tls(tls[i]), sizeof(tls[i]),
          (int *)(tls[i] + 0x38));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
  }
  ASSERT_EQ(THREADS, finished);
  ASSERT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    munmap(stack[i], GetStackSize());
  }
}

BENCH(pthread_mutex_lock, bench) {
  char schar = 0;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  EZBENCH2("pthread_mutex_lock", donothing,
           (pthread_mutex_lock(&lock), pthread_mutex_unlock(&lock)));
  EZBENCH2("__fds_lock", donothing, (__fds_lock(), __fds_unlock()));
  EZBENCH2("_spinlock", donothing, (_spinlock(&schar), _spunlock(&schar)));
  EZBENCH2("_spinlock_tiny", donothing,
           (_spinlock_tiny(&schar), _spunlock(&schar)));
  EZBENCH2("_spinlock_coop", donothing,
           (_spinlock_cooperative(&schar), _spunlock(&schar)));
  EZBENCH2("content mut", donothing, pthread_mutex_lock_contention());
  EZBENCH2("content spin", donothing, _spinlock_contention());
}
