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
#include "libc/calls/struct/cpuset.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/popcnt.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/proc/posix_spawn.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

void SetUp(void) {
  if (!IsLinux() && !IsFreebsd() && !IsWindows()) {
    exit(0);
  }
  if (IsFreebsd() && getuid() != 0) {
    exit(0);
  }
}

TEST(sched_getaffinity, firstOnly) {
  cpu_set_t x, y;
  CPU_ZERO(&x);
  CPU_SET(0, &x);
  ASSERT_SYS(0, 0, sched_setaffinity(0, sizeof(x), &x));
  ASSERT_SYS(0, 0, sched_getaffinity(0, sizeof(y), &y));
  EXPECT_EQ(1, CPU_COUNT(&y));
  EXPECT_TRUE(CPU_ISSET(0, &y));
  EXPECT_FALSE(CPU_ISSET(1, &y));
}

TEST(sched_getaffinity, secondOnly) {
  if (__get_cpu_count() < 2) return;
  cpu_set_t x, y;
  CPU_ZERO(&x);
  CPU_SET(1, &x);
  ASSERT_SYS(0, 0, sched_setaffinity(0, sizeof(x), &x));
  ASSERT_SYS(0, 0, sched_getaffinity(0, sizeof(y), &y));
  EXPECT_EQ(1, CPU_COUNT(&y));
  EXPECT_FALSE(CPU_ISSET(0, &y));
  EXPECT_TRUE(CPU_ISSET(1, &y));
}

TEST(sched_setaffinity, isInheritedAcrossFork) {
  cpu_set_t x, y;
  CPU_ZERO(&x);
  CPU_SET(0, &x);
  ASSERT_SYS(0, 0, sched_setaffinity(0, sizeof(x), &x));
  SPAWN(fork);
  ASSERT_SYS(0, 0, sched_getaffinity(0, sizeof(y), &y));
  EXPECT_EQ(1, CPU_COUNT(&y));
  EXPECT_TRUE(CPU_ISSET(0, &y));
  EXPECT_FALSE(CPU_ISSET(1, &y));
  EXITS(0);
}

__attribute__((__constructor__)) static void init(void) {
  cpu_set_t y;
  switch (atoi(nulltoempty(getenv("THE_DOGE")))) {
    case 42:
      ASSERT_SYS(0, 0, sched_getaffinity(0, sizeof(y), &y));
      EXPECT_EQ(1, CPU_COUNT(&y));
      EXPECT_TRUE(CPU_ISSET(0, &y));
      EXPECT_FALSE(CPU_ISSET(1, &y));
      exit(42);
    default:
      break;
  }
}

#ifdef __x86_64__
TEST(sched_setaffinity, isInheritedAcrossExecve) {
  cpu_set_t x;
  CPU_ZERO(&x);
  CPU_SET(0, &x);
  ASSERT_SYS(0, 0, sched_setaffinity(0, sizeof(x), &x));
  int ws, pid;
  char *prog = GetProgramExecutableName();
  char *args[] = {program_invocation_name, NULL};
  char *envs[] = {"THE_DOGE=42", NULL};
  EXPECT_EQ(0, posix_spawn(&pid, prog, NULL, NULL, args, envs));
  EXPECT_NE(-1, waitpid(pid, &ws, 0));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
}
#endif /* __x86_64__ */

TEST(sched_getaffinity, getpid) {
  cpu_set_t x, y;
  CPU_ZERO(&x);
  CPU_SET(0, &x);
  ASSERT_SYS(0, 0, sched_setaffinity(getpid(), sizeof(x), &x));
  ASSERT_SYS(0, 0, sched_getaffinity(getpid(), sizeof(y), &y));
  EXPECT_EQ(1, CPU_COUNT(&y));
  EXPECT_TRUE(CPU_ISSET(0, &y));
  EXPECT_FALSE(CPU_ISSET(1, &y));
}

TEST(pthread_getaffinity, getpid) {
  cpu_set_t x, y;
  CPU_ZERO(&x);
  CPU_SET(0, &x);
  ASSERT_SYS(0, 0, pthread_setaffinity_np(pthread_self(), sizeof(x), &x));
  if (IsWindows()) return;  // win32 doesn't define GetThreadAffinityMask ;_;
  ASSERT_SYS(0, 0, pthread_getaffinity_np(pthread_self(), sizeof(y), &y));
  EXPECT_EQ(1, CPU_COUNT(&y));
  EXPECT_TRUE(CPU_ISSET(0, &y));
  EXPECT_FALSE(CPU_ISSET(1, &y));
}
