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
#include "dsp/core/core.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/directmap.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "libc/x/xsigaction.h"
#include "libc/x/xspawn.h"

#ifdef __x86_64__

#define MEM (64 * 1024 * 1024)

static char tmpname[PATH_MAX];

void OnSigxcpu(int sig) {
  ASSERT_EQ(SIGXCPU, sig);
  _Exit(0);
}

void OnSigxfsz(int sig) {
  unlink(tmpname);
  ASSERT_EQ(SIGXFSZ, sig);
  _Exit(0);
}

TEST(setrlimit, testCpuLimit) {
  int wstatus;
  struct rlimit rlim;
  struct timespec start;
  double matrices[3][3][3];
  if (IsWindows()) return;  // of course it doesn't work on windows
  if (IsXnu()) return;      // TODO(jart): it worked before
  if (IsOpenbsd()) return;  // TODO(jart): fix flake
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, xsigaction(SIGXCPU, OnSigxcpu, 0, 0, 0));
    ASSERT_EQ(0, getrlimit(RLIMIT_CPU, &rlim));
    rlim.rlim_cur = 1;  // set soft limit to one second
    ASSERT_EQ(0, setrlimit(RLIMIT_CPU, &rlim));
    start = timespec_real();
    do {
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
      matmul3(matrices[0], matrices[1], matrices[2]);
    } while (timespec_sub(timespec_real(), start).tv_sec < 5);
    _Exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

TEST(setrlimit, testFileSizeLimit) {
  char junkdata[512];
  int i, fd, wstatus;
  struct rlimit rlim;
  if (IsWindows()) return; /* of course it doesn't work on windows */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, xsigaction(SIGXFSZ, OnSigxfsz, 0, 0, 0));
    ASSERT_EQ(0, getrlimit(RLIMIT_FSIZE, &rlim));
    rlim.rlim_cur = 1024 * 1024; /* set soft limit to one megabyte */
    ASSERT_EQ(0, setrlimit(RLIMIT_FSIZE, &rlim));
    snprintf(tmpname, sizeof(tmpname), "%s/%s.%d",
             firstnonnull(getenv("TMPDIR"), "/tmp"),
             firstnonnull(program_invocation_short_name, "unknown"), getpid());
    ASSERT_NE(-1, (fd = open(tmpname, O_RDWR | O_CREAT | O_TRUNC, 0644)));
    rngset(junkdata, 512, _rand64, -1);
    for (i = 0; i < 5 * 1024 * 1024 / 512; ++i) {
      ASSERT_EQ(512, write(fd, junkdata, 512));
    }
    close(fd);
    unlink(tmpname);
    _Exit(1);
  }
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_FALSE(WIFSIGNALED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  EXPECT_EQ(0, WTERMSIG(wstatus));
}

int SetKernelEnforcedMemoryLimit(size_t n) {
  struct rlimit rlim;
  getrlimit(RLIMIT_AS, &rlim);
  rlim.rlim_cur = n;
  return setrlimit(RLIMIT_AS, &rlim);
}

TEST(setrlimit, testMemoryLimit) {
  char *p;
  bool gotsome;
  int i, wstatus;
  if (IsXnu()) return;
  if (IsAsan()) return; /* b/c we use sys_mmap */
  ASSERT_NE(-1, (wstatus = xspawn(0)));
  if (wstatus == -2) {
    ASSERT_EQ(0, SetKernelEnforcedMemoryLimit(MEM));
    for (gotsome = false, i = 0; i < (MEM * 2) / getauxval(AT_PAGESZ); ++i) {
      p = mmap(0, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE,
               MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0);
      if (p != MAP_FAILED) {
        gotsome = true;
      } else {
        if (!IsNetbsd()) {
          // TODO(jart): what's going on with NetBSD?
          ASSERT_TRUE(gotsome);
        }
        ASSERT_EQ(ENOMEM, errno);
        _Exit(0);
      }
      rngset(p, getauxval(AT_PAGESZ), _rand64, -1);
    }
    _Exit(1);
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
    for (i = 0; i < (MEM * 2) / getauxval(AT_PAGESZ); ++i) {
      p = sys_mmap(0, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0)
              .addr;
      if (p == MAP_FAILED) {
        ASSERT_EQ(ENOMEM, errno);
        _Exit(0);
      }
      rngset(p, getauxval(AT_PAGESZ), _rand64, -1);
    }
    _Exit(1);
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
    for (i = 0; i < (MEM * 2) / getauxval(AT_PAGESZ); ++i) {
      p = sys_mmap(0, getauxval(AT_PAGESZ), PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, -1, 0)
              .addr;
      if (p == MAP_FAILED) {
        ASSERT_EQ(ENOMEM, errno);
        _Exit(0);
      }
      rngset(p, getauxval(AT_PAGESZ), _rand64, -1);
    }
    _Exit(1);
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
  _Exit(0);
}

TEST(setrlimit, isVforkSafe) {
  int ws;
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

#endif /* __x86_64__ */
