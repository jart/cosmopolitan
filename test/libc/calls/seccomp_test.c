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
#include "libc/calls/internal.h"
#include "libc/calls/struct/bpf.internal.h"
#include "libc/calls/struct/filter.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/seccomp.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "tool/net/sandbox.h"

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio proc", 0));
}

// It's been reported that Chromebooks return EINVAL here.
bool CanUseSeccomp(void) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    if (seccomp(SECCOMP_SET_MODE_STRICT, 0, 0) != -1) {
      _Exit1(0);
    } else {
      _Exit1(1);
    }
  }
  EXPECT_NE(-1, wait(&ws));
  return WIFEXITED(ws) && !WEXITSTATUS(ws);
}

void SetUp(void) {
  if (!IsLinux() || !__is_linux_2_6_23() || !CanUseSeccomp()) {
    exit(0);
  }
}

TEST(seccompStrictMode, evilProcess_getsKill9d) {
  int ws, pid;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_EQ(0, seccomp(SECCOMP_SET_MODE_STRICT, 0, 0));
    open("/etc/passwd", O_RDWR);
    _Exit1(127);
  }
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFSIGNALED(ws));
  EXPECT_EQ(SIGKILL, WTERMSIG(ws));
}

TEST(seccompStrictMode, goodProcess_isAuthorized) {
  int ws, pid;
  int pfds[2];
  char buf[3] = {0};
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_EQ(0, seccomp(SECCOMP_SET_MODE_STRICT, 0, 0));
    write(pfds[1], "hi", 3);
    _Exit1(0);
  }
  EXPECT_SYS(0, 0, close(pfds[1]));
  EXPECT_SYS(0, 3, read(pfds[0], buf, 3));
  EXPECT_SYS(0, 0, close(pfds[0]));
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_STREQ("hi", buf);
}

TEST(seccompFilter, isSoMuchBetter) {
  char buf[3] = {0};
  int ws, pid, pfds[2];
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    struct sock_filter filter[] = {
        _SECCOMP_MACHINE(AUDIT_ARCH_X86_64),  //
        _SECCOMP_LOAD_SYSCALL_NR(),           //
        _SECCOMP_ALLOW_SYSCALL(0x0013),       // readv
        _SECCOMP_ALLOW_SYSCALL(0x0014),       // writev
        _SECCOMP_ALLOW_SYSCALL(0x0000),       // read
        _SECCOMP_ALLOW_SYSCALL(0x0001),       // write
        _SECCOMP_ALLOW_SYSCALL(0x0003),       // close
        _SECCOMP_ALLOW_SYSCALL(0x000f),       // rt_sigreturn
        _SECCOMP_ALLOW_SYSCALL(0x00e7),       // exit_group
        _SECCOMP_ALLOW_SYSCALL(0x0009),       // mmap
        _SECCOMP_ALLOW_SYSCALL(0x0106),       // newfstatat
        _SECCOMP_ALLOW_SYSCALL(0x0008),       // lseek
        _SECCOMP_ALLOW_SYSCALL(0x000b),       // munmap
        _SECCOMP_ALLOW_SYSCALL(0x00e4),       // clock_gettime
        _SECCOMP_ALLOW_SYSCALL(0x003f),       // uname
        _SECCOMP_LOG_AND_RETURN_ERRNO(1),     // EPERM
    };
    struct sock_fprog prog = {
        .len = ARRAYLEN(filter),
        .filter = filter,
    };
    ASSERT_EQ(0, prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0));
    ASSERT_EQ(0, prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog));
    ASSERT_SYS(0, 3, write(pfds[1], "hi", 3));
    ASSERT_SYS(EPERM, -1, open("/etc/passwd", O_RDONLY));
    _Exit(0);
  }
  EXPECT_SYS(0, 0, close(pfds[1]));
  EXPECT_SYS(0, 3, read(pfds[0], buf, 3));
  EXPECT_SYS(0, 0, close(pfds[0]));
  EXPECT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_STREQ("hi", buf);
}
