/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/xchg.h"
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/mappings.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

unsigned m1;

TEST(mmap, testMapUnmapAnonAnyAddr) {
  void *p;
  m1 = _mm.i;
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap(p, FRAMESIZE));
  EXPECT_EQ(m1 + 0, _mm.i);
}

TEST(mmap, testMunmapUnmapsMultiple) {
  void *p1, *p2;
  m1 = _mm.i;
  EXPECT_NE(MAP_FAILED, (p1 = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_NE(MAP_FAILED, (p2 = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  if ((intptr_t)p1 > (intptr_t)p2) xchg(&p1, &p2);
  EXPECT_EQ(m1 + 2, _mm.i);
  EXPECT_NE(-1, munmap(p1, (intptr_t)p2 + (intptr_t)FRAMESIZE - (intptr_t)p1));
  EXPECT_EQ(m1 + 0, _mm.i);
}

TEST(mmap, testPartialUnmapRight) {
  if (1) return; /* naaah */
  char *p;
  m1 = _mm.i;
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, FRAMESIZE * 2, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap(p + FRAMESIZE, FRAMESIZE));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap(p, FRAMESIZE));
  EXPECT_EQ(m1 + 0, _mm.i);
}

TEST(mmap, testPartialUnmapLeft) {
  if (1) return; /* naaah */
  char *p;
  m1 = _mm.i;
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, FRAMESIZE * 2, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap(p, FRAMESIZE));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap(p + FRAMESIZE, FRAMESIZE));
  EXPECT_EQ(m1 + 0, _mm.i);
}

TEST(mmap, testMapFile) {
  int fd;
  char *p;
  char path[PATH_MAX];
  sprintf(path, "%s%s.%d", kTmpPath, program_invocation_short_name, getpid());
  m1 = _mm.i;
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_STREQ("hello", p);
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_EQ(m1 + 0, _mm.i);
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, testMapFile_fdGetsClosed_makesNoDifference) {
  int fd;
  char *p, buf[16], path[PATH_MAX];
  sprintf(path, "%s%s.%d", kTmpPath, program_invocation_short_name, getpid());
  m1 = _mm.i;
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  EXPECT_NE(-1, close(fd));
  EXPECT_EQ(m1 + 1, _mm.i);
  EXPECT_STREQ("hello", p);
  p[1] = 'a';
  EXPECT_NE(-1, msync(p, PAGESIZE, MS_SYNC));
  ASSERT_NE(-1, (fd = open(path, O_RDONLY)));
  EXPECT_EQ(5, read(fd, buf, 5));
  EXPECT_STREQN("hallo", buf, 5);
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_EQ(m1 + 0, _mm.i);
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, testMapFixed_destroysEverythingInItsPath) {
  m1 = _mm.i;
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedMappingsStart + FRAMESIZE * 0),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedMappingsStart + FRAMESIZE * 1),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedMappingsStart + FRAMESIZE * 2),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ(m1 + 3, _mm.i);
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedMappingsStart + FRAMESIZE * 0),
                             FRAMESIZE * 3, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ(m1 + 1, _mm.i);
  EXPECT_NE(-1, munmap((void *)kFixedMappingsStart, FRAMESIZE * 3));
}
