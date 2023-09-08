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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

int fds[2];
_Thread_local sig_atomic_t gotsig;

void SetUp(void) {
  if (IsXnuSilicon()) {
    fprintf(stderr, "TODO(jart): Get pthread_kill() working on XNU silicon\n");
    exit(0);
  }
}

void OnSig(int sig) {
  gotsig = 1;
}

void *ReadWorker(void *arg) {
  char buf[8];
  ASSERT_SYS(EINTR, -1, read(fds[0], buf, 8));
  ASSERT_TRUE(gotsig);
  return 0;
}

TEST(pthread_kill, canCancelReadOperation) {
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_EQ(0, pthread_create(&t, 0, ReadWorker, 0));
  ASSERT_SYS(0, 0, usleep(100000));  // potentially flaky
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_FALSE(gotsig);
  ASSERT_SYS(0, 0, close(fds[0]));
  ASSERT_SYS(0, 0, close(fds[1]));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
}

volatile unsigned got_sig_async;
volatile pthread_t cpu_worker_th;
volatile unsigned is_wasting_cpu;

void OnSigAsync(int sig) {
  ASSERT_TRUE(pthread_equal(cpu_worker_th, pthread_self()));
  got_sig_async = 1;
}

void *CpuWorker(void *arg) {
  cpu_worker_th = pthread_self();
  while (!got_sig_async) {
    is_wasting_cpu = 1;
  }
  return 0;
}

TEST(pthread_kill, canAsynchronouslyRunHandlerInsideTargetThread) {
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSigAsync};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_EQ(0, pthread_create(&t, 0, CpuWorker, 0));
  while (!is_wasting_cpu) donothing;
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(got_sig_async);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
}

volatile int is_having_fun;

void *FunWorker(void *arg) {
  for (;;) {
    is_having_fun = 1;
    sched_yield();
  }
  return 0;
}

TEST(pthread_kill, defaultThreadSignalHandlerWillKillWholeProcess) {
  SPAWN(fork);
  pthread_t t;
  ASSERT_EQ(0, pthread_create(&t, 0, FunWorker, 0));
  while (!is_having_fun) sched_yield();
  ASSERT_SYS(0, 0, pthread_kill(t, SIGKILL));
  for (;;) sched_yield();
  TERMS(SIGKILL);
}
