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
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clone.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#include "third_party/nsync/mu.h"

/**
 * @fileoverview Austere Mutex Test
 *
 * 1. Tests what happens when *NSYNC isn't linked.
 * 2. Tests what happens when TLS isn't enabled.
 *
 * If either:
 *
 * 1. malloc() isn't linked, or
 * 2. we're in MODE=tiny
 *
 * Then we use Cosmopolitan's hand-rolled tiny locks. They're not
 * scalable. They use a lot of CPU if you have lots of threads. But
 * they're wicked fast and free of bloat if your app is tiny.
 */

atomic_int ready;
atomic_int counter;
atomic_int success;
pthread_mutex_t mu;

#define ASSERT_EQ(WANT, GOT)                                          \
  do {                                                                \
    long _want = WANT, _got = GOT;                                    \
    if (_want != _got)                                                \
      __assert_eq_fail(__FILE__, __LINE__, #WANT, #GOT, _want, _got); \
  } while (0)

void __assert_eq_fail(const char *file, int line, const char *wantstr,
                      const char *gotstr, long want, long got) {
  kprintf("%s:%d: %s vs. %s was %ld vs. %ld (%s)\n", file, line, wantstr,
          gotstr, want, got, !(got & ~255) ? _strerrno(got) : "n/a");
  _Exit(1);
}

double time2dbl(struct timespec t) {
  return (((double)t.tv_sec) + ((double)t.tv_nsec * 1e-9));
}

char *time2str(double s) {
  static char buf[32];
  static const struct {
    const char *suffix;
    double multiplier;
  } scale[] = {
      {"ns", 1.0e-9},  //
      {"us", 1e-6},    //
      {"ms", 1e-3},    //
      {"s", 1.0},      //
      {"hr", 3600.0},  //
  };
  int i = 0;
  while (i + 1 != sizeof(scale) / sizeof(scale[0]) &&
         scale[i + 1].multiplier <= s) {
    i++;
  }
  stpcpy(FormatInt32(buf, s / scale[i].multiplier), scale[i].suffix);
  return buf;
}

int Worker(void *arg) {
  int i, x;
  atomic_store(&ready, 1);
  for (i = 0; i < 10000; ++i) {
    ASSERT_EQ(0, pthread_mutex_lock(&mu));
    x = atomic_load_explicit(&counter, memory_order_relaxed);
    atomic_store_explicit(&counter, x + 1, memory_order_relaxed);
    ASSERT_EQ(x + 1, atomic_load_explicit(&counter, memory_order_relaxed));
    ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  }
  atomic_store(&success, 1);
  return 0;
}

void TestContendedLock(const char *name, int kind) {
  char *stk;
  double ns;
  errno_t rc;
  struct timespec t1, t2;
  pthread_mutexattr_t attr;
  int tid, x, i, n = 10000;
  struct CosmoTib tib = {.tib_self = &tib, .tib_self2 = &tib, .tib_tid = -1};
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, kind);
  pthread_mutex_init(&mu, &attr);
  pthread_mutexattr_destroy(&attr);
  atomic_store(&ready, 0);
  atomic_store(&success, 0);
  stk = NewCosmoStack();
  rc = clone(Worker, stk, GetStackSize() - 16 /* openbsd:stackbound */,
             CLONE_VM | CLONE_THREAD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                 CLONE_SYSVSEM | CLONE_PARENT_SETTID | CLONE_CHILD_SETTID |
                 CLONE_CHILD_CLEARTID | CLONE_SETTLS,
             0, &tid, &tib, &tib.tib_tid);
  if (rc) {
    kprintf("clone failed: %s\n", strerror(rc));
    _Exit(1);
  }
  while (!atomic_load(&ready)) donothing;
  t1 = timespec_real();
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(0, pthread_mutex_lock(&mu));
    x = atomic_load_explicit(&counter, memory_order_relaxed);
    atomic_store_explicit(&counter, x - 1, memory_order_relaxed);
    ASSERT_EQ(x - 1, atomic_load_explicit(&counter, memory_order_relaxed));
    ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  }
  t2 = timespec_real();
  while (tib.tib_tid) donothing;
  ASSERT_EQ(1, atomic_load(&success));
  ASSERT_EQ(0, atomic_load(&counter));
  FreeCosmoStack(stk);
  ASSERT_EQ(0, pthread_mutex_destroy(&mu));
  ns = time2dbl(timespec_sub(t2, t1)) / n;
  kprintf("%s contended took %s\n", name, time2str(ns));
}

