/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/tls.h"

TEST(fork, testPipes) {
  int a, b;
  int ws, pid;
  int pipefds[2];
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    a = 31337;
    close(pipefds[0]);
    write(pipefds[1], &a, sizeof(a));
    close(pipefds[1]);
    _exit(0);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_EQ(sizeof(b), read(pipefds[0], &b, sizeof(b)));
  EXPECT_NE(-1, close(pipefds[0]));
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_EQ(31337, b);
}

TEST(fork, testSharedMemory) {
  int ws, pid;
  int stackvar;
  int *sharedvar;
  int *privatevar;
  EXPECT_NE(MAP_FAILED,
            (sharedvar = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0)));
  EXPECT_NE(MAP_FAILED,
            (privatevar = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  stackvar = 1;
  *sharedvar = 1;
  *privatevar = 1;
  EXPECT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_EQ(NULL, getenv("_FORK"));
    ++stackvar;
    ++*sharedvar;
    ++*privatevar;
    msync((void *)ROUNDDOWN((intptr_t)&stackvar, FRAMESIZE), FRAMESIZE,
          MS_SYNC);
    EXPECT_NE(-1, msync(privatevar, FRAMESIZE, MS_SYNC));
    EXPECT_NE(-1, msync(sharedvar, FRAMESIZE, MS_SYNC));
    _exit(0);
  }
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_EQ(1, stackvar);
  EXPECT_EQ(2, *sharedvar);
  EXPECT_EQ(1, *privatevar);
  EXPECT_NE(-1, munmap(sharedvar, FRAMESIZE));
  EXPECT_NE(-1, munmap(privatevar, FRAMESIZE));
}

static volatile bool gotsigusr1;
static volatile bool gotsigusr2;

static void OnSigusr1(int sig) {
  gotsigusr1 = true;
}

static void OnSigusr2(int sig) {
  gotsigusr2 = true;
}

TEST(fork, childToChild) {
  if (IsWindows()) return;  // :'(
  sigset_t mask, oldmask;
  int ws, parent, child1, child2;
  gotsigusr1 = false;
  gotsigusr2 = false;
  parent = getpid();
  signal(SIGUSR1, OnSigusr1);
  signal(SIGUSR2, OnSigusr2);
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGUSR2);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);
  ASSERT_NE(-1, (child1 = fork()));
  if (!child1) {
    kill(parent, SIGUSR2);
    sigsuspend(0);
    _Exit(!gotsigusr1);
  }
  sigsuspend(0);
  ASSERT_NE(-1, (child2 = fork()));
  if (!child2) {
    kill(child1, SIGUSR1);
    _Exit(0);
  }
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  ASSERT_NE(-1, wait(&ws));
  EXPECT_EQ(0, ws);
  sigprocmask(SIG_SETMASK, &oldmask, 0);
}

TEST(fork, preservesTlsMemory) {
  __get_tls()->tib_errno = 31337;
  SPAWN(fork);
  ASSERT_EQ(31337, __get_tls()->tib_errno);
  EXITS(0);
}

void ForkInSerial(void) {
  int pid, ws;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) _Exit(0);
  ASSERT_NE(-1, waitpid(pid, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

BENCH(fork, bench) {
  EZBENCH2("fork a", donothing, ForkInSerial());
}
