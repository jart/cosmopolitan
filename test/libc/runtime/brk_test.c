/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "ape/sections.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

void SetUp(void) {
  if (IsWindows()) {
    ASSERT_SYS(ENOSYS, -1, brk(0));
    ASSERT_SYS(ENOSYS, MAP_FAILED, sbrk(0));
    exit(0);
  }
}

TEST(sbrk, testReportCurrentBreak) {
  ASSERT_SYS(0, _end, sbrk(0));
}

TEST(sbrk, hugeDelta_returnsEoverflow) {
  ASSERT_SYS(EOVERFLOW, MAP_FAILED, sbrk(INTPTR_MAX));
}

TEST(brk, underflowsEnd_returnsEinval) {
  ASSERT_SYS(EINVAL, -1, brk(0));
}

TEST(sbrk, underflowsEnd_returnsEinval) {
  ASSERT_SYS(EINVAL, MAP_FAILED, sbrk(-GUARDSIZE));
}

TEST(sbrk, giantDelta_returnsEnomem) {
  if (IsXnu()) return;   // mmap polyfills this but brk doesn't right now
  if (IsWsl1()) return;  // WSL1 setrlimit() is busted
  SPAWN(fork);
  struct rlimit rl = {1024 * 1024, 1024 * 1024};
  ASSERT_SYS(0, 0, setrlimit(RLIMIT_AS, &rl));
  ASSERT_SYS(ENOMEM, MAP_FAILED, sbrk(1024 * 1024 * 4));
  EXITS(0);
}

TEST(sbrk, overlapsExistingMapping_failsWithEexist) {
  char *p = (char *)ROUNDUP((intptr_t)_end, FRAMESIZE);
  ASSERT_EQ(p, mmap(p, FRAMESIZE, PROT_READ | PROT_WRITE,
                    MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0));
  ASSERT_SYS(EEXIST, MAP_FAILED, sbrk(FRAMESIZE));
  ASSERT_SYS(0, 0, munmap(p, FRAMESIZE));
}

TEST(sbrk, testGrowAndShrink) {
  SPAWN(fork);
  ASSERT_FALSE(testlib_memoryexists(_end));
  ASSERT_SYS(0, _end, sbrk(GUARDSIZE));
  ASSERT_SYS(0, _end + GUARDSIZE, sbrk(0));
  ASSERT_TRUE(testlib_memoryexists(_end));
  ASSERT_FALSE(testlib_memoryexists(_end + GUARDSIZE));
  ASSERT_SYS(0, _end + GUARDSIZE, sbrk(-GUARDSIZE));
  ASSERT_FALSE(testlib_memoryexists(_end));
  EXITS(0);
}

TEST(brk, testGrowAndShrink) {
  SPAWN(fork);
  ASSERT_FALSE(testlib_memoryexists(_end));
  ASSERT_EQ(0, brk(_end + GUARDSIZE));
  ASSERT_TRUE(testlib_memoryexists(_end));
  ASSERT_FALSE(testlib_memoryexists(_end + GUARDSIZE));
  ASSERT_EQ(0, brk(_end));
  EXITS(0);
}
