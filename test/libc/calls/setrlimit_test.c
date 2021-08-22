/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "dsp/core/core.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/fmt/fmt.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/directmap.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

#define MEM (64 * 1024 * 1024)

char tmpfile[PATH_MAX];

void OnSigxcpu(int sig) {
  ASSERT_EQ(SIGXCPU, sig);
  _exit(0);
}

void OnSigxfsz(int sig) {
  unlink(tmpfile);
  ASSERT_EQ(SIGXFSZ, sig);
  _exit(0);
}

TEST(setrlimit, testCpuLimit) {
  char *p;
  int i, wstatus;
  long double start;
  struct rlimit rlim;
  double matrices[3][3][3];
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    CHECK_EQ(0, xsigaction(SIGXCPU, OnSigxcpu, 0, 0, 0));
    ASSERT_EQ(0, getrlimit(RLIMIT_CPU, &rlim));
    rlim.rlim_cur = 1; /* set soft limit to one second */
    ASSERT_EQ(0, setrlimit(RLIMIT_CPU, &rlim));
    start = nowl();
    do {
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
    } while ((nowl() - start) < 5);
    _exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

TEST(setrlimit, testFileSizeLimit) {
  char *p;
  char junkdata[512];
  int i, fd, wstatus;
  struct rlimit rlim;
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    CHECK_EQ(0, xsigaction(SIGXFSZ, OnSigxfsz, 0, 0, 0));
    ASSERT_EQ(0, getrlimit(RLIMIT_FSIZE, &rlim));
    rlim.rlim_cur = 1024 * 1024; /* set soft limit to one megabyte */
    ASSERT_EQ(0, setrlimit(RLIMIT_FSIZE, &rlim));
    snprintf(tmpfile, sizeof(tmpfile), "%s/%s.%d",
             firstnonnull(getenv("TMPDIR"), "/tmp"),
             program_invocation_short_name, getpid());
    ASSERT_NE(-1, (fd = open(tmpfile, O_RDWR | O_CREAT | O_TRUNC)));
    rngset(junkdata, 512, rand64, -1);
    for (i = 0; i < 5 * 1024 * 1024 / 512; ++i) {
      ASSERT_EQ(512, write(fd, junkdata, 512));
    }
    close(fd);
    unlink(tmpfile);
    _exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

int SetKernelEnforcedMemoryLimit(size_t n) {
  struct rlimit rlim = {n, n};
  if (IsWindows() || IsXnu()) return -1;
  return setrlimit(!IsOpenbsd() ? RLIMIT_AS : RLIMIT_DATA, &rlim);
}

TEST(setrlimit, testMemoryLimit) {
  char *p;
  int i, wstatus;
  if (IsAsan()) return;    /* b/c we use sys_mmap */
  if (IsXnu()) return;     /* doesn't work on darwin */
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, SetKernelEnforcedMemoryLimit(MEM));
    for (i = 0; i < (MEM * 2) / PAGESIZE; ++i) {
      p = sys_mmap(0, PAGESIZE, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0)
              .addr;
      if (p == MAP_FAILED) {
        ASSERT_EQ(ENOMEM, errno);
        _exit(0);
      }
      rngset(p, PAGESIZE, rand64, -1);
    }
    _exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

TEST(setrlimit, testVirtualMemoryLimit) {
  char *p;
  int i, wstatus;
  if (IsAsan()) return;
  if (IsXnu()) return;     /* doesn't work on darwin */
  if (IsOpenbsd()) return; /* unavailable on openbsd */
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, setrlimit(RLIMIT_AS, &(struct rlimit){MEM, MEM}));
    for (i = 0; i < (MEM * 2) / PAGESIZE; ++i) {
      p = sys_mmap(0, PAGESIZE, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0)
              .addr;
      if (p == MAP_FAILED) {
        ASSERT_EQ(ENOMEM, errno);
        _exit(0);
      }
      rngset(p, PAGESIZE, rand64, -1);
    }
    _exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

TEST(setrlimit, testDataMemoryLimit) {
  char *p;
  int i, wstatus;
  if (IsAsan()) return;
  if (IsXnu()) return;     /* doesn't work on darwin */
  if (IsNetbsd()) return;  /* doesn't work on netbsd */
  if (IsFreebsd()) return; /* doesn't work on freebsd */
  if (IsLinux()) return;   /* doesn't work on gnu/systemd */
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, setrlimit(RLIMIT_DATA, &(struct rlimit){MEM, MEM}));
    for (i = 0; i < (MEM * 2) / PAGESIZE; ++i) {
      p = sys_mmap(0, PAGESIZE, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0)
              .addr;
      if (p == MAP_FAILED) {
        ASSERT_EQ(ENOMEM, errno);
        _exit(0);
      }
      rngset(p, PAGESIZE, rand64, -1);
    }
    _exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

TEST(setrlimit, testPhysicalMemoryLimit) {
  /* RLIMIT_RSS doesn't work on gnu/systemd */
  /* RLIMIT_RSS doesn't work on darwin */
  /* RLIMIT_RSS doesn't work on freebsd */
  /* RLIMIT_RSS doesn't work on netbsd */
  /* RLIMIT_RSS doesn't work on openbsd */
  /* of course it doesn't work on windows */
}

wontreturn void OnVfork(void *ctx) {
  struct rlimit *rlim;
  rlim = ctx;
  rlim->rlim_cur -= 1;
  ASSERT_EQ(0, getrlimit(RLIMIT_CPU, rlim));
  _exit(0);
}

TEST(setrlimit, isVforkSafe) {
  int pid, ws;
  struct rlimit rlim[2];
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_EQ(0, getrlimit(RLIMIT_CPU, rlim));
  ASSERT_NE(-1, (ws = xvspawn(OnVfork, rlim, 0)));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_FALSE(WIFSIGNALED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  EXPECT_EQ(0, WTERMSIG(ws));
  ASSERT_EQ(0, getrlimit(RLIMIT_CPU, rlim + 1));
  EXPECT_EQ(rlim[0].rlim_cur, rlim[1].rlim_cur);
  EXPECT_EQ(rlim[0].rlim_max, rlim[1].rlim_max);
}
