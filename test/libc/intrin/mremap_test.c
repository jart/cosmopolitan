/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sysv/consts/mremap.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/timespec.h"
#include "libc/cosmotime.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mremap.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

TEST(cosmo_mremap, dontMove_hasRoom_itMoves) {
  char *p;
  int gransz = getgransize();
  ASSERT_NE(MAP_FAILED,
            (p = mmap(__maps_randaddr(), gransz, PROT_READ | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_FALSE(testlib_memoryexists(p + gransz));
  ASSERT_SYS(0, p, cosmo_mremap(p, gransz, gransz * 2, 0));
  EXPECT_TRUE(testlib_memoryexists(p));
  EXPECT_TRUE(testlib_memoryexists(p + gransz));
  ASSERT_SYS(0, 0, munmap(p, gransz * 2));
  EXPECT_FALSE(testlib_memoryexists(p));
  EXPECT_FALSE(testlib_memoryexists(p + gransz));
}

TEST(cosmo_mremap, dontMove_noRoom_itFailsWithEnomem) {
  char *p;
  int gransz = getgransize();
  ASSERT_NE(MAP_FAILED,
            (p = mmap(__maps_randaddr(), gransz * 2, PROT_READ | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + gransz * 0));
  EXPECT_TRUE(testlib_memoryexists(p + gransz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + gransz * 2));
  ASSERT_SYS(ENOMEM, MAP_FAILED, cosmo_mremap(p, gransz, gransz * 3, 0));
  EXPECT_TRUE(testlib_memoryexists(p + gransz * 0));
  EXPECT_TRUE(testlib_memoryexists(p + gransz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + gransz * 2));
  ASSERT_SYS(0, 0, munmap(p, gransz * 2));
  EXPECT_FALSE(testlib_memoryexists(p + gransz * 0));
  EXPECT_FALSE(testlib_memoryexists(p + gransz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + gransz * 2));
}

TEST(cosmo_mremap, mayMove_noRoom_itRelocates) {
  if (!IsLinux() && !IsNetbsd())
    return;
  char *p, *p2;
  int pagesz = getpagesize();
  ASSERT_NE(MAP_FAILED,
            (p = mmap(__maps_randaddr(), pagesz * 2, PROT_READ | PROT_EXEC,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)));
  EXPECT_TRUE(testlib_memoryexists(p + pagesz * 0));
  EXPECT_TRUE(testlib_memoryexists(p + pagesz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 2));
  ASSERT_NE(MAP_FAILED,
            (p2 = cosmo_mremap(p, pagesz, pagesz * 3, MREMAP_MAYMOVE)));
  ASSERT_NE(p, p2);
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 0));
  EXPECT_TRUE(testlib_memoryexists(p + pagesz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 2));
  EXPECT_TRUE(testlib_memoryexists(p2 + pagesz * 0));
  EXPECT_TRUE(testlib_memoryexists(p2 + pagesz * 1));
  EXPECT_TRUE(testlib_memoryexists(p2 + pagesz * 2));
  ASSERT_SYS(0, 0, munmap(p + pagesz, pagesz));
  ASSERT_SYS(0, 0, munmap(p2, pagesz * 3));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 0));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 1));
  EXPECT_FALSE(testlib_memoryexists(p + pagesz * 2));
  EXPECT_FALSE(testlib_memoryexists(p2 + pagesz * 0));
  EXPECT_FALSE(testlib_memoryexists(p2 + pagesz * 1));
  EXPECT_FALSE(testlib_memoryexists(p2 + pagesz * 2));
}

// demonstrate value of cosmo_mremap() system call
//
//       mmap(1'048'576) took 1'130 ns
//     cosmo_mremap(1'048'576 -> 2'097'152) took 3'117 ns
//     cosmo_mremap(2'097'152 -> 1'048'576) took 3'596 ns
//     cosmo_mremap(1'048'576 -> 2'097'152) took 796'381 ns [simulated]
//     munmap(2'097'152) took 50'020 ns
//

TEST(cosmo_mremap, bench) {
#define N 10
  long size = 1024 * 1024;
  char *rollo = __maps_randaddr();
  char *addr[N];

  // create mappings
  struct timespec ts1 = timespec_real();
  for (long i = 0; i < N; ++i)
    if ((addr[i] = mmap((rollo += size * 2), size, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0)) ==
        MAP_FAILED) {
      kprintf("  first mmap failed: %m\n");
      exit(1);
    }
  kprintf("  mmap(%'zu) took %'ld ns\n", size,
          timespec_tonanos(timespec_sub(timespec_real(), ts1)) / N);

  // use cosmo_mremap to grow mappings
  ts1 = timespec_real();
  for (long i = 0; i < N; ++i)
    if ((addr[i] = cosmo_mremap(addr[i], size, size * 2, MREMAP_MAYMOVE)) ==
        MAP_FAILED) {
      kprintf("  grow cosmo_mremap failed: %m\n");
      exit(1);
    }
  kprintf("  cosmo_mremap(%'zu -> %'zu) took %'ld ns\n", size, size * 2,
          timespec_tonanos(timespec_sub(timespec_real(), ts1)) / N);

  // use cosmo_mremap to shrink mappings
  ts1 = timespec_real();
  for (long i = 0; i < N; ++i)
    if (cosmo_mremap(addr[i], size * 2, size, 0) != addr[i]) {
      kprintf("  shrink cosmo_mremap failed: %m\n");
      exit(1);
    }
  kprintf("  cosmo_mremap(%'zu -> %'zu) took %'ld ns\n", size * 2, size,
          timespec_tonanos(timespec_sub(timespec_real(), ts1)) / N);

  // do the thing that cosmo_mremap is trying to optimize
  ts1 = timespec_real();
  for (long i = 0; i < N; ++i) {
    char *addr2;
    if ((addr2 = mmap(0, size * 2, PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == MAP_FAILED) {
      kprintf("  simulated mmap failed: %m\n");
      exit(1);
    }
    memmove(addr2, addr[i], size);
    if (munmap(addr[i], size)) {
      kprintf("  simulated munmap failed: %m\n");
      exit(1);
    }
    addr[i] = addr2;
  }
  kprintf("  cosmo_mremap(%'zu -> %'zu) took %'ld ns [simulated]\n", size,
          size * 2, timespec_tonanos(timespec_sub(timespec_real(), ts1)) / N);

  // unmap mappings
  ts1 = timespec_real();
  for (long i = 0; i < N; ++i)
    if (munmap(addr[i], size * 2)) {
      kprintf("  munmap failed: %m\n");
      exit(1);
    }
  kprintf("  munmap(%'zu) took %'ld ns\n", size * 2,
          timespec_tonanos(timespec_sub(timespec_real(), ts1)) / N);
}
