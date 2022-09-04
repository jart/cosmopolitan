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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"

////////////////////////////////////////////////////////////////////////////////
// SIGTRAP

TEST(raise, trap_sysv) {
  if (IsWindows()) return;
  signal(SIGTRAP, SIG_DFL);
  SPAWN(fork);
  raise(SIGTRAP);
  TERMS(SIGTRAP);
}

TEST(raise, trap_windows) {
  if (!IsWindows()) return;
  signal(SIGTRAP, SIG_DFL);
  SPAWN(fork);
  raise(SIGTRAP);
  EXITS(128 + SIGTRAP);
}

////////////////////////////////////////////////////////////////////////////////
// SIGFPE

TEST(raise, fpe_sysv) {
  if (IsWindows()) return;
  signal(SIGFPE, SIG_DFL);
  SPAWN(fork);
  raise(SIGFPE);
  TERMS(SIGFPE);
}

TEST(raise, fpe_windows) {
  if (!IsWindows()) return;
  signal(SIGFPE, SIG_DFL);
  SPAWN(fork);
  raise(SIGFPE);
  EXITS(128 + SIGFPE);
}

////////////////////////////////////////////////////////////////////////////////
// SIGUSR1

TEST(raise, usr1_sysv) {
  if (IsWindows()) return;
  SPAWN(fork);
  raise(SIGUSR1);
  TERMS(SIGUSR1);
}

TEST(raise, usr1_windows) {
  if (!IsWindows()) return;
  SPAWN(fork);
  raise(SIGUSR1);
  EXITS(128 + SIGUSR1);
}

////////////////////////////////////////////////////////////////////////////////
// THREADS

int threadid;

void WorkerQuit(int sig, siginfo_t *si, void *ctx) {
  ASSERT_EQ(SIGILL, sig);
  if (!IsXnu() && !IsOpenbsd()) {
    ASSERT_EQ(SI_TKILL, si->si_code);
  }
  ASSERT_EQ(threadid, gettid());
}

int Worker(void *arg, int tid) {
  struct sigaction sa = {.sa_sigaction = WorkerQuit, .sa_flags = SA_SIGINFO};
  ASSERT_EQ(0, sigaction(SIGILL, &sa, 0));
  threadid = tid;
  ASSERT_EQ(0, raise(SIGILL));
  return 0;
}

TEST(raise, threaded) {
  signal(SIGILL, SIG_DFL);
  struct spawn worker;
  ASSERT_SYS(0, 0, _spawn(Worker, 0, &worker));
  ASSERT_SYS(0, 0, _join(&worker));
}
