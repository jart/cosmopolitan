/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigset.h"
#include "libc/dce.h"
#include "libc/nt/process.h"
#include "libc/nt/thread.h"
#include "libc/runtime/clktck.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

// test we can:
// 1. raise the same signal inside the signal handler
// 2. that the signal will trigger immediately after handling

struct SharedMemory {
  pthread_t target;
  atomic_bool ready;
  atomic_bool got_signal;
  atomic_bool handler_returned;
} * shm;

void OnSig(int sig) {
  signal(SIGUSR1, SIG_DFL);
  raise(SIGUSR1);
  shm->got_signal = true;
}

void SetUpOnce(void) {
  signal(SIGUSR1, OnSig);
  shm = _mapshared(sizeof(*shm));
}

void TearDownOnce(void) {
  munmap(shm, sizeof(*shm));
}

void SetUp(void) {
  bzero(shm, sizeof(*shm));
}

void *Killer(void *arg) {
  ASSERT_EQ(0, pthread_kill(shm->target, SIGUSR1));
  return 0;
}

void *Killed(void *arg) {
  shm->ready = true;
  while (!shm->got_signal) donothing;
  shm->handler_returned = true;
  return 0;
}

TEST(handkill, raise) {
  SPAWN(fork);
  raise(SIGUSR1);
  shm->handler_returned = true;
  TERMS(SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}

TEST(handkill, main2thread_async) {
  SPAWN(fork);
  pthread_t th;
  shm->target = pthread_self();
  pthread_create(&th, 0, Killed, 0);
  while (!shm->ready) donothing;
  ASSERT_EQ(0, pthread_kill(th, SIGUSR1));
  ASSERT_EQ(0, pthread_join(th, 0));
  TERMS(SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}

TEST(handkill, thread2main_async) {
  SPAWN(fork);
  pthread_t th;
  shm->target = pthread_self();
  pthread_create(&th, 0, Killer, 0);
  while (!shm->got_signal) donothing;
  shm->handler_returned = true;
  pthread_join(th, 0);
  TERMS(SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}

TEST(handkill, thread2thread_async) {
  SPAWN(fork);
  pthread_t th;
  pthread_create(&shm->target, 0, Killed, 0);
  pthread_create(&th, 0, Killer, 0);
  pthread_join(shm->target, 0);
  pthread_join(th, 0);
  TERMS(SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}

TEST(handkill, process_async) {
  if (IsWindows()) return;
  SPAWN(fork);
  shm->ready = true;
  while (!shm->got_signal) donothing;
  shm->handler_returned = true;
  PARENT();
  while (!shm->ready) donothing;
  ASSERT_SYS(0, 0, kill(child, SIGUSR1));
  WAIT(term, SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}

TEST(handkill, process_pause) {
  if (IsWindows()) return;
  SPAWN(fork);
  shm->ready = true;
  pause();
  shm->handler_returned = true;
  PARENT();
  while (!shm->ready) donothing;
  usleep(1e6 / CLK_TCK * 2);
  ASSERT_SYS(0, 0, kill(child, SIGUSR1));
  WAIT(term, SIGUSR1);
  EXPECT_TRUE(shm->got_signal);
  EXPECT_FALSE(shm->handler_returned);
}
