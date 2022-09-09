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
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/pthread.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

static void *Increment(void *arg) {
  ASSERT_EQ(gettid(), pthread_getthreadid_np());
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
  pthread_exit((void *)((uintptr_t)arg + 1));
}

TEST(pthread_create, testCreateExitJoin) {
  void *rc;
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, IncExit, (void *)2));
  ASSERT_EQ(0, pthread_join(id, &rc));
  ASSERT_EQ((void *)3, rc);
}

TEST(pthread_detach, testCreateReturn) {
  pthread_t id;
  ASSERT_EQ(0, pthread_create(&id, 0, Increment, 0));
  ASSERT_EQ(0, pthread_detach(id));
}

TEST(pthread_detach, testDetachUponCreation) {
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  ASSERT_EQ(0, pthread_create(0, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
}

static void *CheckStack(void *arg) {
  char buf[1024 * 1024];
  CheckLargeStackAllocation(buf, 1024 * 1024);
  return 0;
}

TEST(pthread_detach, testBigStack) {
  pthread_t id;
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstacksize(&attr, 2 * 1000 * 1000));
  ASSERT_EQ(0, pthread_create(&id, &attr, CheckStack, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
}

TEST(pthread_detach, testCustomStack_withReallySmallSize) {
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
  // we still own the stack memory
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setstack(&attr, stk, siz));
  ASSERT_EQ(0, pthread_create(&id, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
  ASSERT_EQ(0, pthread_join(id, 0));
  free(stk);
}

TEST(pthread_exit, mainThreadWorks) {
  // _Exit1() can't set process exit code on XNU/NetBSD/OpenBSD.
  if (IsLinux() || IsFreebsd() || IsWindows()) {
    SPAWN(fork);
    pthread_exit((void *)2);
    EXITS(2);
  } else {
    SPAWN(fork);
    pthread_exit((void *)0);
    EXITS(0);
  }
}

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
  pthread_attr_t attr;
  ASSERT_EQ(0, pthread_attr_init(&attr));
  ASSERT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  ASSERT_EQ(0, pthread_create(0, &attr, Increment, 0));
  ASSERT_EQ(0, pthread_attr_destroy(&attr));
}

BENCH(pthread_create, bench) {
  EZBENCH2("CreateJoin", donothing, CreateJoin());
  EZBENCH2("CreateDetach", donothing, CreateDetach());
  EZBENCH2("CreateDetached", donothing, CreateDetached());
}
