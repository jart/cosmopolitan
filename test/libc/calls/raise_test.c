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
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

TEST(raise, trap) {
  signal(SIGTRAP, SIG_DFL);
  SPAWN(fork);
  raise(SIGTRAP);
  TERMS(SIGTRAP);
}

TEST(raise, fpe) {
  signal(SIGFPE, SIG_DFL);
  SPAWN(fork);
  raise(SIGFPE);
  TERMS(SIGFPE);
}

TEST(raise, usr1) {
  SPAWN(fork);
  raise(SIGUSR1);
  TERMS(SIGUSR1);
}

int threadid;

void WorkerQuit(int sig, siginfo_t *si, void *ctx) {
  ASSERT_EQ(SIGILL, sig);
  if (!IsXnu() && !IsOpenbsd()) {
    ASSERT_EQ(SI_TKILL, si->si_code);
  }
  ASSERT_EQ(threadid, gettid());
}

void *Worker(void *arg) {
  struct sigaction sa = {.sa_sigaction = WorkerQuit, .sa_flags = SA_SIGINFO};
  ASSERT_EQ(0, sigaction(SIGILL, &sa, 0));
  threadid = gettid();
  ASSERT_EQ(0, raise(SIGILL));
  return 0;
}

TEST(raise, threaded) {
  SPAWN(fork);
  signal(SIGILL, SIG_DFL);
  pthread_t worker;
  ASSERT_EQ(0, pthread_create(&worker, 0, Worker, 0));
  ASSERT_EQ(0, pthread_join(worker, 0));
  pthread_exit(0);
  EXITS(0);
}

void OnRaise(int sig) {
}

BENCH(raise, bench) {
  signal(SIGUSR1, OnRaise);
  EZBENCH2("raise", donothing, raise(SIGUSR1));
}
