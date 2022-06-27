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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/io.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("zip_uri_support");

int ws, pid;
char testlib_enable_tmp_setup_teardown;

__attribute__((__constructor__)) static void init(void) {
  pledge("stdio rpath wpath cpath fattr proc exec", 0);
  errno = 0;
}

bool UsingBinfmtMisc(void) {
  return fileexists("/proc/sys/fs/binfmt_misc/APE");
}

// see: #431
// todo(jart): figure out what is wrong with github actions
// thetanil: same issue reproducible on my debian 5.10
// bool HasMzHeader(const char *path) {
//   char buf[2] = {0};
//   open(path, O_RDONLY);
//   read(3, buf, 2);
//   close(3);
//   return buf[0] == 'M' && buf[1] == 'Z';
// }

void Extract(const char *from, const char *to, int mode) {
  ASSERT_SYS(0, 3, open(from, O_RDONLY), "%s %s", from, to);
  ASSERT_SYS(0, 4, creat(to, mode));
  ASSERT_NE(-1, _copyfd(3, 4, -1));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
}

void SetUp(void) {
  ASSERT_SYS(0, 0, mkdir("tmp", 0755));
  ASSERT_SYS(0, 0, mkdir("bin", 0755));
  Extract("/zip/tiny64.elf", "bin/tiny64.elf", 0755);
  // Extract("/zip/pylife.com", "bin/pylife.com", 0755);
  Extract("/zip/life-nomod.com", "bin/life-nomod.com", 0755);
  Extract("/zip/life-classic.com", "bin/life-classic.com", 0755);
  setenv("TMPDIR", "tmp", true);
  if (IsOpenbsd()) {
    // printf is in /usr/bin/printf on openbsd...
    setenv("PATH", "/bin:/usr/bin", true);
  } else if (!IsWindows()) {
    setenv("PATH", "/bin", true);
  }
}

////////////////////////////////////////////////////////////////////////////////

TEST(execve, system_elf) {
  if (!IsLinux()) return;
  ws = system("bin/tiny64.elf");
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
  system("cp bin/tiny64.elf /tmp/tiny64.elf");
}

TEST(execve, fork_elf) {
  if (!IsLinux()) return;
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    execl("bin/tiny64.elf", "bin/tiny64.elf", 0);
    _Exit(127);
  }
  ASSERT_EQ(pid, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
}

TEST(execve, vfork_elf) {
  if (!IsLinux()) return;
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid) {
    execl("bin/tiny64.elf", "bin/tiny64.elf", 0);
    _Exit(127);
  }
  ASSERT_EQ(pid, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(42, WEXITSTATUS(ws));
}

////////////////////////////////////////////////////////////////////////////////

TEST(execve, system_apeNoModifySelf) {
  if (IsWindows()) return;  // todo(jart): wut
  for (int i = 0; i < 2; ++i) {
    ws = system("bin/life-nomod.com");
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // EXPECT_TRUE(HasMzHeader("bin/life-nomod.com"));
    system("cp bin/life-nomod.com /tmp/life-nomod.com");
  }
}

TEST(execve, fork_apeNoModifySelf) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = fork()));
    if (!pid) {
      execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // EXPECT_TRUE(HasMzHeader("bin/life-nomod.com"));
  }
}

TEST(execve, vfork_apeNoModifySelf) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = vfork()));
    if (!pid) {
      execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // EXPECT_TRUE(HasMzHeader("bin/life-nomod.com"));
  }
}

////////////////////////////////////////////////////////////////////////////////

TEST(execve, system_apeClassic) {
  if (IsWindows()) return;  // todo(jart): wut
  for (int i = 0; i < 2; ++i) {
    system("bin/life-classic.com");
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // if (UsingBinfmtMisc()) {
    //  EXPECT_TRUE(HasMzHeader("bin/life-classic.com"));
    // }
  }
}

TEST(execve, fork_apeClassic) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = fork()));
    if (!pid) {
      execl("bin/life-classic.com", "bin/life-classic.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // if (UsingBinfmtMisc()) {
    //  EXPECT_TRUE(HasMzHeader("bin/life-classic.com"));
    // }
  }
}

