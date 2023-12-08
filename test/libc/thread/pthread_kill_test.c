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
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

int fds[2];
atomic_bool ready;
atomic_bool was_completed;
volatile pthread_t gottid;
_Thread_local atomic_int gotsig;

void SetUp(void) {
  gotsig = 0;
  gottid = 0;
  ready = false;
  was_completed = false;
  // strace_enabled(+1);
}

void TearDown(void) {
  CheckForFileLeaks();
}

void OnSig(int sig) {
  gotsig = sig;
  gottid = pthread_self();
}

void WaitUntilReady(void) {
  while (!ready) pthread_yield();
  ASSERT_EQ(0, errno);
  ASSERT_SYS(0, 0, usleep(100000));
}

void *SleepWorker(void *arg) {
  ready = true;
  ASSERT_SYS(EINTR, -1, usleep(30 * 1e6));
  ASSERT_EQ(SIGUSR1, gotsig);
  was_completed = true;
  return 0;
}

TEST(pthread_kill, canInterruptSleepOperation) {
  pthread_t t;
  sighandler_t old = signal(SIGUSR1, OnSig);
  ASSERT_EQ(0, pthread_create(&t, 0, SleepWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(0, gotsig);
  signal(SIGUSR1, old);
}

void *ReadWorker(void *arg) {
  char buf[8] = {0};
  ready = true;
  ASSERT_SYS(EINTR, -1, read(fds[0], buf, 8));
  ASSERT_EQ(pthread_self(), gottid);
  ASSERT_EQ(SIGUSR1, gotsig);
  was_completed = true;
  return 0;
}

TEST(pthread_kill, canInterruptReadOperation) {
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_EQ(0, pthread_create(&t, 0, ReadWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(t, gottid);
  ASSERT_EQ(0, gotsig);
  ASSERT_SYS(0, 0, close(fds[0]));
  ASSERT_SYS(0, 0, close(fds[1]));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
}

void *RestartReadWorker(void *arg) {
  char buf;
  ready = true;
  ASSERT_SYS(0, 1, read(fds[0], &buf, 1));
  ASSERT_EQ(pthread_self(), gottid);
  ASSERT_EQ(SIGUSR1, gotsig);
  ASSERT_EQ('x', buf);
  was_completed = true;
  return 0;
}

TEST(pthread_kill, canRestartReadOperation) {
  pthread_t t;
  signal(SIGUSR1, OnSig);
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_EQ(0, pthread_create(&t, 0, RestartReadWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_SYS(0, 1, write(fds[1], "x", 1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(t, gottid);
  ASSERT_EQ(0, gotsig);
  ASSERT_SYS(0, 0, close(fds[0]));
  ASSERT_SYS(0, 0, close(fds[1]));
  signal(SIGUSR1, SIG_DFL);
}

void *SocketReadWorker(void *arg) {
  char buf[8] = {0};
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ready = true;
  ASSERT_SYS(EINTR, -1, read(fds[0], buf, 8));
  ASSERT_EQ(pthread_self(), gottid);
  ASSERT_EQ(SIGUSR1, gotsig);
  ASSERT_SYS(0, 0, close(3));
  was_completed = true;
  return 0;
}

TEST(pthread_kill, canInterruptSocketReadOperation) {
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_EQ(0, pthread_create(&t, 0, SocketReadWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(t, gottid);
  ASSERT_EQ(0, gotsig);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
}

void *SocketAcceptWorker(void *arg) {
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, listen(3, 1));
  ready = true;
  ASSERT_SYS(EINTR, -1, accept(3, 0, 0));
  ASSERT_EQ(pthread_self(), gottid);
  ASSERT_EQ(SIGUSR1, gotsig);
  ASSERT_SYS(0, 0, close(3));
  was_completed = true;
  return 0;
}

TEST(pthread_kill, canInterruptSocketAcceptOperation) {
  if (IsWindows()) return;  // TODO(jart): BAH
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_EQ(0, pthread_create(&t, 0, SocketAcceptWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(t, gottid);
  ASSERT_EQ(0, gotsig);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
}

void *WriteWorker(void *arg) {
  char buf[2048] = {0};
  ready = true;
  for (;;) {
    int rc = write(fds[1], buf, 2048);
    if (rc != 2048) {
      ASSERT_EQ(-1, rc);
      ASSERT_EQ(EINTR, errno);
      ASSERT_EQ(SIGUSR2, gotsig);
      was_completed = true;
      return 0;
    }
  }
}

TEST(pthread_kill, canCancelWriteOperation) {
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSig};
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &sa, &oldsa));
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_EQ(0, pthread_create(&t, 0, WriteWorker, 0));
  WaitUntilReady();
  ASSERT_EQ(0, pthread_kill(t, SIGUSR2));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(was_completed);
  ASSERT_EQ(0, gotsig);
  ASSERT_SYS(0, 0, close(fds[0]));
  ASSERT_SYS(0, 0, close(fds[1]));
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &oldsa, 0));
}

volatile unsigned got_sig_async;
volatile pthread_t cpu_worker_th;
volatile unsigned is_wasting_cpu;
volatile unsigned exited_original_loop;

void OnSigAsync(int sig) {
  ASSERT_TRUE(pthread_equal(cpu_worker_th, pthread_self()));
  got_sig_async = 1;
}

void *CpuWorker(void *arg) {
  cpu_worker_th = pthread_self();
  while (!got_sig_async) {
    is_wasting_cpu = 1;
  }
  exited_original_loop = 1;
  return 0;
}

TEST(pthread_kill, canAsynchronouslyRunHandlerInsideTargetThread) {
  ASSERT_NE(0, __get_tls()->tib_tid);
  pthread_t t;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_handler = OnSigAsync};
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_EQ(0, pthread_create(&t, 0, CpuWorker, 0));
  while (!is_wasting_cpu) donothing;
  EXPECT_EQ(0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, 0));
  ASSERT_TRUE(got_sig_async);
  ASSERT_TRUE(exited_original_loop);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
  ASSERT_EQ(0, gotsig);
  ASSERT_NE(0, __get_tls()->tib_tid);
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
  ASSERT_NE(0, __get_tls()->tib_tid);
  SPAWN(fork);
  pthread_t t;
  ASSERT_EQ(0, pthread_create(&t, 0, FunWorker, 0));
  while (!is_having_fun) sched_yield();
  ASSERT_SYS(0, 0, pthread_kill(t, SIGKILL));
  for (;;) sched_yield();
  TERMS(SIGKILL);
  ASSERT_NE(0, __get_tls()->tib_tid);
}

void *SuspendWorker(void *arg) {
  sigset_t ss;
  sigemptyset(&ss);
  ASSERT_SYS(EINTR, -1, sigsuspend(&ss));
  return (void *)(long)gotsig;
}

TEST(pthread_kill, canInterruptSigsuspend) {
  ASSERT_NE(0, __get_tls()->tib_tid);
  int tid;
  void *res;
  pthread_t t;
  sigset_t ss, oldss;
  sighandler_t oldsig;
  sigemptyset(&ss);
  sigaddset(&ss, SIGUSR1);
  oldsig = signal(SIGUSR1, OnSig);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &ss, &oldss));
  ASSERT_EQ(0, pthread_create(&t, 0, SuspendWorker, 0));
  ASSERT_EQ(0, pthread_getunique_np(t, &tid));
  ASSERT_SYS(0, 0, pthread_kill(t, SIGUSR1));
  ASSERT_EQ(0, pthread_join(t, &res));
  ASSERT_EQ(0, gotsig);
  ASSERT_EQ(SIGUSR1, (intptr_t)res);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldss, 0));
  signal(SIGUSR1, oldsig);
}
