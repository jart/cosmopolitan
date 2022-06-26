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
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/errno.h"
#include "libc/intrin/futex.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"
#include "libc/intrin/spinlock.h"
#include "libc/intrin/wait0.internal.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

#define THREADS    8
#define ITERATIONS 512
#define TLS_SIZE   PAGESIZE

char *tls[THREADS];
char *stack[THREADS];
_Alignas(PAGESIZE) char tlsdata[THREADS * 3][TLS_SIZE];

int count;
_Atomic(int) started;
_Atomic(int) finished;
_Alignas(64) char slock;
pthread_mutex_t mylock;

__attribute__((__constructor__)) void init(void) {
  int i;
  __enable_tls();
  __enable_threads();
  for (i = 0; i < THREADS; ++i) {
    CHECK_NE(-1, mprotect(tlsdata[i * 3 + 0], TLS_SIZE, PROT_NONE));
    tls[i] = tlsdata[i * 3 + 1];
    CHECK_NE(-1, mprotect(tlsdata[i * 3 + 2], TLS_SIZE, PROT_NONE));
  }
}

TEST(pthread_mutex_lock, normal) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
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
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
}

TEST(pthread_mutex_lock, errorcheck) {
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  __assert_disable = true;
  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&lock, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(EPERM, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_lock(&lock));
  ASSERT_EQ(EDEADLK, pthread_mutex_lock(&lock));
  ASSERT_EQ(0, pthread_mutex_unlock(&lock));
  ASSERT_EQ(EPERM, pthread_mutex_unlock(&lock));
  ASSERT_EQ(0, pthread_mutex_destroy(&lock));
  __assert_disable = false;
}

int MutexWorker(void *p) {
  int i;
  ++started;
  for (i = 0; i < ITERATIONS; ++i) {
    pthread_mutex_lock(&mylock);
    ++count;
    pthread_mutex_unlock(&mylock);
  }
  ASSERT_NE(0, (int *)(tls[(intptr_t)p] + 0x38));
  ++finished;
  return 0;
}

TEST(pthread_mutex_lock, contention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_NE(MAP_FAILED,
              (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                               MAP_STACK | MAP_ANONYMOUS, -1, 0)));
    ASSERT_NE(-1, clone(MutexWorker, stack[i], GetStackSize(),
                        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                            CLONE_SIGHAND | CLONE_CHILD_SETTID |
                            CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                        (void *)(intptr_t)i, 0, __initialize_tls(tls[i]),
                        TLS_SIZE, (int *)(tls[i] + 0x38)));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0, munmap(stack[i], GetStackSize()));
  }
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

TEST(pthread_mutex_lock, rcontention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_NE(MAP_FAILED,
              (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                               MAP_STACK | MAP_ANONYMOUS, -1, 0)));
    ASSERT_NE(-1, clone(MutexWorker, stack[i], GetStackSize(),
                        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                            CLONE_SIGHAND | CLONE_CHILD_SETTID |
                            CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                        (void *)(intptr_t)i, 0, __initialize_tls(tls[i]),
                        TLS_SIZE, (int *)(tls[i] + 0x38)));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0, munmap(stack[i], GetStackSize()));
  }
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

TEST(pthread_mutex_lock, econtention) {
  int i;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
  pthread_mutex_init(&mylock, &attr);
  pthread_mutexattr_destroy(&attr);
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_NE(MAP_FAILED,
              (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                               MAP_STACK | MAP_ANONYMOUS, -1, 0)));
    ASSERT_NE(-1, clone(MutexWorker, stack[i], GetStackSize(),
                        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                            CLONE_SIGHAND | CLONE_CHILD_SETTID |
                            CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                        (void *)(intptr_t)i, 0, __initialize_tls(tls[i]),
                        TLS_SIZE, (int *)(tls[i] + 0x38)));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  for (i = 0; i < THREADS; ++i) {
    ASSERT_EQ(0, *(int *)(tls[i] + 0x38));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0, munmap(stack[i], GetStackSize()));
  }
  EXPECT_EQ(0, pthread_mutex_destroy(&mylock));
}

int SpinlockWorker(void *p) {
  int i;
  ++started;
  for (i = 0; i < ITERATIONS; ++i) {
    _spinlock(&slock);
    ++count;
    _spunlock(&slock);
  }
  ASSERT_NE(0, (int *)(tls[(intptr_t)p] + 0x38));
  ++finished;
  return 0;
}

TEST(_spinlock, contention) {
  int i;
  count = 0;
  started = 0;
  finished = 0;
  for (i = 0; i < THREADS; ++i) {
    ASSERT_NE(MAP_FAILED,
              (stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                               MAP_STACK | MAP_ANONYMOUS, -1, 0)));
    ASSERT_NE(-1, clone(SpinlockWorker, stack[i], GetStackSize(),
                        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                            CLONE_SIGHAND | CLONE_CHILD_SETTID |
                            CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                        (void *)(intptr_t)i, 0, __initialize_tls(tls[i]),
                        TLS_SIZE, (int *)(tls[i] + 0x38)));
  }
  for (i = 0; i < THREADS; ++i) {
    _wait0((int *)(tls[i] + 0x38));
  }
  EXPECT_EQ(THREADS, started);
  EXPECT_EQ(THREADS, finished);
  EXPECT_EQ(THREADS * ITERATIONS, count);
  for (i = 0; i < THREADS; ++i) {
    ASSERT_SYS(0, 0, munmap(stack[i], GetStackSize()));
  }
}

BENCH(pthread_mutex_lock, bench) {
  char schar = 0;
  pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  EZBENCH2("_spinlock", donothing, _spinlock_contention());
  EZBENCH2("normal", donothing, pthread_mutex_lock_contention());
  EZBENCH2("recursive", donothing, pthread_mutex_lock_rcontention());
  EZBENCH2("errorcheck", donothing, pthread_mutex_lock_econtention());
}
