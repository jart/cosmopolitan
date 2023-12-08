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
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <spawn.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <unistd.h>

// clang-format off
// sh -c 'build/bootstrap/make.com -j8 V=1 o//test/posix/sigchld_test.com.runs'
// clang-format on

void Assert(const char *file, int line, bool ok) {
  sigset_t ss;
  if (!ok) {
    sigfillset(&ss);
    sigprocmask(SIG_BLOCK, &ss, 0);
    fprintf(stderr, "%s:%d: assertion failed\n", file, line);
    _exit(1);
  }
}

#define FL                        __FILE__, __LINE__
#define ASSERT_TRUE(got)          Assert(FL, got)
#define EXPECT_TRUE(got)          Assert(FL, got)
#define EXPECT_EQ(want, got)      Assert(FL, (want) == (got))
#define ASSERT_EQ(want, got)      Assert(FL, (want) == (got))
#define EXPECT_NE(donotwant, got) Assert(FL, (donotwant) != (got))
#define ASSERT_SYS(err, want, got) \
  errno = 0, Assert(FL, (want) == (got)), Assert(FL, (err) == errno)
#define EXPECT_SYS(err, want, got) \
  errno = 0, Assert(FL, (want) == (got)), Assert(FL, (err) == errno)

int sigchld_pid;
volatile bool sigchld_got_signal;

// 1. Test SIGCHLD is delivered due to lack of waiters.
// 2. Test wait4() returns ECHILD when nothing remains.
// 3. Test wait4() failing doesn't clobber *ws and *ru.
// 4. Check information passed in siginfo and ucontext.
void OnSigchld(int sig, siginfo_t *si, void *arg) {
  int ws;
  struct rusage ru;
  ucontext_t *ctx = arg;
  EXPECT_NE(-1, wait4(sigchld_pid, &ws, 0, &ru));
  EXPECT_SYS(ECHILD, -1, wait4(sigchld_pid, &ws, 0, &ru));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
  EXPECT_EQ(SIGCHLD, sig);
  EXPECT_EQ(SIGCHLD, si->si_signo);
  // these fields aren't very portable
  // EXPECT_EQ(CLD_EXITED, si->si_code);
  // EXPECT_EQ(sigchld_pid, si->si_pid);
  // EXPECT_EQ(getuid(), si->si_uid);
  EXPECT_NE(NULL, ctx);
  sigchld_got_signal = true;
}

int main(int argc, char *argv[]) {
  const char *startup = getenv("EXITCODE");
  if (startup) exit(atoi(startup));
  struct sigaction newsa, oldsa;
  sigset_t oldmask, blocksigchld, unblockall;
  char *prog = argv[0];
  char *args[] = {prog, NULL};
  char *envs[] = {"EXITCODE=42", NULL};
  newsa.sa_flags = SA_SIGINFO;
  newsa.sa_sigaction = OnSigchld;
  sigemptyset(&newsa.sa_mask);
  ASSERT_SYS(0, 0, sigaction(SIGCHLD, &newsa, &oldsa));
  sigemptyset(&blocksigchld);
  sigaddset(&blocksigchld, SIGCHLD);
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &blocksigchld, &oldmask));
  EXPECT_EQ(0, posix_spawn(&sigchld_pid, prog, NULL, NULL, args, envs));
  sigemptyset(&unblockall);
  EXPECT_SYS(EINTR, -1, sigsuspend(&unblockall));
  EXPECT_TRUE(sigchld_got_signal);
  EXPECT_SYS(0, 0, sigaction(SIGCHLD, &oldsa, 0));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldmask, 0));
}
