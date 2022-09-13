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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.internal.h"
#include "libc/runtime/internal.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"

atomic_int itworked;
_Thread_local int var;

void SetUpOnce(void) {
  __enable_threads();
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

int Worker(void *arg, int tid) {
  int i = (long)arg;
  ASSERT_EQ(0, var++);
  ASSERT_EQ(gettid(), tid);
  ASSERT_EQ(1, var++);
  ASSERT_EQ(sys_gettid(), tid);
  ASSERT_EQ(2, var++);
  itworked++;
  return 0;
}

TEST(_spawn, test) {
  long i, n = 128;
  struct spawn *t = gc(malloc(sizeof(struct spawn) * n));
  for (i = 0; i < n; ++i) ASSERT_SYS(0, 0, _spawn(Worker, (void *)i, t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
  for (i = 0; i < n; ++i) EXPECT_SYS(0, 0, _join(t + i));
  EXPECT_EQ(n, itworked);
}
