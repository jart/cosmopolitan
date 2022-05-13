/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/spinlock.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/testlib.h"

int x, thechilde;
_Alignas(64) volatile char lock;

void SetUp(void) {
  x = 0;
  lock = 0;
  thechilde = 0;
}

int thread(void *arg) {
  x = 42;
  ASSERT_EQ(23, (intptr_t)arg);
  thechilde = gettid();
  _spunlock(&lock);
  return 0;
}

TEST(clone, test) {
  if (IsXnu()) return;
  int me, tid;
  char *stack;
  me = gettid();
  _spinlock(&lock);
  stack = mmap(0, FRAMESIZE, PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
  ASSERT_NE(-1, (tid = clone(thread, stack, FRAMESIZE,
                             CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND,
                             (void *)23, 0, 0, 0, 0)));
  _spinlock(&lock);
  ASSERT_EQ(42, x);
  ASSERT_NE(me, tid);
  ASSERT_EQ(tid, thechilde);
  EXPECT_SYS(0, 0, munmap(stack, FRAMESIZE));
}
