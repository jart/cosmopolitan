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
#include "ape/sections.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/xchg.internal.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xspawn.h"
#include "third_party/xed/x86.h"

__static_yoink("zipos");

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  // ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath proc", 0));
}

TEST(mmap, zeroSize) {
  ASSERT_SYS(EINVAL, MAP_FAILED,
             mmap(NULL, 0, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}

TEST(mmap, overflow) {
  ASSERT_SYS(EINVAL, MAP_FAILED,
             mmap(NULL, 0x800000000000, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE,
                  -1, 0));
  ASSERT_SYS(EINVAL, MAP_FAILED,
             mmap(NULL, 0x7fffffffffff, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE,
                  -1, 0));
}

TEST(mmap, outOfAutomapRange) {
  ASSERT_SYS(
      ENOMEM, MAP_FAILED,
      mmap(NULL, kAutomapSize, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}

TEST(mmap, noreplaceImage) {
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(__executable_start, FRAMESIZE, PROT_READ,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0));
}

TEST(mmap, noreplaceExistingMap) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, FRAMESIZE, PROT_READ,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(p, FRAMESIZE, PROT_READ,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0));
  EXPECT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(mmap, smallerThanPage_mapsRemainder) {
  long pagesz = sysconf(_SC_PAGESIZE);
  char *map =
      mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NE(MAP_FAILED, map);
  EXPECT_TRUE(testlib_memoryexists(map));
  EXPECT_TRUE(testlib_memoryexists(map + (pagesz - 1)));
  EXPECT_SYS(0, 0, munmap(map, 1));
  EXPECT_FALSE(testlib_memoryexists(map));
  EXPECT_FALSE(testlib_memoryexists(map + (pagesz - 1)));
}

TEST(mmap, smallerThanPage_remainderIsPoisoned) {
  if (!IsAsan()) return;
  char *map;
  ASSERT_NE(MAP_FAILED, (map = mmap(0, 1, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_TRUE(__asan_is_valid(map, 1));
  EXPECT_FALSE(__asan_is_valid(map + 1, 1));
  EXPECT_SYS(0, 0, munmap(map, 1));
}

TEST(mmap, testMapFile) {
  int fd;
  char *p;
  char path[PATH_MAX];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("hello", p, 5);
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, testMapFile_fdGetsClosed_makesNoDifference) {
  int fd;
  char *p, buf[16], path[PATH_MAX];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  EXPECT_NE(-1, close(fd));
  EXPECT_STREQN("hello", p, 5);
  p[1] = 'a';
  EXPECT_NE(-1, msync(p, getauxval(AT_PAGESZ), MS_SYNC));
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
}

#ifdef __x86_64__
TEST(mmap, customStackMemory_isAuthorized) {
  char *stack;
  uintptr_t w, r;
  ASSERT_NE(MAP_FAILED,
            (stack = mmap(NULL, GetStackSize(), PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_STACK, -1, 0)));
  asm("mov\t%%rsp,%0\n\t"
      "mov\t%2,%%rsp\n\t"
      "push\t%3\n\t"
      "pop\t%1\n\t"
      "mov\t%0,%%rsp"
      : "=&r"(w), "=&r"(r)
      : "rm"(stack + GetStackSize() - 8), "i"(123));
  ASSERT_EQ(123, r);
  EXPECT_SYS(0, 0, munmap(stack, GetStackSize()));
}
#endif /* __x86_64__ */

TEST(mmap, fileOffset) {
  int fd;
  char *map;
  int offset_align = IsWindows() ? FRAMESIZE : getauxval(AT_PAGESZ);
  ASSERT_NE(-1, (fd = open("foo", O_CREAT | O_RDWR, 0644)));
  EXPECT_NE(-1, ftruncate(fd, offset_align * 2));
  EXPECT_NE(-1, pwrite(fd, "hello", 5, offset_align * 0));
  EXPECT_NE(-1, pwrite(fd, "there", 5, offset_align * 1));
  EXPECT_NE(-1, fdatasync(fd));
  ASSERT_NE(MAP_FAILED, (map = mmap(NULL, offset_align, PROT_READ, MAP_PRIVATE,
                                    fd, offset_align)));
  EXPECT_EQ(0, memcmp(map, "there", 5), "%#.*s", 5, map);
  EXPECT_NE(-1, munmap(map, offset_align));
  EXPECT_NE(-1, close(fd));
}

TEST(mmap, mapPrivate_writesDontChangeFile) {
  int fd;
  char *map, buf[6];
  ASSERT_NE(-1, (fd = open("bar", O_CREAT | O_RDWR, 0644)));
  EXPECT_NE(-1, ftruncate(fd, FRAMESIZE));
  EXPECT_NE(-1, pwrite(fd, "hello", 5, 0));
  ASSERT_NE(MAP_FAILED, (map = mmap(NULL, FRAMESIZE, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE, fd, 0)));
  memcpy(map, "there", 5);
  EXPECT_NE(-1, msync(map, FRAMESIZE, MS_SYNC));
  EXPECT_NE(-1, munmap(map, FRAMESIZE));
  EXPECT_NE(-1, pread(fd, buf, 6, 0));
  EXPECT_EQ(0, memcmp(buf, "hello", 5), "%#.*s", 5, buf);
  EXPECT_NE(-1, close(fd));
}

TEST(mmap, twoPowerSize_automapsAddressWithThatAlignment) {
  char *q, *p;
  // increase the likelihood automap is unaligned w.r.t. following call
  ASSERT_NE(MAP_FAILED, (q = mmap(NULL, 0x00010000, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0)));
  // ask for a nice big round size
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 0x00080000, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0)));
  // verify it's aligned
  ASSERT_EQ(0, (intptr_t)p & 0x0007ffff);
  EXPECT_SYS(0, 0, munmap(p, 0x00080000));
}

TEST(isheap, nullPtr) {
  ASSERT_FALSE(_isheap(NULL));
}

TEST(isheap, malloc) {
  ASSERT_TRUE(_isheap(gc(malloc(1))));
}

/* TEST(isheap, emptyMalloc) { */
/*   ASSERT_TRUE(_isheap(gc(malloc(0)))); */
/* } */

/* TEST(isheap, mallocOffset) { */
/*   char *p = gc(malloc(131072)); */
/*   ASSERT_TRUE(_isheap(p + 100000)); */
/* } */

static const char *ziposLifePath = "/zip/life.elf";
TEST(mmap, ziposCannotBeAnonymous) {
  int fd;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_SYS(EINVAL, MAP_FAILED,
             (p = mmap(NULL, 0x00010000, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS,
                       fd, 0)));
  close(fd);
}

TEST(mmap, ziposCannotBeShared) {
  int fd;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_SYS(EINVAL, MAP_FAILED,
             (p = mmap(NULL, 0x00010000, PROT_READ, MAP_SHARED, fd, 0)));
  close(fd);
}

////////////////////////////////////////////////////////////////////////////////
// zipos NON-SHARED READ-ONLY FILE MEMORY

TEST(mmap, ziposCow) {
  int fd;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 0x00010000, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("\177ELF", ((const char *)p), 4);
  EXPECT_NE(-1, munmap(p, 0x00010000));
  EXPECT_NE(-1, close(fd));
}

////////////////////////////////////////////////////////////////////////////////
// zipos NON-SHARED READ-ONLY FILE MEMORY BETWEEN PROCESSES

TEST(mmap, ziposCowFileMapReadonlyFork) {
  int fd, ws;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 4, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("ELF", ((const char *)p) + 1, 3);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("ELF", ((const char *)p) + 1, 3);
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("ELF", ((const char *)p) + 1, 3);
  EXPECT_NE(-1, munmap(p, 6));
  EXPECT_NE(-1, close(fd));
}

////////////////////////////////////////////////////////////////////////////////
// zipos NON-SHARED READ/WRITE FILE MEMORY BETWEEN PROCESSES

TEST(mmap, ziposCowFileMapFork) {
  int fd, ws;
  void *p;
  char lol[4];
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 6, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)));
  memcpy(p, "parnt", 6);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("parnt", p, 5);
    strcpy(p, "child");
    ASSERT_STREQN("child", p, 5);
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("parnt", p, 5);  // child changing memory did not change parent
  EXPECT_EQ(4, pread(fd, lol, 4, 0));
  EXPECT_STREQN("ELF", &lol[1], 3);  // changing memory did not change file
  EXPECT_NE(-1, munmap(p, 6));
  EXPECT_NE(-1, close(fd));
}

