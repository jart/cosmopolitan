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
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

static char tls[64];

int PutProcessInThreadingMode(void *p) {
  return 0;
}

void SetUp(void) {
  clone(PutProcessInThreadingMode,
        mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
             MAP_STACK | MAP_ANONYMOUS, -1, 0),
        GetStackSize(),
        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
            CLONE_SETTLS,
        0, 0, __initialize_tls(tls), sizeof(tls), 0);
}

TEST(pthread_mutex_lock, normal) {
  pthread_mutex_t lock;
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
}