void TestUncontendedLock(const char *name, int kind) {
  double ns;
  long i, n = 10000;
  struct timespec t1, t2;
  pthread_mutex_t lock;
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, kind);
  pthread_mutex_init(&lock, &attr);
  pthread_mutexattr_destroy(&attr);
  t1 = timespec_real();
  for (i = 0; i < n; ++i) {
    pthread_mutex_lock(&lock);
    pthread_mutex_unlock(&lock);
  }
  t2 = timespec_real();
  pthread_mutex_destroy(&lock);
  ns = time2dbl(timespec_sub(t2, t1)) / n;
  kprintf("%s took %s\n", name, time2str(ns));
}

int main(int argc, char *argv[]) {
  pthread_mutexattr_t attr;

#ifdef __aarch64__
  // our usage of raw clone() is probably broken in aarch64
  // we should just get rid of clone()
  if (1) return 0;
#endif

  if (_weaken(nsync_mu_lock)) {
    kprintf("*NSYNC should not be linked\n");
    _Exit(1);
  }

  __threaded = 1;

  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
  ASSERT_EQ(0, pthread_mutex_init(&mu, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  ASSERT_EQ(0, pthread_mutex_destroy(&mu));

  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
  ASSERT_EQ(0, pthread_mutex_init(&mu, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  ASSERT_EQ(0, pthread_mutex_destroy(&mu));

  ASSERT_EQ(0, pthread_mutexattr_init(&attr));
  ASSERT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
  ASSERT_EQ(0, pthread_mutex_init(&mu, &attr));
  ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(EDEADLK, pthread_mutex_lock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  ASSERT_EQ(0, pthread_mutex_destroy(&mu));

  ASSERT_EQ(1, __tls_enabled);

  TestUncontendedLock("PTHREAD_MUTEX_NORMAL RAW TLS", PTHREAD_MUTEX_NORMAL);
  TestUncontendedLock("PTHREAD_MUTEX_RECURSIVE RAW TLS",
                      PTHREAD_MUTEX_RECURSIVE);
  TestUncontendedLock("PTHREAD_MUTEX_ERRORCHECK RAW TLS",
                      PTHREAD_MUTEX_ERRORCHECK);

  TestContendedLock("PTHREAD_MUTEX_NORMAL RAW TLS", PTHREAD_MUTEX_NORMAL);
  TestContendedLock("PTHREAD_MUTEX_RECURSIVE RAW TLS", PTHREAD_MUTEX_RECURSIVE);
  TestContendedLock("PTHREAD_MUTEX_ERRORCHECK RAW TLS",
                    PTHREAD_MUTEX_ERRORCHECK);

  __tls_enabled_set(false);

  TestUncontendedLock("PTHREAD_MUTEX_NORMAL RAW", PTHREAD_MUTEX_NORMAL);
  TestUncontendedLock("PTHREAD_MUTEX_RECURSIVE RAW", PTHREAD_MUTEX_RECURSIVE);
  TestUncontendedLock("PTHREAD_MUTEX_ERRORCHECK RAW", PTHREAD_MUTEX_ERRORCHECK);

  TestContendedLock("PTHREAD_MUTEX_NORMAL RAW", PTHREAD_MUTEX_NORMAL);
  TestContendedLock("PTHREAD_MUTEX_RECURSIVE RAW", PTHREAD_MUTEX_RECURSIVE);
  TestContendedLock("PTHREAD_MUTEX_ERRORCHECK RAW", PTHREAD_MUTEX_ERRORCHECK);

  //
}
