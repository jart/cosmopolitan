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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/maps.h"
#include "libc/limits.h"
#include "libc/literal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/msync.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/benchmark.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xspawn.h"

// this is also a good torture test for mmap
//
//     make -j o//test/libc/intrin/pthread_mutex_lock2_test
//     for i in $(seq 100); do
//       o//test/libc/intrin/pthread_mutex_lock2_test &
//     done
//

__static_yoink("zipos");

int pagesz;
int gransz;

void SetUpOnce(void) {
  pagesz = getpagesize();
  gransz = getgransize();
  testlib_enable_tmp_setup_teardown();
  // ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath proc", 0));
}

void TearDown(void) {
  ASSERT_FALSE(__maps_held());
}

TEST(mmap, zeroSize) {
  ASSERT_SYS(EINVAL, MAP_FAILED,
             mmap(NULL, 0, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}

TEST(mmap, overflow) {
  ASSERT_SYS(ENOMEM, MAP_FAILED,
             mmap(NULL, 0x800000000000, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE,
                  -1, 0));
  ASSERT_SYS(ENOMEM, MAP_FAILED,
             mmap(NULL, 0x7fffffffffff, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE,
                  -1, 0));
}

TEST(mmap, noreplaceImage) {
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(__executable_start, 1, PROT_READ,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0));
}

TEST(mmap, noreplaceExistingMap) {
  char *p;
  ASSERT_NE(MAP_FAILED,
            (p = mmap(0, 1, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(p, 1, PROT_READ,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0));
  EXPECT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, pageBeyondGone) {
  int pagesz = getpagesize();
  char *p = mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  ASSERT_EQ(0, munmap(p, pagesz * 2));
  p = mmap(p, 1, PROT_READ | PROT_WRITE,
           MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  ASSERT_NE(MAP_FAILED, p);
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_TRUE(testlib_memoryexists(p + 1));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz));
  ASSERT_EQ(0, munmap(p, 1));
}

TEST(mmap, fixedTaken) {
  char *p;
  ASSERT_NE(MAP_FAILED,
            (p = mmap(0, 1, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  ASSERT_NE(MAP_FAILED, mmap(p, 1, PROT_READ,
                             MAP_FIXED | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
  EXPECT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, anon_rw_to_rx) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 1, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_READ | PROT_EXEC));
  ASSERT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, anon_rw_fork_to_rx) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap(0, 1, PROT_READ | PROT_WRITE,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  SPAWN(fork);
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_READ | PROT_EXEC));
  EXITS(0);
  ASSERT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, anon_r_to_rw) {
  char *p;
  ASSERT_NE(MAP_FAILED,
            (p = mmap(0, 1, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_READ | PROT_WRITE));
  ASSERT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, anon_r_fork_to_rw) {
  char *p;
  ASSERT_NE(MAP_FAILED,
            (p = mmap(0, 1, PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  SPAWN(fork);
  ASSERT_SYS(0, 0, mprotect(p, 1, PROT_READ | PROT_WRITE));
  EXITS(0);
  ASSERT_SYS(0, 0, munmap(p, 1));
}

TEST(mmap, hint) {
  char *p;

  if (IsWindows())
    return;  // needs carving

  // obtain four pages
  ASSERT_NE(MAP_FAILED, (p = mmap(__maps_randaddr(), pagesz * 4, PROT_READ,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));

  // unmap two of those pages
  EXPECT_SYS(0, 0, munmap(p + pagesz, pagesz));
  EXPECT_SYS(0, 0, munmap(p + pagesz * 3, pagesz));

  // test AVAILABLE nonfixed nonzero addr is granted
  // - posix doesn't mandate this behavior (but should)
  // - freebsd always chooses for you (which has no acceptable workaround)
  // - netbsd manual claims it'll be like freebsd, but is actually like openbsd
  if (!IsFreebsd())
    ASSERT_EQ(p + pagesz, mmap(p + pagesz, pagesz, PROT_READ,
                               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

  // test UNAVAILABLE nonfixed nonzero addr picks something nearby
  // - posix actually does require this, but doesn't say how close
  // - xnu / linux / openbsd always choose nearest on the right
  // - freebsd goes about 16mb to the right
  // - qemu-user is off the wall
  /*if (!IsQemuUser()) {
    q = mmap(p + pagesz * 2, pagesz, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1,
             0);
    EXPECT_LE(ABS(q - (p + pagesz * 2)), 128 * 1024 * 1024);
    EXPECT_SYS(0, 0, munmap(q, pagesz));
  }*/

  // clean up
  EXPECT_SYS(0, 0, munmap(p, pagesz * 4));
}

TEST(mprotect, punchHoleAndFillHole) {
  char *p;
  int count = __maps.count;

  if (IsWindows())
    return;  // needs carving

  // obtain memory
  ASSERT_NE(MAP_FAILED,
            (p = mmap(__maps_randaddr(), gransz * 3, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  ASSERT_EQ((count += IsWindows() ? 3 : 1), __maps.count);

  // if munmap punches a hole...
  // the number of mappings may increase
  // this is why it's possible for munmap() to ENOMEM lool
  EXPECT_SYS(0, 0, munmap(p + gransz, gransz));
  ASSERT_EQ((count += IsWindows() ? -1 : +1), __maps.count);

  // now if we fill that hole
  // the memory manager will coalesce the mappings
  ASSERT_NE(MAP_FAILED,
            (p = mmap(p + gransz, gransz, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0)));
  ASSERT_EQ((count += IsWindows() ? +1 : -1), __maps.count);

  // clean up
  EXPECT_SYS(0, 0, munmap(p, gransz * 3));
}

TEST(mmap, smallerThanPage_mapsRemainder) {
  long pagesz = sysconf(_SC_PAGESIZE);
  char *map =
      mmap(0, 1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  ASSERT_NE(MAP_FAILED, map);
  EXPECT_TRUE(testlib_memoryexists(map));
  EXPECT_TRUE(testlib_pokememory(map + (pagesz - 1)));
  EXPECT_TRUE(testlib_memoryexists(map + (pagesz - 1)));
  EXPECT_SYS(0, 0, munmap(map, 1));
  EXPECT_FALSE(testlib_memoryexists(map));
  EXPECT_FALSE(testlib_memoryexists(map + (pagesz - 1)));
}

TEST(mmap, testMapFile) {
  int fd;
  char *p;
  char path[PATH_MAX];
  sprintf(path, "%s.%ld", program_invocation_short_name, lemur64());
  ASSERT_NE(-1, (fd = open(path, O_CREAT | O_TRUNC | O_RDWR, 0644)));
  EXPECT_EQ(5, write(fd, "hello", 5));
  EXPECT_NE(-1, fdatasync(fd));
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
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
  ASSERT_NE(MAP_FAILED,
            (p = mmap(NULL, 5, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)));
  EXPECT_NE(-1, close(fd));
  EXPECT_STREQN("hello", p, 5);
  p[1] = 'a';
  EXPECT_NE(-1, msync(p, getpagesize(), MS_SYNC));
  ASSERT_NE(-1, (fd = open(path, O_RDONLY)));
  EXPECT_EQ(5, read(fd, buf, 5));
  EXPECT_STREQN("hallo", buf, 5);
  EXPECT_NE(-1, close(fd));
  EXPECT_NE(-1, munmap(p, 5));
  EXPECT_NE(-1, unlink(path));
}

TEST(mmap, fileOffset) {
  int fd;
  char *map;
  ASSERT_NE(-1, (fd = open("foo", O_CREAT | O_RDWR, 0644)));
  EXPECT_NE(-1, ftruncate(fd, gransz * 2));
  EXPECT_NE(-1, pwrite(fd, "hello", 5, gransz * 0));
  EXPECT_NE(-1, pwrite(fd, "there", 5, gransz * 1));
  EXPECT_NE(-1, fdatasync(fd));
  ASSERT_SYS(EINVAL, MAP_FAILED,
             mmap(NULL, gransz, PROT_READ, MAP_PRIVATE, fd, gransz - 1));
  ASSERT_NE(MAP_FAILED,
            (map = mmap(NULL, gransz, PROT_READ, MAP_PRIVATE, fd, gransz)));
  EXPECT_EQ(0, memcmp(map, "there", 5), "%#.*s", 5, map);
  EXPECT_NE(-1, munmap(map, gransz));
  EXPECT_NE(-1, close(fd));
}

TEST(mmap, mapPrivate_writesDontChangeFile) {
  int fd;
  char *map, buf[6];
  ASSERT_NE(-1, (fd = open("bar", O_CREAT | O_RDWR, 0644)));
  EXPECT_NE(-1, ftruncate(fd, gransz));
  EXPECT_NE(-1, pwrite(fd, "hello", 5, 0));
  ASSERT_NE(MAP_FAILED, (map = mmap(NULL, gransz, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE, fd, 0)));
  memcpy(map, "there", 5);
  EXPECT_NE(-1, msync(map, gransz, MS_SYNC));
  EXPECT_NE(-1, munmap(map, gransz));
  EXPECT_NE(-1, pread(fd, buf, 6, 0));
  EXPECT_EQ(0, memcmp(buf, "hello", 5), "%#.*s", 5, buf);
  EXPECT_NE(-1, close(fd));
}

static const char *ziposLifePath = "/zip/life.elf";

TEST(mmap, ziposCannotBeShared) {
  int fd;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_SYS(EINVAL, MAP_FAILED,
             (p = mmap(NULL, gransz, PROT_READ, MAP_SHARED, fd, 0)));
  close(fd);
}

TEST(mmap, misalignedAddr_justIgnoresIt) {
  char *p;
  ASSERT_NE(MAP_FAILED, (p = mmap((void *)1, 1, PROT_READ,
                                  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)));
  munmap(p, 1);
}

TEST(mmap, nullFixed_isNotAllowed) {
  ASSERT_SYS(
      EPERM, MAP_FAILED,
      mmap(0, 1, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0));
  ASSERT_SYS(EPERM, MAP_FAILED,
             mmap(0, 1, PROT_NONE,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE, -1, 0));
}

static int GetBitsInAddressSpace(void) {
  int i;
  void *ptr;
  uint64_t want;
  for (i = 0; i < 40; ++i) {
    want = UINT64_C(0x8123000000000000) >> i;
    if (want > UINTPTR_MAX)
      continue;
    if (msync((void *)(uintptr_t)want, 1, MS_ASYNC) == 0 || errno == EBUSY)
      return 64 - i;
    ptr = mmap((void *)(uintptr_t)want, 1, PROT_READ,
               MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
    if (ptr != MAP_FAILED) {
      munmap(ptr, 1);
      return 64 - i;
    }
  }
  abort();
}

TEST(mmap, negative_isNotAllowed) {
  ASSERT_SYS(ENOMEM, MAP_FAILED,
             mmap((void *)-(70 * 1024 * 1024), 1, PROT_NONE,
                  MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0));
}

TEST(mmap, pml5t) {
  switch (GetBitsInAddressSpace()) {
    case 56:
      ASSERT_EQ((void *)0x00fff2749119c000,
                mmap((void *)0x00fff2749119c000, 1, PROT_NONE,
                     MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0));
      munmap((void *)0x00fff2749119c000, 1);
      break;
    case 48:
      ASSERT_EQ((void *)0x0000f2749119c000,
                mmap((void *)0x0000f2749119c000, 1, PROT_NONE,
                     MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0));
      munmap((void *)0x0000f2749119c000, 1);
      break;
    default:
      break;
  }
}

TEST(mmap, windows) {
  if (!IsWindows())
    return;
  int count = __maps.count;
  char *base = __maps_randaddr();

  ASSERT_EQ(base, mmap(base, pagesz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // isn't granularity aligned
  ASSERT_SYS(EINVAL, -1, munmap(base + pagesz, pagesz));

  // doesn't overlap any maps
  ASSERT_SYS(0, 0, munmap(base + gransz, pagesz));
  ASSERT_EQ(count, __maps.count);

  // doesn't overlap any maps
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz));
  ASSERT_EQ(count, __maps.count);

  // partially overlaps map
  ASSERT_SYS(ENOTSUP, -1, munmap(base, pagesz));
  ASSERT_EQ(count, __maps.count);

  // envelops map
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz + pagesz * 4));
  ASSERT_EQ((count -= 1), __maps.count);

  // win32 actually unmapped map
  ASSERT_EQ(base, mmap(base, pagesz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // change status of middle page results in three fragments
  ASSERT_SYS(0, 0, mprotect(base + pagesz, pagesz, PROT_NONE));
  ASSERT_EQ((count += 2), __maps.count);

  // change status back (todo: should reunite fragments)
  ASSERT_SYS(0, 0, mprotect(base + pagesz, pagesz, PROT_READ | PROT_WRITE));
  ASSERT_EQ(count, __maps.count);

  // clean up
  ASSERT_SYS(0, 0, munmap(base, pagesz * 3));
  ASSERT_EQ((count -= 3), __maps.count);
}

TEST(mmap, windows_partial_overlap_enotsup) {
  if (!IsWindows())
    return;
  int count = __maps.count;
  char *base = __maps_randaddr();

  ASSERT_EQ(base, mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // partially overlaps on left
  ASSERT_SYS(ENOTSUP, -1, munmap(base - gransz, gransz * 2));
  ASSERT_SYS(ENOTSUP, -1, munmap(base, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // partially overlaps the middle
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 1, gransz));
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 1, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // partially overlaps on right
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 2, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // doesn't overlap any maps
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz));
  ASSERT_SYS(0, 0, munmap(base + gransz * 3, gransz));
  ASSERT_EQ(count, __maps.count);

  // unmap envelops
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz * 4));
  ASSERT_EQ((count -= 1), __maps.count);

  // win32 actually removed the memory
  ASSERT_EQ(base, mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // clean up
  ASSERT_SYS(0, 0, munmap(base, gransz * 3));
  ASSERT_EQ((count -= 1), __maps.count);
}

TEST(munmap, windows_not_all_fragments_included_enotsup) {
  if (!IsWindows())
    return;
  int count = __maps.count;
  char *base = __maps_randaddr();

  ASSERT_EQ(base, mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // win32 memory actually exists
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                  MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(base + gransz * 0, gransz, PROT_READ | PROT_WRITE,
                  MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(base + gransz * 1, gransz, PROT_READ | PROT_WRITE,
                  MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_SYS(EEXIST, MAP_FAILED,
             mmap(base + gransz * 2, gransz, PROT_READ | PROT_WRITE,
                  MAP_FIXED_NOREPLACE | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

  // change status of middle page results in three fragments
  ASSERT_SYS(0, 0, mprotect(base + gransz, gransz, PROT_NONE));
  ASSERT_EQ((count += 2), __maps.count);

  // partially overlaps on left
  ASSERT_SYS(ENOTSUP, -1, munmap(base - gransz, gransz * 2));
  ASSERT_SYS(ENOTSUP, -1, munmap(base, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // partially overlaps the middle
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 1, gransz));
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 1, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // partially overlaps on right
  ASSERT_SYS(ENOTSUP, -1, munmap(base + gransz * 2, gransz * 2));
  ASSERT_EQ(count, __maps.count);

  // doesn't overlap any maps
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz));
  ASSERT_SYS(0, 0, munmap(base + gransz * 3, gransz));
  ASSERT_EQ(count, __maps.count);

  // unmap envelops
  ASSERT_SYS(0, 0, munmap(base - gransz, gransz * 4));
  ASSERT_EQ((count -= 3), __maps.count);

  // win32 actually removed the memory
  ASSERT_EQ(base, mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_EQ((count += 1), __maps.count);

  // clean up
  ASSERT_SYS(0, 0, munmap(base, gransz * 3));
  ASSERT_EQ((count -= 1), __maps.count);
}

TEST(mmap, windows_private_memory_fork_uses_virtualfree) {
  if (IsFreebsd())
    return;  // freebsd can't take a hint
  char *base;
  ASSERT_NE(MAP_FAILED, (base = mmap(0, gransz * 3, PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  SPAWN(fork);
  ASSERT_SYS(0, 0, munmap(base, gransz * 3));
  ASSERT_EQ(base, mmap(base, gransz * 3, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  ASSERT_SYS(0, 0, munmap(base, gransz * 3));
  EXITS(0);
  ASSERT_SYS(0, 0, munmap(base, gransz * 3));
}

////////////////////////////////////////////////////////////////////////////////
// zipos NON-SHARED READ-ONLY FILE MEMORY

TEST(mmap, ziposCow) {
  int fd;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  EXPECT_NE(MAP_FAILED,
            (p = mmap(NULL, gransz, PROT_READ, MAP_PRIVATE, fd, 0)));
  EXPECT_STREQN("\177ELF", ((const char *)p), 4);
  EXPECT_NE(-1, munmap(p, gransz));
  EXPECT_NE(-1, close(fd));
}

////////////////////////////////////////////////////////////////////////////////
// zipos NON-SHARED READ-ONLY FILE MEMORY BETWEEN PROCESSES

TEST(mmap, ziposCowFileMapReadonlyFork) {
  int fd, ws;
  void *p;
  ASSERT_NE(-1, (fd = open(ziposLifePath, O_RDONLY), "%s", ziposLifePath));
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 4, PROT_READ, MAP_PRIVATE, fd, 0)));
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
  ASSERT_NE(MAP_FAILED,
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
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 5, PROT_READ, MAP_PRIVATE, fd, 0)));
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
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 6, PROT_READ, MAP_PRIVATE, fd, 0)));
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
  ASSERT_NE(MAP_FAILED,
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
  ASSERT_NE(MAP_FAILED, (p = mmap(NULL, 6, PROT_READ | PROT_WRITE,
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
  ASSERT_NE(MAP_FAILED,
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

#define N 1000

int count;
void *ptrs[N];
size_t sizes[N];

void BenchMmapPrivate(void) {
  void *p;
  p = mmap(0, (sizes[count] = rand() % (pagesz * 500)), PROT_READ | PROT_WRITE,
           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  ASSERT_NE(MAP_FAILED, p);
  ptrs[count] = p;
  ++count;
}

void BenchUnmap(void) {
  --count;
  ASSERT_SYS(0, 0, munmap(ptrs[count], sizes[count]));
}

void BenchBigMmap(void) {
  void *p;
  p = mmap(0, 101 * 1024 * 1024, PROT_READ | PROT_WRITE,
           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  ASSERT_NE(MAP_FAILED, p);
  ptrs[count++] = p;
}

void BenchBigMunmap(void) {
  ASSERT_SYS(0, 0, munmap(ptrs[--count], 101 * 1024 * 1024));
}

TEST(mmap, bench) {
  BENCHMARK(N, 1, BenchMmapPrivate());
  BENCHMARK(N, 1, BenchUnmap());
  /* BENCHMARK(N, 1, BenchBigMmap()); */
  /* BENCHMARK(N, 1, BenchBigMunmap()); */
}
