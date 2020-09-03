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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/memtrack.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(mmap, testMapFile) {
  int fd;
  char *p;
  char path[PATH_MAX];
  sprintf(path, "%s%s.%d", kTmpPath, program_invocation_short_name, getpid());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQ("hello", p);
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, testMapFile_fdGetsClosed_makesNoDifference) {
  int fd;
  char *p, buf[16], path[PATH_MAX];
  sprintf(path, "%s%s.%d", kTmpPath, program_invocation_short_name, getpid());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  EXPECT_NE(-1, close(fd));
  EXPECT_STREQ("hello", p);
  p[1] = 'a';
  EXPECT_NE(-1, msync(p, PAGESIZE, MS_SYNC));
  ASSERT_NE(-1, (fd = open(path, O_RDONLY)));
  EXPECT_EQ(5, read(fd, buf, 5));
  EXPECT_STREQN("hallo", buf, 5);
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, testMapFixed_destroysEverythingInItsPath) {
  unsigned m1 = _mmi.i;
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedmapStart + FRAMESIZE * 0),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedmapStart + FRAMESIZE * 1),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedmapStart + FRAMESIZE * 2),
                             FRAMESIZE, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  EXPECT_NE(MAP_FAILED, mmap((void *)(kFixedmapStart + FRAMESIZE * 0),
                             FRAMESIZE * 3, PROT_READ | PROT_WRITE,
                             MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_GT(_mmi.i, m1);
  EXPECT_NE(-1, munmap((void *)kFixedmapStart, FRAMESIZE * 3));
#ifdef __SANITIZE_ADDRESS__
  ASSERT_EQ(m1 + 1, _mmi.i);
#else
  ASSERT_EQ(m1, _mmi.i);
#endif
}

TEST(isheap, nullPtr) {
  ASSERT_FALSE(isheap(NULL));
}

TEST(isheap, malloc) {
  ASSERT_TRUE(isheap(gc(malloc(1))));
}

TEST(isheap, emptyMalloc) {
  ASSERT_TRUE(isheap(gc(malloc(0))));
}

TEST(isheap, mallocOffset) {
  ASSERT_TRUE(isheap((char *)gc(malloc(131072)) + 100000));
}
