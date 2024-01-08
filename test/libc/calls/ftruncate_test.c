/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

struct stat st;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath", 0));
}

TEST(ftruncate, ebadf) {
  ASSERT_SYS(EBADF, -1, ftruncate(-1, 0));
  ASSERT_SYS(EBADF, -1, ftruncate(+3, 0));
}

TEST(ftruncate, negativeLength_einval) {
  ASSERT_SYS(0, 3, creat("foo", 0755));
  ASSERT_SYS(EINVAL, -1, ftruncate(3, -1));
  ASSERT_SYS(0, 0, close(3));
}

TEST(ftruncate, doesntHaveWritePermission_einval) {
  ASSERT_SYS(0, 3, creat("foo", 0755));
  ASSERT_SYS(0, 5, write(3, "hello", 5));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("foo", O_RDONLY));
  ASSERT_SYS(EINVAL, -1, ftruncate(3, 0));
  ASSERT_SYS(0, 0, close(3));
}

TEST(ftruncate, pipeFd_einval) {
  int fds[2];
  ASSERT_SYS(0, 0, pipe(fds));
  ASSERT_SYS(EINVAL, -1, ftruncate(3, 0));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
}

TEST(ftruncate, efbig) {
  // FreeBSD and RHEL7 return 0 (why??)
  if (IsLinux() || IsFreebsd()) return;
  sighandler_t old = signal(SIGXFSZ, SIG_IGN);
  ASSERT_SYS(0, 3, creat("foo", 0755));
  ASSERT_SYS(IsWindows() ? EINVAL : EFBIG, -1, ftruncate(3, INT64_MAX));
  ASSERT_SYS(0, 0, close(3));
  signal(SIGXFSZ, old);
}

TEST(ftruncate, test) {
  char got[512], want[512] = "helloworld";  // tests zero-extending
  ASSERT_SYS(0, 3, open("foo", O_CREAT | O_TRUNC | O_RDWR, 0755));
  ASSERT_SYS(0, 5, write(3, "hello", 5));
  ASSERT_SYS(0, 0, ftruncate(3, 8192));
  ASSERT_SYS(0, 5, lseek(3, 0, SEEK_CUR));  // doesn't change position
  ASSERT_SYS(0, 5, write(3, "world", 5));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_EQ(8192, st.st_size);
#if 0
  if (!IsGenuineBlink()) {
    if (IsWindows() || IsNetbsd() || IsOpenbsd()) {  //
      ASSERT_EQ(8192 / 512, st.st_blocks);           // 8192 is physical size
    } else if (IsFreebsd()) {                        //
      ASSERT_EQ(512 / 512, st.st_blocks);            // 512 is physical size
    } else if (IsLinux() || IsXnu()) {               //
      ASSERT_EQ(4096 / 512, st.st_blocks);           // 4096 is physical size
    } else {
      notpossible;
    }
  }
#endif
  ASSERT_SYS(0, 512, pread(3, got, 512, 0));
  ASSERT_EQ(0, memcmp(want, got, 512));
  ASSERT_SYS(0, 0, ftruncate(3, 0));  // shrink file to be empty
  ASSERT_SYS(0, 0, pread(3, got, 512, 0));
  ASSERT_SYS(0, 0, read(3, got, 512));
  ASSERT_SYS(0, 10, lseek(3, 0, SEEK_CUR));  // position stays past eof
  ASSERT_SYS(0, 0, close(3));
}

TEST(ftruncate, isConsistentWithLseek) {
  ASSERT_SYS(0, 3, creat("foo", 0666));
  ASSERT_SYS(0, 0, lseek(3, 0, SEEK_END));
  ASSERT_SYS(0, 0, ftruncate(3, 10));
  ASSERT_SYS(0, 10, lseek(3, 0, SEEK_END));
  ASSERT_SYS(0, 0, close(3));
}

TEST(ftruncate, isConsistentWithFstat) {
  struct stat st;
  ASSERT_SYS(0, 3, creat("foo", 0666));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_EQ(0, st.st_size);
  ASSERT_SYS(0, 0, ftruncate(3, 10));
  ASSERT_SYS(0, 0, fstat(3, &st));
  ASSERT_EQ(10, st.st_size);
  ASSERT_SYS(0, 0, close(3));
}
