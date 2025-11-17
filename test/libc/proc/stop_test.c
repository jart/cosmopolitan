/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/proc/proc.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/testlib/testlib.h"

__attribute__((__constructor__)) static void init(void) {
  if (__argc == 2 && !strcmp(__argv[1], "--test1")) {
    raise(SIGSTOP);
    _Exit(7);
  }
}

TEST(wuntraced_fork, getsStopNotification) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    raise(SIGSTOP);
    _Exit(7);
  }
  ASSERT_SYS(0, pid, waitpid(-1, &ws, WUNTRACED));
  ASSERT_TRUE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFEXITED(ws));
  ASSERT_EQ(SIGSTOP, WSTOPSIG(ws));
  ASSERT_SYS(0, 0, kill(pid, SIGCONT));
  ASSERT_SYS(0, pid, waitpid(-1, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}

TEST(wuntraced_fork_exec, getsStopNotification) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    execl(GetProgramExecutableName(), GetProgramExecutableName(), "--test1",
          NULL);
    _Exit(127);
  }
  ASSERT_SYS(0, pid, waitpid(-1, &ws, WUNTRACED));
  ASSERT_TRUE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFEXITED(ws));
  ASSERT_EQ(SIGSTOP, WSTOPSIG(ws));
  ASSERT_SYS(0, 0, kill(pid, SIGCONT));
  ASSERT_SYS(0, pid, waitpid(-1, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}

TEST(wuntraced_vfork_exec, getsStopNotification) {
  int ws, pid;
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid) {
    execl(GetProgramExecutableName(), GetProgramExecutableName(), "--test1",
          NULL);
    _Exit(127);
  }
  ASSERT_SYS(0, pid, waitpid(-1, &ws, WUNTRACED));
  ASSERT_TRUE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFEXITED(ws));
  ASSERT_EQ(SIGSTOP, WSTOPSIG(ws));
  ASSERT_SYS(0, 0, kill(pid, SIGCONT));
  ASSERT_SYS(0, pid, waitpid(-1, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}

TEST(wait, normallyIgnoresStop) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_NE(-1, (pid = fork()));
    if (!pid) {
      usleep(4e5);
      kill(getppid(), SIGCONT);
      _Exit(0);
    }
    raise(SIGSTOP);
    _Exit(7);
  }
  ASSERT_SYS(0, pid, waitpid(-1, &ws, 0));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}

TEST(wuntraced_specific, getsStopNotification) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    raise(SIGSTOP);
    _Exit(7);
  }
  ASSERT_SYS(0, pid, waitpid(pid, &ws, WUNTRACED));
  ASSERT_TRUE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_EQ(SIGSTOP, WSTOPSIG(ws));
  ASSERT_SYS(0, 0, kill(pid, SIGCONT));
  ASSERT_SYS(0, pid, waitpid(pid, &ws, 0));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}

TEST(wait_specific, normallyIgnoresStop) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_NE(-1, (pid = fork()));
    if (!pid) {
      usleep(4e5);
      kill(getppid(), SIGCONT);
      _Exit(0);
    }
    raise(SIGSTOP);
    _Exit(7);
  }
  ASSERT_SYS(0, pid, waitpid(pid, &ws, 0));
  ASSERT_FALSE(WIFSTOPPED(ws));
  ASSERT_FALSE(WIFSIGNALED(ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(7, WEXITSTATUS(ws));
}
