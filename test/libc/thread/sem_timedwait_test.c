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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/semaphore.h"
#include "libc/thread/thread.h"

void IgnoreStderr(void) {
  close(2);
  open("/dev/null", O_WRONLY);
}

TEST(sem_init, einval) {
  sem_t sem;
  ASSERT_SYS(EINVAL, -1, sem_init(&sem, 0, -1));
}

TEST(sem_post, afterDestroyed_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t sem;
  SPAWN(fork);
  signal(SIGILL, SIG_DFL);
  ASSERT_SYS(0, 0, sem_init(&sem, 0, 0));
  ASSERT_SYS(0, 0, sem_destroy(&sem));
  IgnoreStderr();
  sem_post(&sem);
  TERMS(SIGILL);
}

TEST(sem_trywait, afterDestroyed_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t sem;
  SPAWN(fork);
  signal(SIGILL, SIG_DFL);
  ASSERT_SYS(0, 0, sem_init(&sem, 0, 0));
  ASSERT_SYS(0, 0, sem_destroy(&sem));
  IgnoreStderr();
  sem_trywait(&sem);
  TERMS(SIGILL);
}

TEST(sem_wait, afterDestroyed_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t sem;
  SPAWN(fork);
  signal(SIGILL, SIG_DFL);
  ASSERT_SYS(0, 0, sem_init(&sem, 0, 0));
  ASSERT_SYS(0, 0, sem_destroy(&sem));
  IgnoreStderr();
  sem_wait(&sem);
  TERMS(SIGILL);
}

TEST(sem_timedwait, afterDestroyed_isUndefinedBehavior) {
  if (!IsModeDbg()) return;
  sem_t sem;
  SPAWN(fork);
  signal(SIGILL, SIG_DFL);
  ASSERT_SYS(0, 0, sem_init(&sem, 0, 0));
  ASSERT_SYS(0, 0, sem_destroy(&sem));
  IgnoreStderr();
  sem_timedwait(&sem, 0);
  TERMS(SIGILL);
}

void *Worker(void *arg) {
  sem_t **s = arg;
  struct timespec ts;
  ASSERT_SYS(0, 0, clock_gettime(CLOCK_REALTIME, &ts));
  ts.tv_sec += 10;
  ASSERT_SYS(0, 0, sem_post(s[0]));
  ASSERT_SYS(0, 0, sem_timedwait(s[1], &ts));
  return 0;
}

TEST(sem_timedwait, threads) {
  int i, r, n = 4;
  sem_t sm[2], *s[2] = {sm, sm + 1};
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  ASSERT_SYS(0, 0, sem_init(s[0], 0, 0));
  ASSERT_SYS(0, 0, sem_init(s[1], 0, 0));
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_create(t + i, 0, Worker, s));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_wait(s[0]));
  ASSERT_SYS(0, 0, sem_getvalue(s[0], &r));
  ASSERT_EQ(0, r);
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_post(s[1]));
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_join(t[i], 0));
  ASSERT_SYS(0, 0, sem_getvalue(s[1], &r));
  ASSERT_EQ(0, r);
  ASSERT_SYS(0, 0, sem_destroy(s[1]));
  ASSERT_SYS(0, 0, sem_destroy(s[0]));
}

TEST(sem_timedwait, processes) {
  int i, r, rc, n = 4, pshared = 1;
  sem_t *sm = _mapshared(FRAMESIZE), *s[2] = {sm, sm + 1};
  ASSERT_SYS(0, 0, sem_init(s[0], pshared, 0));
  ASSERT_SYS(0, 0, sem_init(s[1], pshared, 0));
  for (i = 0; i < n; ++i) {
    ASSERT_NE(-1, (rc = fork()));
    if (!rc) Worker(s), _Exit(0);
  }
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_wait(s[0]));
  ASSERT_SYS(0, 0, sem_getvalue(s[0], &r));
  ASSERT_EQ(0, r);
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, sem_post(s[1]));
  for (;;) {
    int ws, pid, e = errno;
    if ((pid = waitpid(0, &ws, 0)) != -1) {
      if (WIFSIGNALED(ws)) {
        kprintf("process %d terminated with %G\n", pid, WTERMSIG(ws));
        testlib_incrementfailed();
      } else if (WEXITSTATUS(ws)) {
        kprintf("process %d exited with %d\n", pid, WEXITSTATUS(ws));
        testlib_incrementfailed();
      }
    } else {
      ASSERT_EQ(ECHILD, errno);
      errno = e;
      break;
    }
  }
  ASSERT_SYS(0, 0, sem_getvalue(s[1], &r));
  ASSERT_EQ(0, r);
  ASSERT_SYS(0, 0, sem_destroy(s[1]));
  ASSERT_SYS(0, 0, sem_destroy(s[0]));
  ASSERT_SYS(0, 0, munmap(sm, FRAMESIZE));
}
