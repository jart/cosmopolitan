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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/temp.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

__static_yoink("zipos");

#define N 16

int fds[2];
char buf[8];
bool HasProcFSAndMemfd = false;
void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void GenBuf(char buf[8], int x) {
  int i;
  bzero(buf, 8);
  for (i = 0; i < 7; ++i) {
    buf[i] = x & 127;  // nt doesn't respect invalid unicode?
    x >>= 1;
  }
}

__attribute__((__constructor__)) static void init(void) {
  char buf[8];
  if (__argc == 4 && !strcmp(__argv[1], "-")) {
    GenBuf(buf, atoi(__argv[2]));
    ASSERT_STREQ(buf, __argv[3]);
    exit(0);
  }
  // zipos execve requires /proc and memfd_create
  // TODO check for memfd
  if (IsLinux()) {
    struct stat st;
    HasProcFSAndMemfd = stat("/proc/self/fd", &st) == 0 && S_ISDIR(st.st_mode);
  }
}

TEST(execve, testArgPassing) {
  int i;
  char ibuf[12], buf[8];
  const char *prog = "./execve_test_prog1.com";
  testlib_extract("/zip/execve_test_prog1.com", prog, 0755);
  for (i = 0; i < N; ++i) {
    FormatInt32(ibuf, i);
    GenBuf(buf, i);
    SPAWN(vfork);
    execve(prog, (char *const[]){(char *)prog, "-", ibuf, buf, 0},
           (char *const[]){0});
    kprintf("execve failed: %m\n");
    EXITS(0);
  }
}

TEST(execve, elfIsUnreadable_mayBeExecuted) {
  if (IsWindows() || IsXnu()) return;
  testlib_extract("/zip/echo.elf", "echo", 0111);
  ASSERT_SYS(0, 0, pipe2(fds, O_CLOEXEC));
  SPAWN(vfork);
  ASSERT_SYS(0, 1, dup2(4, 1));
  ASSERT_SYS(
      0, 0,
      execve("echo", (char *const[]){"echo", "hi", 0}, (char *const[]){0}));
  exit(1);
  EXITS(0);
  bzero(buf, 8);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 3, read(3, buf, 7));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_STREQ("hi\n", buf);
}

TEST(execve, ziposELF) {
  if (IsFreebsd()) return;                        // TODO: fixme on freebsd
  if (IsLinux() && !__is_linux_2_6_23()) return;  // TODO: fixme on old linux
  if (IsLinux() && !HasProcFSAndMemfd) return;
  if (!IsLinux() && !IsFreebsd()) {
    EXPECT_SYS(ENOSYS, -1,
               execve("/zip/life.elf", (char *const[]){0}, (char *const[]){0}));
    return;
  }
  SPAWN(fork);
  execve("/zip/life.elf", (char *const[]){0}, (char *const[]){0});
  kprintf("execve failed: %m\n");
  EXITS(42);
}

TEST(execve, ziposAPE) {
  if (IsFreebsd()) return;                        // TODO: fixme on freebsd
  if (IsLinux() && !__is_linux_2_6_23()) return;  // TODO: fixme on old linux
  if (IsLinux() && !HasProcFSAndMemfd) return;
  if (!IsLinux() && !IsFreebsd()) {
    EXPECT_EQ(-1, execve("/zip/life-nomod.com", (char *const[]){0},
                         (char *const[]){0}));
    return;
  }
  SPAWN(fork);
  execve("/zip/life-nomod.com", (char *const[]){0}, (char *const[]){0});
  kprintf("execve failed: %m\n");
  EXITS(42);
}

// clang-format off
#define TINY_ELF_PROGRAM "\
\177\105\114\106\002\001\001\000\000\000\000\000\000\000\000\000\
\002\000\076\000\001\000\000\000\170\000\100\000\000\000\000\000\
\100\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\100\000\070\000\001\000\000\000\000\000\000\000\
\001\000\000\000\005\000\000\000\000\000\000\000\000\000\000\000\
\000\000\100\000\000\000\000\000\000\000\100\000\000\000\000\000\
\200\000\000\000\000\000\000\000\200\000\000\000\000\000\000\000\
\000\020\000\000\000\000\000\000\152\052\137\152\074\130\017\005"
// clang-format on

void ExecvTinyElf(const char *path) {
  int ws;
  if (!vfork()) {
    execv(path, (char *[]){(char *)path, 0});
    abort();
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void ExecvpTinyElf(const char *path) {
  int ws;
  if (!vfork()) {
    execvp(path, (char *[]){(char *)path, 0});
    abort();
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

void ExecveTinyElf(const char *path) {
  int ws;
  if (!vfork()) {
    execve(path, (char *[]){(char *)path, 0}, (char *[]){0});
    abort();
  }
  ASSERT_NE(-1, wait(&ws));
  ASSERT_EQ(42, WEXITSTATUS(ws));
}

BENCH(execve, bench) {
  if (!IsLinux()) return;
  char path[128] = "/tmp/tinyelf.XXXXXX";
  int fd = mkstemp(path);
  fchmod(fd, 0700);
  write(fd, TINY_ELF_PROGRAM, sizeof(TINY_ELF_PROGRAM));
  close(fd);
  EZBENCH2("execv", donothing, ExecvTinyElf(path));
  EZBENCH2("execvp", donothing, ExecvpTinyElf(path));
  EZBENCH2("execve", donothing, ExecveTinyElf(path));
  unlink(path);
}
