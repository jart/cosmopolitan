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
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/mfd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/s.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
// clang-format off

__static_yoink("zipos");

int fds[2];
char buf[8];
uint8_t elf_buf[4096];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  if (IsFreebsd()) exit(0);           // TODO: fixme on freebsd
  if (IsLinux() && !__is_linux_2_6_23()) exit(0);  // TODO: fixme on old linux
  // linux fexecve relies on execve from /proc
  if (IsLinux()) {
    struct stat st;
    if (stat("/proc/self/fd", &st) != 0 || !S_ISDIR(st.st_mode)) {
      exit(0);
    }
  }
}

TEST(fexecve, elfIsUnreadable_mayBeExecuted) {
  if (!IsLinux() && !IsFreebsd()) return;
  testlib_extract("/zip/echo.elf", "echo", 0111);
  ASSERT_SYS(0, 0, pipe2(fds, O_CLOEXEC));
  SPAWN(vfork);
  ASSERT_SYS(0, 1, dup2(4, 1));
  ASSERT_SYS(0, 5, open("echo", O_EXEC | O_CLOEXEC));
  if (IsFreebsd()) ASSERT_SYS(0, 1, lseek(5, 1, SEEK_SET));
  ASSERT_SYS(0, 0,
             fexecve(5, (char *const[]){"echo", "hi", 0}, (char *const[]){0}));
  exit(1);
  EXITS(0);
  bzero(buf, 8);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 3, read(3, buf, 7));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_STREQ("hi\n", buf);
}

TEST(fexecve, memfd_create) {
  if (!IsLinux()) return;
  int life_fd = open("/zip/life.elf", O_RDONLY);
  ASSERT_NE(-1, life_fd);
  int fd = memfd_create("foo", MFD_CLOEXEC);
  if(fd == -1) {
    ASSERT_EQ(ENOSYS, errno);
    return;
  }
  while(1) {
    const ssize_t bytes_read = read(life_fd, elf_buf, sizeof(elf_buf));
    if (bytes_read <= 0) {
      ASSERT_LE(0, bytes_read);
      break;
    }
    ASSERT_EQ(bytes_read, write(fd, elf_buf, bytes_read));
  }
  ASSERT_SYS(0, 0, close(life_fd));
  SPAWN(vfork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  ASSERT_SYS(0, 0, close(fd));
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
  if (!IsLinux() && !IsFreebsd()) return;
  int fd = open("/zip/zipread.com", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}
