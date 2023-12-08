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
#if 0  // TODO(G4Vi): improve reliability of fexecve() implementation
#include "libc/calls/calls.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
// clang-format off

__static_yoink("zipos");

int fds[2];
char buf[8];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  if (IsFreebsd()) exit(0);           // TODO: fixme on freebsd
  if (IsLinux() && !__is_linux_2_6_23()) exit(0);  // TODO: fixme on old linux
}

TEST(execve, elfIsUnreadable_mayBeExecuted) {
  if (IsWindows() || IsXnu()) return;
  testlib_extract("/zip/echo", "echo", 0111);
  ASSERT_SYS(0, 0, pipe2(fds, O_CLOEXEC));
  SPAWN(vfork);
  ASSERT_SYS(0, 1, dup2(4, 1));
  ASSERT_SYS(
      0, 0,
      execve("echo", (char *const[]){"echo", "hi", 0}, (char *const[]){0}));
  notpossible;
  EXITS(0);
  bzero(buf, 8);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 3, read(3, buf, 7));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_STREQ("hi\n", buf);
}

TEST(fexecve, elfIsUnreadable_mayBeExecuted) {
  if (!IsLinux() && !IsFreebsd()) return;
  testlib_extract("/zip/echo", "echo", 0111);
  ASSERT_SYS(0, 0, pipe2(fds, O_CLOEXEC));
  SPAWN(vfork);
  ASSERT_SYS(0, 1, dup2(4, 1));
  ASSERT_SYS(0, 5, open("echo", O_EXEC | O_CLOEXEC));
  if (IsFreebsd()) ASSERT_SYS(0, 1, lseek(5, 1, SEEK_SET));
  ASSERT_SYS(0, 0,
             fexecve(5, (char *const[]){"echo", "hi", 0}, (char *const[]){0}));
  notpossible;
  EXITS(0);
  bzero(buf, 8);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 3, read(3, buf, 7));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_STREQ("hi\n", buf);
}

TEST(fexecve, memfd_create) {
  if (1) return; // TODO: fixme
  if (!IsLinux()) return;
  SPAWN(vfork);
#define TINY_ELF_PROGRAM "\
\177\105\114\106\002\001\001\000\000\000\000\000\000\000\000\000\
\002\000\076\000\001\000\000\000\170\000\100\000\000\000\000\000\
\100\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\100\000\070\000\001\000\000\000\000\000\000\000\
\001\000\000\000\005\000\000\000\000\000\000\000\000\000\000\000\
\000\000\100\000\000\000\000\000\000\000\100\000\000\000\000\000\
\200\000\000\000\000\000\000\000\200\000\000\000\000\000\000\000\
\000\020\000\000\000\000\000\000\152\052\137\152\074\130\017\005"
  int fd = memfd_create("foo", MFD_CLOEXEC);
  if (fd == -1 && errno == ENOSYS) _Exit(42);
  write(fd, TINY_ELF_PROGRAM, sizeof(TINY_ELF_PROGRAM) - 1);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

TEST(fexecve, APE) {
  if (!IsLinux() && !IsFreebsd()) return;
  testlib_extract("/zip/life-nomod.com", "life-nomod.com", 0555);
  SPAWN(fork);
  int fd = open("life-nomod.com", O_RDONLY);
  ASSERT_NE(-1, fd);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

TEST(fexecve, APE_cloexec) {
  if (!IsLinux() && !IsFreebsd()) return;
  testlib_extract("/zip/life-nomod.com", "life-nomod.com", 0555);
  SPAWN(fork);
  int fd = open("life-nomod.com", O_RDONLY | O_CLOEXEC);
  ASSERT_NE(-1, fd);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

TEST(fexecve, zipos) {
  if (!IsLinux() && !IsFreebsd()) return;
  int fd = open("/zip/life.elf", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

TEST(fexecve, ziposAPE) {
  if (!IsLinux() && !IsFreebsd()) return;
  int fd = open("/zip/life-nomod.com", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

TEST(fexecve, ziposAPEHasZipos) {
  if (1) return; // TODO: fixme
  if (!IsLinux() && !IsFreebsd()) return;
  int fd = open("/zip/zipread.com", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  ASSERT_NE(-1, fd);
  if (fd == -1 && errno == ENOSYS) _Exit(42);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

#endif
