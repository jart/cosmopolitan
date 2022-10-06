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
#include "libc/calls/calls.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/utime.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath fattr", 0));
}

TEST(utimes, test) {
  struct stat st;
  struct timeval tv[2] = {
      {1655455857, 1},  // atim: Fri Jun 17 2022 08:50:57 GMT+0000
      {827727928, 2},   // mtim: Mon Mar 25 1996 04:25:28 GMT+0000
  };
  EXPECT_SYS(0, 0, touch("boop", 0644));
  EXPECT_SYS(0, 0, utimes("boop", tv));
  EXPECT_SYS(0, 0, stat("boop", &st));
  EXPECT_EQ(1655455857, st.st_atim.tv_sec);
  EXPECT_EQ(827727928, st.st_mtim.tv_sec);
  if (IsLinux() && !__is_linux_2_6_23()) {
    // rhel5 only seems to have second granularity
    EXPECT_EQ(0, st.st_atim.tv_nsec);
    EXPECT_EQ(0, st.st_mtim.tv_nsec);
  } else {
    EXPECT_EQ(1000, st.st_atim.tv_nsec);
    EXPECT_EQ(2000, st.st_mtim.tv_nsec);
  }
}

TEST(futimes, test) {
  if (IsLinux() && !__is_linux_2_6_23()) return;
  struct stat st;
  struct timeval tv[2] = {{1655455857, 1}, {827727928, 2}};
  EXPECT_SYS(0, 3, creat("boop", 0644));
  EXPECT_SYS(0, 0, futimes(3, tv));
  EXPECT_SYS(0, 0, fstat(3, &st));
  EXPECT_EQ(1655455857, st.st_atim.tv_sec);
  EXPECT_EQ(827727928, st.st_mtim.tv_sec);
  EXPECT_EQ(1000, st.st_atim.tv_nsec);
  EXPECT_EQ(2000, st.st_mtim.tv_nsec);
  EXPECT_SYS(0, 0, close(3));
}

TEST(futimes, rhel5_enosys) {
  if (IsLinux() && !__is_linux_2_6_23()) {
    struct timeval tv[2] = {{1655455857}, {827727928}};
    EXPECT_SYS(0, 3, creat("boop", 0644));
    EXPECT_SYS(ENOSYS, -1, futimes(3, tv));
    EXPECT_SYS(0, 0, close(3));
  }
}

TEST(utimensat, test) {
  struct stat st;
  struct timespec ts[2] = {
      {1655455857},  // atim: Fri Jun 17 2022 08:50:57 GMT+0000
      {827727928},   // mtim: Mon Mar 25 1996 04:25:28 GMT+0000
  };
  EXPECT_SYS(0, 0, touch("boop", 0644));
  EXPECT_SYS(0, 0, utimensat(AT_FDCWD, "boop", ts, 0));
  EXPECT_SYS(0, 0, stat("boop", &st));
  EXPECT_EQ(1655455857, st.st_atim.tv_sec);
  EXPECT_EQ(827727928, st.st_mtim.tv_sec);
}

TEST(futimens, test) {
  if (IsLinux() && !__is_linux_2_6_23()) {
    // TODO(jart): How does RHEL5 do futimes()?
    return;
  }
  struct stat st;
  struct timespec ts[2] = {
      {1655455857},  // atim: Fri Jun 17 2022 08:50:57 GMT+0000
      {827727928},   // mtim: Mon Mar 25 1996 04:25:28 GMT+0000
  };
  EXPECT_SYS(0, 0, touch("boop", 0644));
  EXPECT_SYS(0, 3, open("boop", O_RDWR));
  EXPECT_SYS(0, 0, futimens(3, ts));
  EXPECT_SYS(0, 0, fstat(3, &st));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_EQ(1655455857, st.st_atim.tv_sec);
  EXPECT_EQ(827727928, st.st_mtim.tv_sec);
}

TEST(utimensat, testOmit) {
  if (IsLinux() && !__is_linux_2_6_23()) {
    // TODO(jart): Ugh.
    return;
  }
  struct stat st;
  struct timespec ts[2] = {
      {123, UTIME_OMIT},
      {123, UTIME_OMIT},
  };
  EXPECT_SYS(0, 0, touch("boop", 0644));
  EXPECT_SYS(0, 0, utimensat(AT_FDCWD, "boop", ts, 0));
  EXPECT_SYS(0, 0, stat("boop", &st));
  EXPECT_NE(123, st.st_atim.tv_sec);
  EXPECT_NE(123, st.st_mtim.tv_sec);
}
