/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(madvise, anon) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p, getpagesize(), MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
}

TEST(madvise, file) {
  char *p;
  ASSERT_SYS(0, 3, creat("foo.dat", 0644));
  ASSERT_SYS(0, 0, ftruncate(3, getpagesize()));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("foo.dat", O_RDWR));
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE, 3, 0)));
  ASSERT_SYS(0, 0, madvise(p, getpagesize(), MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
  ASSERT_SYS(0, 0, close(3));
}

TEST(madvise, short) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p, getpagesize() - 1, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
}

TEST(madvise, zero) {
  if (IsQemuUser())
    return;  // doesn't validate flags
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p, 0, MADV_WILLNEED));
  ASSERT_SYS(0, 0, madvise(0, 0, MADV_WILLNEED));
  ASSERT_SYS(EINVAL, -1, madvise(p, 0, 666));
  ASSERT_SYS(EINVAL, -1, madvise(0, 0, 666));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
}

TEST(madvise, subPages) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize() * 2, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p + getpagesize(), getpagesize(), MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getpagesize() * 2));
}

TEST(madvise, madvWillNeed_unmappedRegion) {
  if (IsWindows())
    return;  // needs carving
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getgransize() * 3, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, munmap(p + getgransize(), getgransize()));
  if (IsXnu())
    ASSERT_SYS(EINVAL, -1, madvise(p, getgransize() * 3, MADV_WILLNEED));
  else if (IsBsd() || IsWindows() || IsQemuUser())
    ASSERT_SYS(0, 0, madvise(p, getgransize() * 3, MADV_WILLNEED));
  else
    ASSERT_SYS(ENOMEM, -1, madvise(p, getgransize() * 3, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getgransize() * 3));
}

TEST(madvise, madvFree_unmappedRegion) {
  if (IsWindows())
    return;  // needs carving
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getgransize() * 3, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, munmap(p + getgransize(), getgransize()));
  if (IsXnu())
    ASSERT_SYS(EINVAL, -1, madvise(p, getgransize() * 3, MADV_FREE));
  else if (IsNetbsd() || IsOpenbsd())
    ASSERT_SYS(EFAULT, -1, madvise(p, getgransize() * 3, MADV_FREE));
  else if (IsFreebsd() || IsWindows() || IsQemuUser())
    ASSERT_SYS(0, 0, madvise(p, getgransize() * 3, MADV_FREE));
  else
    ASSERT_SYS(ENOMEM, -1, madvise(p, getgransize() * 3, MADV_FREE));
  ASSERT_SYS(0, 0, munmap(p, getgransize() * 3));
}

TEST(madvise, misalign) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(EINVAL, -1, madvise(p + 1, getpagesize(), MADV_WILLNEED));
  ASSERT_SYS(0, 0, madvise(p, 1, MADV_WILLNEED));
  ASSERT_SYS(EINVAL, -1, madvise(p, -1, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
}

TEST(madvise, badAdvice) {
  char *p;
  if (IsAarch64() && IsQemuUser())
    return;  // qemu doesn't validate advice
  ASSERT_NE(MAP_FAILED, (p = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(EINVAL, -1, madvise(p, getpagesize(), 127));
  ASSERT_SYS(0, 0, munmap(p, getpagesize()));
}

TEST(madvise, missingMemory) {
  if (!IsLinux())
    return;  // most platforms don't care
  if (IsQemuUser())
    return;  // qemu claims to be linux but doesn't care
  ASSERT_SYS(ENOMEM, -1,
             madvise((char *)0x83483838000, getpagesize(), MADV_WILLNEED));
}