TEST(execve, vfork_apeClassic) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = vfork()));
    if (!pid) {
      execl("bin/life-classic.com", "bin/life-classic.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    // see: HasMzHeader()
    // if (UsingBinfmtMisc()) {
    //   EXPECT_TRUE(HasMzHeader("bin/life-classic.com"));
    // }
  }
}

////////////////////////////////////////////////////////////////////////////////
#if 0  // not worth depending on THIRD_PARTY_PYTHON for this test

TEST(execve, system_apeNoMod3mb) {
  if (IsWindows()) return;  // todo(jart): wut
  for (int i = 0; i < 2; ++i) {
    system("bin/pylife.com");
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    EXPECT_TRUE(HasMzHeader("bin/pylife.com"));
  }
}

TEST(execve, fork_apeNoMod3mb) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = fork()));
    if (!pid) {
      execl("bin/pylife.com", "bin/pylife.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    EXPECT_TRUE(HasMzHeader("bin/pylife.com"));
  }
}

TEST(execve, vfork_apeNoMod3mb) {
  for (int i = 0; i < 2; ++i) {
    ASSERT_NE(-1, (pid = vfork()));
    if (!pid) {
      execl("bin/pylife.com", "bin/pylife.com", 0);
      _Exit(127);
    }
    ASSERT_EQ(pid, wait(&ws));
    EXPECT_TRUE(WIFEXITED(ws));
    EXPECT_EQ(42, WEXITSTATUS(ws));
    EXPECT_TRUE(HasMzHeader("bin/pylife.com"));
  }
}

#endif
////////////////////////////////////////////////////////////////////////////////

void SystemElf(void) {
  system("bin/tiny64.elf");
}

void ForkElf(void) {
  if (!(pid = fork())) {
    execl("bin/tiny64.elf", "bin/tiny64.elf", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

void VforkElf(void) {
  if (!(pid = vfork())) {
    execl("bin/tiny64.elf", "bin/tiny64.elf", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

void SystemNoMod(void) {
  system("bin/life-nomod.com");
}

void ForkNoMod(void) {
  if (!(pid = fork())) {
    execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

void VforkNoMod(void) {
  if (!(pid = vfork())) {
    execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

void SystemClassic(void) {
  system("bin/life-classic.com");
}

void ForkClassic(void) {
  if (!(pid = fork())) {
    execl("bin/life-classic.com", "bin/life-classic.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

void VforkClassic(void) {
  if (!(pid = vfork())) {
    execl("bin/life-classic.com", "bin/life-classic.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////

void SystemNoMod3mb(void) {
  system("bin/life-nomod.com");
}

void ForkNoMod3mb(void) {
  if (!(pid = fork())) {
    execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

void VforkNoMod3mb(void) {
  if (!(pid = vfork())) {
    execl("bin/life-nomod.com", "bin/life-nomod.com", 0);
    _Exit(127);
  }
  waitpid(pid, 0, 0);
}

BENCH(execve, bench1) {
  if (IsLinux()) {
    EZBENCH2("ForkElf", donothing, ForkElf());
    EZBENCH2("VforkElf", donothing, VforkElf());
    EZBENCH2("SystemElf", donothing, SystemElf());
    kprintf("\n");
  }

  EZBENCH2("ForkApeClassic", donothing, ForkClassic());
  EZBENCH2("VforkApeClassic", donothing, VforkClassic());
  if (!IsWindows()) {
    EZBENCH2("SystemApeClassic", donothing, SystemClassic());
  }
  kprintf("\n");

  EZBENCH2("ForkApeNoMod", donothing, ForkNoMod());
  EZBENCH2("VforkApeNoMod", donothing, VforkNoMod());
  if (!IsWindows()) {
    EZBENCH2("SystemApeNoMod", donothing, SystemNoMod());
  }
  kprintf("\n");

  EZBENCH2("ForkNoMod3mb", donothing, ForkNoMod3mb());
  EZBENCH2("VforkNoMod3mb", donothing, VforkNoMod3mb());
  if (!IsWindows()) {
    EZBENCH2("SystemNoMod3mb", donothing, SystemNoMod3mb());
  }
}
