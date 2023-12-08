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
#include "libc/sysv/consts/sig.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/enum/context.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thread.h"
#include "libc/runtime/clktck.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/poll.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"

atomic_bool ready;
atomic_bool didit;
atomic_bool isdone;
atomic_bool gotsig;

void SetUp(void) {
  ready = false;
  didit = false;
  isdone = false;
  gotsig = false;
}

void Teleport(long rdi, long rsi, long rdx, long rcx) {
  ASSERT_EQ(0x1111111111111111, rdi);
  ASSERT_EQ(0x2222222222222222, rsi);
  ASSERT_EQ(0x3333333333333333, rdx);
  ASSERT_EQ(0x4444444444444444, rcx);
  didit = true;
  pthread_exit(0);
}

void *Worker(void *arg) {
  for (;;) {
    ready = true;
  }
}

TEST(SetThreadContext, test) {
  pthread_t th;
  if (!IsWindows()) return;
  ASSERT_EQ(0, pthread_create(&th, 0, Worker, 0));
  while (!ready) donothing;
  usleep(1000);
  int64_t hand = _pthread_syshand((struct PosixThread *)th);
  ASSERT_EQ(0, SuspendThread(hand));
  struct NtContext nc;
  nc.ContextFlags = kNtContextFull;
  ASSERT_NE(0, GetThreadContext(hand, &nc));
  nc.Rsp = (nc.Rsp - 128) & -16;
  *(long *)(nc.Rsp -= 8) = nc.Rip;
  nc.Rip = (long)Teleport;
  nc.Rdi = 0x1111111111111111;
  nc.Rsi = 0x2222222222222222;
  nc.Rdx = 0x3333333333333333;
  nc.Rcx = 0x4444444444444444;
  ASSERT_NE(0, SetThreadContext(hand, &nc));
  ASSERT_EQ(1, ResumeThread(hand));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_TRUE(didit);
}

int pfds[2];

void OnSig(int sig) {
  gotsig = true;
}

void *Worker2(void *arg) {
  do {
    ASSERT_SYS(EINTR, -1, poll(&(struct pollfd){pfds[0], POLLIN}, 1, -1u));
  } while (!isdone);
  didit = true;
  return 0;
}

TEST(poll, interrupt) {
  pthread_t th;
  signal(SIGUSR1, OnSig);
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_EQ(0, pthread_create(&th, 0, Worker2, 0));
  for (int i = 0; i < 20; ++i) {
    ASSERT_EQ(0, pthread_kill(th, SIGUSR1));
    usleep(1e6 / CLK_TCK * 2);
  }
  isdone = true;
  ASSERT_EQ(0, pthread_kill(th, SIGUSR1));
  ASSERT_EQ(0, pthread_join(th, 0));
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
  ASSERT_TRUE(gotsig);
  ASSERT_TRUE(didit);
}

TEST(raise, zero) {
  ASSERT_SYS(0, 0, raise(0));
}
