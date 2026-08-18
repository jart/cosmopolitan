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
#include "libc/calls/syscall-sysv.internal.h"
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
int o_cloexec = 0;
bool SupportsOPATH = false;
bool SupportsMemfdCreate = false;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void SetUp(void) {
  if (IsLinux()) {
    // linux fexecve relies on execve from /proc
    struct stat st;
    if (stat("/proc/self/fd", &st) != 0 || !S_ISDIR(st.st_mode)) {
      exit(0);
    }
    o_cloexec = __is_linux_2_6_23() ? O_CLOEXEC : 0;
    SupportsOPATH = __is_linux_2_6_39();
    SupportsMemfdCreate = __is_linux_3_17();
  } else {
    // TODO: FreeBSD and other OSes
    exit(0);
  }
}

TEST(fexecve, elf) {
  if (!SupportsOPATH) return;
  int extracted_mode = 0555;
  int open_flags = _O_PATH | o_cloexec;
  if (IsAarch64() && IsQemuUser()) {
    extracted_mode = 0555;
    open_flags &= ~O_CLOEXEC;
  }
  testlib_extract("/zip/life-nozip.elf", "life-nozip.elf", extracted_mode);
  SPAWN(vfork);
  ASSERT_SYS(0, 3, open("life-nozip.elf", open_flags));
  ASSERT_SYS(0, 0,
             fexecve(3, (char *const[]){"life-nozip.elf", 0}, (char *const[]){0}));
  exit(1);
  EXITS(42);
}


TEST(fexecve, elf_with_zipos) {
  int extracted_mode = 0555;
  int open_flags = _O_PATH | o_cloexec;
  if (IsAarch64() && IsQemuUser()) {
    extracted_mode = 0555;
    open_flags &= ~O_CLOEXEC;
  }
  testlib_extract("/zip/zipread.elf", "zipread.elf", extracted_mode);
  SPAWN(vfork);
  ASSERT_SYS(0, 3, open("zipread.elf", open_flags));
  ASSERT_SYS(0, 0,
             fexecve(3, (char *const[]){"zipread.elf", 0}, (char *const[]){0}));
  EXITS(42);
}

TEST(fexecve, elfIsUnreadable_mayBeExecuted) {
  if (!SupportsOPATH) return;
  int extracted_mode = 0111;
  int open_flags = _O_PATH | O_CLOEXEC;
  if (IsAarch64() && IsQemuUser()) {
    extracted_mode = 0555;
    open_flags &= ~O_CLOEXEC;
  }
  testlib_extract("/zip/echo.elf", "echo", extracted_mode);
  ASSERT_SYS(0, 0, pipe2(fds, O_CLOEXEC));
  SPAWN(vfork);
  ASSERT_SYS(0, 1, dup2(4, 1));
  ASSERT_SYS(0, 5, open("echo", open_flags));
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
  if (!SupportsMemfdCreate) return;
  int life_fd = open("/zip/life.elf", O_RDONLY);
  ASSERT_NE(-1, life_fd);
  const int memfd_flags = (IsAarch64() && IsQemuUser()) ? 0 : MFD_CLOEXEC;
  int fd = sys_memfd_create("foo", memfd_flags);
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
  testlib_extract("/zip/life-nozip", "life-nozip", 0555);
  SPAWN(fork);
  int fd = open("life-nozip", O_RDONLY);
  ASSERT_NE(-1, fd);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

TEST(fexecve, APE_with_zipos) {
  testlib_extract("/zip/life-nomod", "life-nomod", 0555);
  SPAWN(fork);
  int fd = open("life-nomod", O_RDONLY);
  ASSERT_NE(-1, fd);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

// TODO(G4Vi): This might be a bad test, APE's cannot run with O_CLOEXEC right now
TEST(fexecve, APE_cloexec) {
  if (!o_cloexec) return;
  testlib_extract("/zip/life-nomod", "life-nomod", 0555);
  SPAWN(fork);
  int fd = open("life-nomod", O_RDONLY | O_CLOEXEC);
  ASSERT_NE(-1, fd);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
}

TEST(fexecve, zipos_elf) {
  if (!SupportsMemfdCreate) return;
  int fd = open("/zip/life-nozip.elf", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

TEST(fexecve, zipos_elf_with_zipos) {
  if (!SupportsMemfdCreate) return;
  int fd = open("/zip/life.elf", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

TEST(fexecve, ziposAPE) {
  if (!SupportsMemfdCreate) return;
  int fd = open("/zip/life-nomod", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}

TEST(fexecve, ziposAPEHasZipos) {
  if (!SupportsMemfdCreate) return;
  int fd = open("/zip/zipread", O_RDONLY);
  ASSERT_NE(-1, fd);
  SPAWN(fork);
  fexecve(fd, (char *const[]){0}, (char *const[]){0});
  EXITS(42);
  close(fd);
}
