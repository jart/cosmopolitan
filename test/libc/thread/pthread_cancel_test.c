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
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

int pfds[2];
atomic_bool ready;
pthread_cond_t cv;
pthread_mutex_t mu;
atomic_int gotcleanup;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  gotcleanup = false;
}

void OnCleanup(void *arg) {
  gotcleanup = true;
}

void *CancelSelfWorkerDeferred(void *arg) {
  char buf[8];
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
  pthread_cleanup_push(OnCleanup, 0);
  pthread_cancel(pthread_self());
  read(pfds[0], buf, sizeof(buf));
  pthread_cleanup_pop(0);
  return 0;
}

TEST(pthread_cancel, self_deferred_waitsForCancelationPoint) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, CancelSelfWorkerDeferred, 0));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

void *Worker(void *arg) {
  char buf[8];
  ready = true;
  pthread_cleanup_push(OnCleanup, 0);
  read(pfds[0], buf, sizeof(buf));
  pthread_cleanup_pop(0);
  return 0;
}

TEST(pthread_cancel, synchronous) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, Worker, 0));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

TEST(pthread_cancel, synchronous_deferred) {
  void *rc;
  pthread_t th;
  if (!IsWindows()) return;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, Worker, 0));
  while (!ready) pthread_yield();
  ASSERT_SYS(0, 0, usleep(10));
  EXPECT_EQ(0, pthread_cancel(th));
  EXPECT_EQ(0, pthread_join(th, &rc));
  EXPECT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

void *DisabledWorker(void *arg) {
  char buf[8];
  pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
  pthread_cleanup_push(OnCleanup, 0);
  ASSERT_SYS(ECANCELED, -1, read(pfds[0], buf, sizeof(buf)));
  pthread_cleanup_pop(0);
  return 0;
}

TEST(pthread_cancel, masked) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, DisabledWorker, 0));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(0, rc);
  ASSERT_FALSE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

TEST(pthread_cancel, masked_delayed) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, DisabledWorker, 0));
  ASSERT_SYS(0, 0, usleep(10));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(0, rc);
  ASSERT_FALSE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

void *CondWaitMaskedWorker(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(ECANCELED, pthread_cond_timedwait(&cv, &mu, 0));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
  return 0;
}

TEST(pthread_cancel, condMaskedWait) {
  void *rc;
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, CondWaitMaskedWorker, 0));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(0, rc);
}

TEST(pthread_cancel, condWaitMaskedDelayed) {
  void *rc;
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, CondWaitMaskedWorker, 0));
  ASSERT_SYS(0, 0, usleep(10));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(0, rc);
}

void *CondWaitDeferredWorker(void *arg) {
  ASSERT_EQ(0, pthread_setcancelstate(PTHREAD_CANCEL_DEFERRED, 0));
  ASSERT_EQ(0, pthread_mutex_lock(&mu));
  ASSERT_EQ(ECANCELED, pthread_cond_timedwait(&cv, &mu, 0));
  abort();
}

TEST(pthread_cancel, condDeferredWait_reacquiresMutex) {
  void *rc;
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, CondWaitDeferredWorker, 0));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_EQ(EBUSY, pthread_mutex_trylock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
}

TEST(pthread_cancel, condDeferredWaitDelayed) {
  void *rc;
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, CondWaitDeferredWorker, 0));
  ASSERT_SYS(0, 0, usleep(10));
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_EQ(EBUSY, pthread_mutex_trylock(&mu));
  ASSERT_EQ(0, pthread_mutex_unlock(&mu));
}

char *wouldleak;
pthread_key_t key;
bool key_destructor_was_run;
atomic_int is_in_infinite_loop;

void KeyDestructor(void *arg) {
  CheckStackIsAligned();
  ASSERT_EQ(31337, (intptr_t)arg);
  key_destructor_was_run = true;
}

#ifdef __x86_64__
void TortureStack(void) {
  asm("sub\t$4,%rsp\n\t"
      "pause\n\t"
      "sub\t$2,%rsp\n\t"
      "pause\n\t"
      "add\t$6,%rsp");
}
#endif

void *CpuBoundWorker(void *arg) {
  char *volatile wontleak1;
  char *volatile wontleak2;
  CheckStackIsAligned();
  wouldleak = malloc(123);
  wontleak1 = malloc(123);
  (void)wontleak1;
  pthread_cleanup_push(free, wontleak1);
  wontleak2 = gc(malloc(123));
  (void)wontleak2;
  ASSERT_EQ(0, pthread_setspecific(key, (void *)31337));
  ASSERT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0));
  for (;;) {
#ifdef __x86_64__
    TortureStack();
#else
    CheckStackIsAligned();
#endif
    is_in_infinite_loop = true;
  }
  pthread_cleanup_pop(1);
  free(wouldleak);
  return 0;
}

TEST(pthread_cancel, async) {
  void *rc;
  pthread_t th;
  ASSERT_EQ(0, pthread_key_create(&key, KeyDestructor));
  wouldleak = NULL;
  is_in_infinite_loop = false;
  key_destructor_was_run = false;
  ASSERT_EQ(0, pthread_create(&th, 0, CpuBoundWorker, 0));
  while (!is_in_infinite_loop) pthread_yield();
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(key_destructor_was_run);
  ASSERT_EQ(0, pthread_key_delete(key));
  free(wouldleak);
}

void *CancelSelfWorkerAsync(void *arg) {
  ASSERT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0));
  pthread_cleanup_push(OnCleanup, 0);
  ASSERT_EQ(0, pthread_cancel(pthread_self()));
  pthread_cleanup_pop(0);
  return 0;
}

TEST(pthread_cancel, self_asynchronous_takesImmediateEffect) {
  void *rc;
  pthread_t th;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, CancelSelfWorkerAsync, 0));
  ASSERT_EQ(0, pthread_join(th, &rc));
  ASSERT_EQ(PTHREAD_CANCELED, rc);
  ASSERT_TRUE(gotcleanup);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

atomic_bool was_completed;

void WaitUntilReady(void) {
  while (!ready) pthread_yield();
  ASSERT_EQ(0, errno);
  ASSERT_SYS(0, 0, usleep(1000));
}

void *SleepWorker(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_MASKED, 0);
  ready = true;
  ASSERT_SYS(ECANCELED, -1, usleep(30 * 1e6));
  was_completed = true;
  return 0;
}

TEST(pthread_cancel, canInterruptSleepOperation) {
  pthread_t th;
  ASSERT_EQ(0, pthread_create(&th, 0, SleepWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_cancel(th));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_TRUE(was_completed);
}
