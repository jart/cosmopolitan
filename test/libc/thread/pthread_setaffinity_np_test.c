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
#include "libc/calls/struct/cpuset.h"
#include "libc/dce.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/thread/thread2.h"

TEST(pthread_setaffinity_np, test) {

  // works on almost nothing
  if (IsXnu())
    return;
  if (IsNetbsd())
    return;
  if (IsOpenbsd())
    return;
  if (__get_cpu_count() < 2)
    return;

  // save bitset
  cpu_set_t old;
  if (!IsWindows())
    ASSERT_EQ(0, pthread_getaffinity_np(pthread_self(), sizeof(old), &old));

  // go to first cpu
  cpu_set_t bitset;
  CPU_ZERO(&bitset);
  CPU_SET(0, &bitset);
  ASSERT_EQ(0, pthread_setaffinity_np(pthread_self(), sizeof(bitset), &bitset));
  ASSERT_EQ(0, sched_getcpu());

  // go to second cpu
  CPU_ZERO(&bitset);
  CPU_SET(1, &bitset);
  ASSERT_EQ(0, pthread_setaffinity_np(pthread_self(), sizeof(bitset), &bitset));
  ASSERT_EQ(1, sched_getcpu());

  // restore bitset
  if (!IsWindows())
    ASSERT_EQ(0, pthread_setaffinity_np(pthread_self(), sizeof(old), &old));
}
