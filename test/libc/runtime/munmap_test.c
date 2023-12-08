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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(munmap, doesntExist_doesntCare) {
  EXPECT_SYS(0, 0, munmap(0, FRAMESIZE * 8));
  if (IsAsan()) {
    // make sure it didn't unmap the null pointer shadow memory
    EXPECT_TRUE(testlib_memoryexists((char *)0x7fff8000));
  }
}

TEST(munmap, invalidParams) {
  EXPECT_SYS(EINVAL, -1, munmap(0, 0));
  EXPECT_SYS(EINVAL, -1, munmap((void *)0x100080000000, 0));
  EXPECT_SYS(EINVAL, -1, munmap((void *)0x100080000001, FRAMESIZE));
}

TEST(munmap, test) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_FALSE(testlib_memoryexists(p));
}

TEST(munmap, punchHoleInMemory) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 2));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE, FRAMESIZE));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 2));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE * 2, FRAMESIZE));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 2));
}

TEST(munmap, memoryHasHole) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE, FRAMESIZE));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 2));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE * 3));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 2));
}

TEST(munmap, blanketFree) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 2));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE * 0, FRAMESIZE));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE * 2, FRAMESIZE));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 2));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE * 3));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 2));
}

TEST(munmap, trimLeft) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE * 2, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE * 2));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
}

TEST(munmap, trimRight) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE * 2, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_SYS(0, 0, munmap(p + FRAMESIZE, FRAMESIZE));
  EXPECT_TRUE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE * 2));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 0));
  EXPECT_FALSE(testlib_memoryexists(p + FRAMESIZE * 1));
}

TEST(munmap, memoryGone) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(munmap, testTooSmallToUnmapAsan) {
  if (!IsAsan()) return;
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists((char *)(((intptr_t)p >> 3) + 0x7fff8000)));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_TRUE(testlib_memoryexists((char *)(((intptr_t)p >> 3) + 0x7fff8000)));
}

TEST(munmap, testLargeEnoughToUnmapAsan) {
  if (!IsAsan()) return;
  if (IsWindows()) {
    // we're unfortunately never able to unmap asan pages on windows
    // because the memtrack array items always have to be 64kb so we
    // we're able to store a handle for each
    return;
  }
  char *p;
  size_t n;
  n = FRAMESIZE * 8 * 2;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, n, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_SYS(0, 0, munmap(p, n));
#if 0
  EXPECT_FALSE(
      testlib_memoryexists((char *)(((intptr_t)(p + n / 2) >> 3) + 0x7fff8000)));
#endif
}

TEST(munmap, tinyFile_roundupUnmapSize) {
  char *p;
  ASSERT_SYS(0, 3, open("doge", O_WRONLY | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, write(3, "hello", 5));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("doge", O_RDONLY));
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 5, PROT_READ, MAP_PRIVATE, 3, 0)));
  ASSERT_SYS(0, 0, close(3));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
  EXPECT_FALSE(testlib_memoryexists(p));
  EXPECT_FALSE(testlib_memoryexists(p + 5));
}

TEST(munmap, tinyFile_preciseUnmapSize) {
  char *p, *q;
  ASSERT_SYS(0, 3, open("doge", O_WRONLY | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS(0, 5, write(3, "hello", 5));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, open("doge", O_RDONLY));
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 5, PROT_READ, MAP_PRIVATE, 3, 0)));
  ASSERT_NE(MAP_FAILED, (q = mmap(0, 5, PROT_READ, MAP_PRIVATE, 3, 0)));
  ASSERT_SYS(0, 0, close(3));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_TRUE(testlib_memoryexists(q));
  EXPECT_SYS(0, 0, munmap(p, 5));
  EXPECT_FALSE(testlib_memoryexists(p));
  EXPECT_TRUE(testlib_memoryexists(q));
  EXPECT_SYS(0, 0, munmap(q, 5));
  EXPECT_FALSE(testlib_memoryexists(q));
}

// clang-format off
TEST(munmap, tinyFile_mapThriceUnmapOnce) {
  char *p = (char *)0x000063d646e20000;
  ASSERT_SYS(0, 3, open("doge", O_RDWR | O_CREAT | O_TRUNC, 0644));
  ASSERT_SYS (0, 5, write(3, "hello", 5));
  ASSERT_EQ(p+FRAMESIZE*0, mmap(p+FRAMESIZE*0, FRAMESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0));
  ASSERT_EQ(p+FRAMESIZE*1, mmap(p+FRAMESIZE*1, 5, PROT_READ, MAP_PRIVATE|MAP_FIXED, 3, 0));
  ASSERT_EQ(p+FRAMESIZE*3, mmap(p+FRAMESIZE*3, 5, PROT_READ, MAP_PRIVATE|MAP_FIXED, 3, 0));
  ASSERT_SYS(0, 0, close(3));
  EXPECT_TRUE(testlib_memoryexists(p+FRAMESIZE*0));
  EXPECT_TRUE(testlib_memoryexists(p+FRAMESIZE*1));
  EXPECT_FALSE(testlib_memoryexists(p+FRAMESIZE*2));
  EXPECT_TRUE(testlib_memoryexists(p+FRAMESIZE*3));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE*5));
  EXPECT_FALSE(testlib_memoryexists(p+FRAMESIZE*0));
  EXPECT_FALSE(testlib_memoryexists(p+FRAMESIZE*1));
  EXPECT_FALSE(testlib_memoryexists(p+FRAMESIZE*2));
  EXPECT_FALSE(testlib_memoryexists(p+FRAMESIZE*3));
}
// clang-format on
