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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/posix.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void Make(const char *path, int mode) {
  int fd, n = lemur64() & 0xfffff;
  char *data = gc(malloc(n));
  rngset(data, n, lemur64, -1);
  ASSERT_NE(-1, (fd = creat(path, mode)));
  ASSERT_SYS(0, n, write(fd, data, n));
  ASSERT_SYS(0, 0, close(fd));
}

void Copy(const char *from, const char *to) {
  ssize_t rc;
  char buf[512];
  struct stat st;
  int fdin, fdout, e = errno;
  ASSERT_NE(-1, (fdin = open(from, O_RDONLY)));
  ASSERT_SYS(0, 0, fstat(fdin, &st));
  ASSERT_NE(-1, (fdout = creat(to, st.st_mode | 0400)));
  if (!(IsXnu() || IsOpenbsd())) {
    ASSERT_SYS(0, 0, posix_fadvise(fdin, 0, st.st_size, POSIX_FADV_SEQUENTIAL));
  }
  ASSERT_SYS(0, 0, ftruncate(fdout, st.st_size));
  while ((rc = copy_file_range(fdin, 0, fdout, 0, -1u, 0))) {
    if (rc == -1) {
      ASSERT_EQ(ENOSYS, errno);
      errno = e;
      while ((rc = read(fdin, buf, sizeof(buf)))) {
        ASSERT_NE(-1, rc);
        ASSERT_EQ(rc, write(fdout, buf, rc));
      }
      break;
    }
  }
  ASSERT_SYS(0, 0, close(fdin));
  ASSERT_SYS(0, 0, close(fdout));
}

TEST(copy_file_range, test) {
  char *p, *q;
  size_t n, m;
  Make("foo", 0644);
  Copy("foo", "bar");
  p = gc(xslurp("foo", &n));
  q = gc(xslurp("bar", &m));
  ASSERT_EQ(n, m);
  ASSERT_EQ(0, memcmp(p, q, n));
}

bool HasCopyFileRange(void) {
  bool ok;
  int rc, e;
  e = errno;
  rc = copy_file_range(-1, 0, -1, 0, 0, 0);
  ok = rc == -1 && errno == EBADF;
  errno = e;
  return ok;
}

TEST(copy_file_range, badFd) {
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(EBADF, -1, copy_file_range(-1, 0, -1, 0, -1u, 0));
}

TEST(copy_file_range, badFlags) {
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(EINVAL, -1, copy_file_range(0, 0, 1, 0, -1u, -1));
}

TEST(copy_file_range, differentFileSystems) {
  return;  // TODO(jart): Why does this flake on GitHub Actions?
  if (!IsLinux()) return;
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(0, 3, open("/proc/stat", 0));
  ASSERT_SYS(0, 4, creat("foo", 0644));
  ASSERT_SYS(EXDEV, -1, copy_file_range(3, 0, 4, 0, -1u, 0));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(copy_file_range, twoDifferentFiles) {
  char buf[16] = {0};
  int64_t i = 1, o = 0;
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 4, open("bar", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, pwrite(3, "hello", 5, 0));
  ASSERT_SYS(0, 4, copy_file_range(3, &i, 4, &o, 4, 0));
  ASSERT_SYS(0, 0, copy_file_range(3, &i, 4, &o, 0, 0));
  ASSERT_EQ(5, i);
  ASSERT_EQ(4, o);
  ASSERT_SYS(0, 4, read(4, buf, 16));
  ASSERT_STREQ("ello", buf);
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(copy_file_range, sameFile_doesntChangeFilePointer) {
  char buf[16] = {0};
  int64_t i = 1, o = 5;
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, pwrite(3, "hello", 5, 0));
  ASSERT_SYS(0, 4, copy_file_range(3, &i, 3, &o, 4, 0));
  ASSERT_EQ(5, i);
  ASSERT_EQ(9, o);
  ASSERT_SYS(0, 9, read(3, buf, 16));
  ASSERT_STREQ("helloello", buf);
  ASSERT_SYS(0, 0, close(3));
}

TEST(copy_file_range, overlappingRange) {
  int rc;
  int64_t i = 1, o = 2;
  if (!HasCopyFileRange()) return;
  ASSERT_SYS(0, 3, open("foo", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, pwrite(3, "hello", 5, 0));
  rc = copy_file_range(3, &i, 3, &o, 4, 0);
  ASSERT_EQ(-1, rc);
  if (IsLinux()) {
    // [wut] rhel7 returns enosys here
    ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
  } else {
    ASSERT_TRUE(errno == EINVAL);
  }
  errno = 0;
  ASSERT_SYS(0, 0, close(3));
}
