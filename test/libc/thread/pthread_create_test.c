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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sched.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

void OnUsr1(int sig, struct siginfo *si, void *vctx) {
}

void SetUp(void) {
  struct sigaction sig = {.sa_sigaction = OnUsr1, .sa_flags = SA_SIGINFO};
  sigaction(SIGUSR1, &sig, 0);
}

void TriggerSignal(void) {
  sched_yield();
  raise(SIGUSR1);
  sched_yield();
}

static void *Increment(void *arg) {
  ASSERT_EQ(gettid(), pthread_getthreadid_np());
  TriggerSignal();
  return (void *)((uintptr_t)arg + 1);
}

TEST(pthread_create, testCreateReturnJoin) {
  void *rc;
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, Increment, (void *)1));
  ASSERT_EQ(0, pthread_join(id, &rc));
  ASSERT_EQ((void *)2, rc);
}

static void *IncExit(void *arg) {
  CheckStackIsAligned();
  TriggerSignal();
  pthread_exit((void *)((uintptr_t)arg + 1));
}

TEST(pthread_create, testCreateExitJoin) {
  void *rc;
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, IncExit, (void *)2));
  ASSERT_EQ(0, pthread_join(id, &rc));
  ASSERT_EQ((void *)3, rc);
}

static void *CheckSchedule(void *arg) {
  int policy;
  struct sched_param prio;
  ASSERT_EQ(0, pthread_getschedparam(pthread_self(), &policy, &prio));
  ASSERT_EQ(SCHED_OTHER, policy);
  ASSERT_EQ(sched_get_priority_min(SCHED_OTHER), prio.sched_priority);
  if (IsWindows() || IsXnu() || IsOpenbsd()) {
    ASSERT_EQ(ENOSYS, pthread_setschedparam(pthread_self(), policy, &prio));
  } else {
    ASSERT_EQ(0, pthread_setschedparam(pthread_self(), policy, &prio));
  }
  return 0;
}

TEST(pthread_create, scheduling) {
  pthread_t id;
  pthread_attr_t attr;
  struct sched_param pri = {sched_get_priority_min(SCHED_OTHER)};
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED));
  ASSERT_EQ(0, pthread_attr_setschedpolicy(&attr, SCHED_OTHER));
  ASSERT_EQ(0, pthread_attr_setschedparam(&attr, &pri));
  ASSERT_EQ(0, pthread_create(&id, &attr, CheckSchedule, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
}

static void *CheckStack(void *arg) {
  char buf[1024 * 1024];
  TriggerSignal();
  CheckLargeStackAllocation(buf, 1024 * 1024);
  return 0;
}

TEST(pthread_create, testBigStack) {
  pthread_t id;
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstacksize(&attr, 2 * 1000 * 1000));
  ASSERT_EQ(0, pthread_create(&id, &attr, CheckStack, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
}

static void *CheckStack2(void *arg) {
  char buf[262144 - 32768 * 2];
  TriggerSignal();
  CheckLargeStackAllocation(buf, sizeof(buf));
  return 0;
}

TEST(pthread_create, testBiggerGuardSize) {
  pthread_t id;
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstacksize(&attr, 262144));
  ASSERT_EQ(0, pthread_attr_setguardsize(&attr, 32768));
  ASSERT_EQ(0, pthread_create(&id, &attr, CheckStack2, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
}

TEST(pthread_create, testCustomStack_withReallySmallSize) {
  char *stk;
  size_t siz;
  pthread_t id;
  pthread_attr_t attr;
  siz = PTHREAD_STACK_MIN;
  stk = malloc(siz);
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstack(&attr, stk, siz));
  ASSERT_EQ(0, pthread_create(&id, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
  free(stk);
  stk = malloc(siz);
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstack(&attr, stk, siz));
  ASSERT_EQ(0, pthread_create(&id, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
  free(stk);
}

void *JoinMainWorker(void *arg) {
  void *rc;
  pthread_t main_thread = (pthread_t)arg;
  gc(malloc(32));
  gc(malloc(32));
  ASSERT_EQ(0, pthread_join(main_thread, &rc));
  ASSERT_EQ(123, (intptr_t)rc);
  return 0;
}

TEST(pthread_join, mainThread) {
  pthread_t id;
  gc(malloc(32));
  gc(malloc(32));
  SPAWN(fork);
  ASSERT_EQ(0, pthread_create(&id, 0, JoinMainWorker, (void *)pthread_self()));
  pthread_exit((void *)123);
  EXITS(0);
}

TEST(pthread_join, mainThreadDelayed) {
  pthread_t id;
  gc(malloc(32));
  gc(malloc(32));
  SPAWN(fork);
  ASSERT_EQ(0, pthread_create(&id, 0, JoinMainWorker, (void *)pthread_self()));
  usleep(10000);
  pthread_exit((void *)123);
  EXITS(0);
}

TEST(pthread_exit, fromMainThread_whenNoThreadsWereCreated) {
  SPAWN(fork);
  pthread_exit((void *)123);
  EXITS(0);
}

atomic_int g_cleanup1;
atomic_int g_cleanup2;

void OnCleanup(void *arg) {
  *(atomic_int *)arg = true;
}

void *CleanupExit(void *arg) {
  pthread_cleanup_push(OnCleanup, &g_cleanup1);
  pthread_cleanup_push(OnCleanup, &g_cleanup2);
  pthread_cleanup_pop(false);
  pthread_exit(0);
  pthread_cleanup_pop(false);
  return 0;
}

TEST(pthread_cleanup, pthread_exit_alwaysCallsCallback) {
  pthread_t id;
  g_cleanup1 = false;
  g_cleanup2 = false;
  ASSERT_EQ(0, pthread_create(&id, 0, CleanupExit, 0));
  ASSERT_EQ(0, pthread_join(id, 0));
  ASSERT_TRUE(g_cleanup1);
  ASSERT_FALSE(g_cleanup2);
}

void *CleanupNormal(void *arg) {
  pthread_cleanup_push(OnCleanup, &g_cleanup1);
  pthread_cleanup_push(OnCleanup, &g_cleanup2);
  pthread_cleanup_pop(true);
  pthread_cleanup_pop(true);
  return 0;
}

TEST(pthread_cleanup, pthread_normal) {
  pthread_t id;
  g_cleanup1 = false;
  g_cleanup2 = false;
  ASSERT_EQ(0, pthread_create(&id, 0, CleanupNormal, 0));
  ASSERT_EQ(0, pthread_join(id, 0));
  ASSERT_TRUE(g_cleanup1);
  ASSERT_TRUE(g_cleanup2);
}

////////////////////////////////////////////////////////////////////////////////
// BENCHMARKS

static void CreateJoin(void) {
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, Increment, 0));
  ASSERT_EQ(0, pthread_join(id, 0));
}

// this is de facto the same as create+join
static void CreateDetach(void) {
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, Increment, 0));
  ASSERT_EQ(0, pthread_detach(id));
}

// this is really fast
static void CreateDetached(void) {
  pthread_t th;
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  ASSERT_EQ(0, pthread_create(&th, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
}

BENCH(pthread_create, bench) {
  EZBENCH2("CreateJoin", donothing, CreateJoin());
  EZBENCH2("CreateDetach", donothing, CreateDetach());
  EZBENCH2("CreateDetached", donothing, CreateDetached());
  while (!pthread_orphan_np()) {
    _pthread_decimate();
  }
}
