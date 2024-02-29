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
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p, FRAMESIZE, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(madvise, file) {
  char *p;
  ASSERT_SYS(0, 3, creat("foo.dat", 0644));
  ASSERT_SYS(0, 0, ftruncate(3, FRAMESIZE));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("foo.dat", O_RDWR));
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE, 3, 0)));
  ASSERT_SYS(0, 0, madvise(p, FRAMESIZE, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
  ASSERT_SYS(0, 0, close(3));
}

TEST(madvise, short) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0, madvise(p, FRAMESIZE - 1, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(madvise, subPages) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(0, 0,
             madvise(p + getauxval(AT_PAGESZ), FRAMESIZE - getauxval(AT_PAGESZ),
                     MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(madvise, misalign) {
  char *p;
  if (!IsLinux()) return;    // most platforms don't care
  if (IsQemuUser()) return;  // qemu claims to be linux but doesn't care
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(EINVAL, -1, madvise(p + 1, FRAMESIZE - 1, MADV_WILLNEED));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(madvise, badAdvice) {
  char *p;
  if (IsAarch64() && IsQemuUser()) return;  // qemu doesn't validate advice
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_SYS(EINVAL, -1, madvise(p, FRAMESIZE, 127));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(madvise, missingMemory) {
  if (!IsLinux()) return;    // most platforms don't care
  if (IsQemuUser()) return;  // qemu claims to be linux but doesn't care
  ASSERT_SYS(ENOMEM, -1,
             madvise((char *)0x83483838000, FRAMESIZE, MADV_WILLNEED));
}
