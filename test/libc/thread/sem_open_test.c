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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/temp.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/semaphore.h"
#include "libc/thread/thread.h"

pthread_barrier_t barrier;
char testlib_enable_tmp_setup_teardown;

void SetUp(void) {
  // TODO(jart): Fix shocking GitHub Actions error.
  if (getenv("CI")) exit(0);
  sem_unlink("/fooz");
  sem_unlink("/barz");
}

void IgnoreStderr(void) {
  close(2);
  open("/dev/null", O_WRONLY);
}

void *Worker(void *arg) {
  sem_t *a, *b;
  struct timespec ts;
  ASSERT_NE(SEM_FAILED, (a = sem_open("/fooz", 0)));
  ASSERT_EQ((sem_t *)arg, a);
  ASSERT_NE(SEM_FAILED, (b = sem_open("/barz", 0)));
  if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
    ASSERT_SYS(0, 0, sem_unlink("/fooz"));
  }
  ASSERT_SYS(0, 0, clock_gettime(CLOCK_REALTIME, &ts));
  ts.tv_sec += 1;
  ASSERT_SYS(0, 0, sem_post(a));
  ASSERT_SYS(0, 0, sem_timedwait(b, &ts));
  ASSERT_SYS(0, 0, sem_close(b));
  ASSERT_TRUE(testlib_memoryexists(b));
  ASSERT_SYS(0, 0, sem_close(a));
  ASSERT_TRUE(testlib_memoryexists(a));
  return 0;
}

// 1. multiple threads opening same name must yield same address
// 2. semaphore memory is freed, when all threads have closed it
// 3. semaphore may be unlinked before it's closed, from creator
// 4. semaphore may be unlinked before it's closed, from threads
TEST(sem_open, test) {
  sem_t *a, *b;
  int i, r, n = 4;
  pthread_t *t = _gc(malloc(sizeof(pthread_t) * n));
  sem_unlink("/fooz");
  sem_unlink("/barz");
  errno = 0;
  ASSERT_EQ(0, pthread_barrier_init(&barrier, 0, n));
  ASSERT_NE(SEM_FAILED, (a = sem_open("/fooz", O_CREAT, 0644, 0)));
  ASSERT_NE(SEM_FAILED, (b = sem_open("/barz", O_CREAT, 0644, 0)));
  ASSERT_SYS(0, 0, sem_getvalue(a, &r));
  ASSERT_EQ(0, r);
  ASSERT_SYS(0, 0, sem_getvalue(b, &r));
  ASSERT_EQ(0, r);
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_create(t + i, 0, Worker, a));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_wait(a));
  ASSERT_SYS(0, 0, sem_getvalue(a, &r));
  ASSERT_EQ(0, r);
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_post(b));
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_join(t[i], 0));
  ASSERT_SYS(0, 0, sem_unlink("/barz"));
  ASSERT_SYS(0, 0, sem_getvalue(b, &r));
  ASSERT_EQ(0, r);
  ASSERT_SYS(0, 0, sem_close(b));
  ASSERT_FALSE(testlib_memoryexists(b));
  ASSERT_SYS(0, 0, sem_close(a));
  ASSERT_FALSE(testlib_memoryexists(a));
  ASSERT_EQ(0, pthread_barrier_destroy(&barrier));
}

TEST(sem_close, withUnnamedSemaphore_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t sem;
  ASSERT_SYS(0, 0, sem_init(&sem, 0, 0));
  SPAWN(fork);
  IgnoreStderr();
  sem_close(&sem);
  EXITS(77);
  ASSERT_SYS(0, 0, sem_destroy(&sem));
}

TEST(sem_destroy, withNamedSemaphore_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t *sem;
  ASSERT_NE(SEM_FAILED, (sem = sem_open("/boop", O_CREAT, 0644, 0)));
  SPAWN(fork);
  IgnoreStderr();
  sem_destroy(sem);
  EXITS(77);
  ASSERT_SYS(0, 0, sem_unlink("/boop"));
  ASSERT_SYS(0, 0, sem_close(sem));
}

TEST(sem_open, inheritAcrossFork) {
  sem_t *a, *b;
  struct timespec ts;
  ASSERT_SYS(0, 0, clock_gettime(CLOCK_REALTIME, &ts));
  ts.tv_sec += 1;
  errno = 0;
  ASSERT_NE(SEM_FAILED, (a = sem_open("/fooz", O_CREAT, 0644, 0)));
  ASSERT_SYS(0, 0, sem_unlink("/fooz"));
  ASSERT_NE(SEM_FAILED, (b = sem_open("/barz", O_CREAT, 0644, 0)));
  ASSERT_SYS(0, 0, sem_unlink("/barz"));
  SPAWN(fork);
  ASSERT_SYS(0, 0, sem_post(a));
  ASSERT_SYS(0, 0, sem_wait(b));
  PARENT();
  ASSERT_SYS(0, 0, sem_wait(a));
  ASSERT_SYS(0, 0, sem_post(b));
  WAIT(exit, 0);
  ASSERT_SYS(0, 0, sem_close(b));
  ASSERT_FALSE(testlib_memoryexists(b));
  ASSERT_SYS(0, 0, sem_close(a));
  ASSERT_FALSE(testlib_memoryexists(a));
}

TEST(sem_open, openReadonlyAfterUnlink_enoent) {
  sem_t *sem;
  sem_unlink("/fooz");
  ASSERT_NE(SEM_FAILED, (sem = sem_open("/fooz", O_CREAT, 0644, 0)));
  ASSERT_EQ(0, sem_unlink("/fooz"));
  ASSERT_EQ(SEM_FAILED, sem_open("/fooz", O_RDONLY));
  ASSERT_EQ(ENOENT, errno);
  ASSERT_EQ(0, sem_close(sem));
}

TEST(sem_open, openReadonlyAfterIndependentUnlinkAndRecreate_returnsNewOne) {
  if (1) return;
  sem_t *a, *b;
  ASSERT_NE(SEM_FAILED, (a = sem_open("/fooz", O_CREAT, 0644, 0)));
  SPAWN(fork);
  ASSERT_EQ(0, sem_unlink("/fooz"));
  ASSERT_NE(SEM_FAILED, (b = sem_open("/fooz", O_CREAT, 0644, 0)));
  ASSERT_NE(a, b);
  ASSERT_SYS(0, 0, sem_post(a));
  ASSERT_SYS(0, 0, sem_wait(b));
  ASSERT_EQ(0, sem_close(b));
  PARENT();
  ASSERT_SYS(0, 0, sem_wait(a));
  ASSERT_NE(SEM_FAILED, (b = sem_open("/fooz", O_RDONLY)));
  ASSERT_NE(a, b);
  ASSERT_SYS(0, 0, sem_post(b));
  ASSERT_EQ(0, sem_close(b));
  WAIT(exit, 0);
  ASSERT_EQ(0, sem_close(a));
}

TEST(sem_close, openTwiceCloseOnce_stillMapped) {
  if (1) return;
  sem_t *a, *b;
  char name[] = "smXXXXXX";
  ASSERT_NE(NULL, mktemp(name));
  ASSERT_NE(SEM_FAILED, (a = sem_open(name, O_CREAT | O_EXCL, 0600, 0)));
  ASSERT_NE(SEM_FAILED, (b = sem_open(name, 0)));
  ASSERT_SYS(0, 0, sem_unlink(name));
  ASSERT_SYS(0, 0, sem_close(a));
  ASSERT_SYS(0, 0, sem_post(a));
  ASSERT_SYS(0, 0, sem_close(b));
}