////////////////////////////////////////////////////////////////////////////////
// NON-SHARED READ-ONLY FILE MEMORY

TEST(mmap, cow) {
  int fd;
  char *p;
  char path[PATH_MAX];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)), "%s",
            path);
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("hello", p, 5);
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

////////////////////////////////////////////////////////////////////////////////
// NON-SHARED READ-ONLY FILE MEMORY BETWEEN PROCESSES

TEST(mmap, cowFileMapReadonlyFork) {
  char *p;
  int fd, ws;
  char path[PATH_MAX];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(6, write(fd, "hello", 6));
  EXPECT_NE(-1, close(fd));
  ASSERT_NE(-1, (fd = open(path, O_RDONLY)));
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 6, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("hello", p, 5);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("hello", p, 5);
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("hello", p, 5);
  EXPECT_NE(-1, munmap(p, 6));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

////////////////////////////////////////////////////////////////////////////////
// NON-SHARED READ/WRITE FILE MEMORY BETWEEN PROCESSES

TEST(mmap, cowFileMapFork) {
  char *p;
  int fd, ws;
  char path[PATH_MAX], lol[6];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(6, write(fd, "parnt", 6));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 6, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("parnt", p, 5);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("parnt", p, 5);
    strcpy(p, "child");
    ASSERT_STREQN("child", p, 5);
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("parnt", p, 5);  // child changing memory did not change parent
  EXPECT_EQ(6, pread(fd, lol, 6, 0));
  EXPECT_STREQN("parnt", lol, 5);  // changing memory did not change file
  EXPECT_NE(-1, munmap(p, 6));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

////////////////////////////////////////////////////////////////////////////////
// SHARED ANONYMOUS MEMORY BETWEEN PROCESSES

TEST(mmap, sharedAnonMapFork) {
  int ws;
  char *p;
  EXPECT_NE(MAP_FAILED, (p = mmap(NULL, 6, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0)));
  strcpy(p, "parnt");
  EXPECT_STREQN("parnt", p, 5);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("parnt", p, 5);
    strcpy(p, "child");
    ASSERT_STREQN("child", p, 5);
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("child", p, 5);  // boom
  EXPECT_NE(-1, munmap(p, 5));
}

////////////////////////////////////////////////////////////////////////////////
// SHARED FILE MEMORY BETWEEN PROCESSES

TEST(mmap, sharedFileMapFork) {
  char *p;
  int fd, ws;
  char path[PATH_MAX], lol[6];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(6, write(fd, "parnt", 6));
  EXPECT_NE(-1, fdatasync(fd));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, 6, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  EXPECT_STREQN("parnt", p, 5);
  ASSERT_NE(-1, (ws = xspawn(0)));
  if (ws == -2) {
    ASSERT_STREQN("parnt", p, 5);
    strcpy(p, "child");
    ASSERT_STREQN("child", p, 5);
    ASSERT_NE(-1, msync(p, 6, MS_SYNC | MS_INVALIDATE));
    _exit(0);
  }
  EXPECT_EQ(0, ws);
  EXPECT_STREQN("child", p, 5);  // child changing memory changed parent memory
  // XXX: RHEL5 has a weird issue where if we read the file into its own
  //      shared memory then corruption occurs!
  EXPECT_EQ(6, pread(fd, lol, 6, 0));
  EXPECT_STREQN("child", lol, 5);  // changing memory changed file
  EXPECT_NE(-1, munmap(p, 6));
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, unlink(path));
}

////////////////////////////////////////////////////////////////////////////////
// BENCHMARKS

#define N (EZBENCH_COUNT * EZBENCH_TRIES)

int count;
void *ptrs[N];

void BenchUnmap(void) {
  ASSERT_EQ(0, munmap(ptrs[count++], FRAMESIZE));
}

void BenchMmapPrivate(void) {
  void *p;
  p = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE,
           -1, 0);
  if (p == MAP_FAILED) abort();
  ptrs[count++] = p;
}

BENCH(mmap, bench) {
  EZBENCH2("mmap", donothing, BenchMmapPrivate());
  EZBENCH2("munmap", donothing, BenchUnmap());
}
