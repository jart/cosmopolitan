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

int granularity;

void SetUpOnce(void) {
  granularity = __granularity();
  testlib_enable_tmp_setup_teardown();
}

TEST(munmap, doesntExist_doesntCare) {
  EXPECT_SYS(0, 0, munmap(0, granularity * 8));
}

TEST(munmap, invalidParams) {
  EXPECT_SYS(EINVAL, -1, munmap(0, 0));
  EXPECT_SYS(EINVAL, -1, munmap((void *)0x100080000000, 0));
  EXPECT_SYS(EINVAL, -1, munmap((void *)0x100080000001, granularity));
}

TEST(munmap, test) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_SYS(0, 0, munmap(p, granularity));
  EXPECT_FALSE(testlib_memoryexists(p));
}

TEST(munmap, punchHoleInMemory) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 2));
  EXPECT_SYS(0, 0, munmap(p + granularity, granularity));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 2));
  EXPECT_SYS(0, 0, munmap(p, granularity));
  EXPECT_SYS(0, 0, munmap(p + granularity * 2, granularity));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 2));
}

TEST(munmap, memoryHasHole) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_SYS(0, 0, munmap(p + granularity, granularity));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 2));
  EXPECT_SYS(0, 0, munmap(p, granularity * 3));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 2));
}

TEST(munmap, blanketFree) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity * 3, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 2));
  EXPECT_SYS(0, 0, munmap(p + granularity * 0, granularity));
  EXPECT_SYS(0, 0, munmap(p + granularity * 2, granularity));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 2));
  EXPECT_SYS(0, 0, munmap(p, granularity * 3));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 2));
}

TEST(munmap, trimLeft) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity * 2, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_SYS(0, 0, munmap(p, granularity));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_SYS(0, 0, munmap(p, granularity * 2));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
}

TEST(munmap, trimRight) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity * 2, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 1));
  EXPECT_SYS(0, 0, munmap(p + granularity, granularity));
  EXPECT_TRUE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
  EXPECT_SYS(0, 0, munmap(p, granularity * 2));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 0));
  EXPECT_FALSE(testlib_memoryexists(p + granularity * 1));
}

TEST(munmap, memoryGone) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, granularity, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  EXPECT_SYS(0, 0, munmap(p, granularity));
  EXPECT_SYS(0, 0, munmap(p, granularity));
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
  EXPECT_SYS(0, 0, munmap(p, granularity));
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
/* TEST(munmap, tinyFile_mapThriceUnmapOnce) { */
/*   char *p = (char *)0x000063d646e20000; */
/*   ASSERT_SYS(0, 3, open("doge", O_RDWR | O_CREAT | O_TRUNC, 0644)); */
/*   ASSERT_SYS (0, 5, write(3, "hello", 5)); */
/*   ASSERT_EQ(p+granularity*0, mmap(p+granularity*0, granularity, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0)); */
/*   ASSERT_EQ(p+granularity*1, mmap(p+granularity*1, 5, PROT_READ, MAP_PRIVATE|MAP_FIXED, 3, 0)); */
/*   ASSERT_EQ(p+granularity*3, mmap(p+granularity*3, 5, PROT_READ, MAP_PRIVATE|MAP_FIXED, 3, 0)); */
/*   ASSERT_SYS(0, 0, close(3)); */
/*   EXPECT_TRUE(testlib_memoryexists(p+granularity*0)); */
/*   EXPECT_TRUE(testlib_memoryexists(p+granularity*1)); */
/*   EXPECT_FALSE(testlib_memoryexists(p+granularity*2)); */
/*   EXPECT_TRUE(testlib_memoryexists(p+granularity*3)); */
/*   EXPECT_SYS(0, 0, munmap(p, granularity*5)); */
/*   EXPECT_FALSE(testlib_memoryexists(p+granularity*0)); */
/*   EXPECT_FALSE(testlib_memoryexists(p+granularity*1)); */
/*   EXPECT_FALSE(testlib_memoryexists(p+granularity*2)); */
/*   EXPECT_FALSE(testlib_memoryexists(p+granularity*3)); */
/* } */
// clang-format on
