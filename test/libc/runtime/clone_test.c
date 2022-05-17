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
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/spinlock.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/stack.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

char *stack, *tls;
int x, me, tid, thechilde;
_Alignas(64) volatile char lock;

void SetUp(void) {
  x = 0;
  lock = 0;
  me = gettid();
  thechilde = 0;
  tls = calloc(1, 64);
  __initialize_tls(tls);
  *(int *)(tls + 0x3c) = 31337;
  ASSERT_NE(MAP_FAILED, (stack = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                                      MAP_STACK | MAP_ANONYMOUS, -1, 0)));
}

void TearDown(void) {
  free(tls);
}

int CloneTest1(void *arg) {
  _checkstackalign();
  x = 42;
  if (!IsWindows()) {
    ASSERT_EQ(31337, errno);
  } else {
    errno = 31337;
    ASSERT_EQ(31337, errno);
  }
  ASSERT_EQ(23, (intptr_t)arg);
  thechilde = gettid();
  ASSERT_NE(gettid(), getpid());
  _spunlock(&lock);
  return 0;
}

int DoNothing(void *arg) {
  _checkstackalign();
  return 0;
}

TEST(clone, test1) {
  _spinlock(&lock);
  ASSERT_NE(-1, (tid = clone(CloneTest1, stack, GetStackSize(),
                             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                                 CLONE_SIGHAND | CLONE_SETTLS,
                             (void *)23, 0, tls, 64, 0)));
  _spinlock(&lock);
  ASSERT_EQ(42, x);
  ASSERT_NE(me, tid);
  ASSERT_EQ(tid, thechilde);
  ASSERT_EQ(0, errno);
  errno = 31337;
  ASSERT_EQ(31337, errno);
  errno = 0;
}

int CloneTestSys(void *arg) {
  _checkstackalign();
  thechilde = gettid();
  ASSERT_EQ(31337, errno);
  open(0, 0);
  ASSERT_EQ(EFAULT, errno);
  _spunlock(&lock);
  return 0;
}

TEST(clone, tlsSystemCallsErrno_wontClobberMainThreadBecauseTls) {
  ASSERT_EQ(0, errno);
  ASSERT_EQ(31337, *(int *)(tls + 0x3c));
  _spinlock(&lock);
  ASSERT_NE(-1, (tid = clone(CloneTestSys, stack, GetStackSize(),
                             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                                 CLONE_SIGHAND | CLONE_SETTLS,
                             (void *)23, 0, tls, 64, 0)));
  _spinlock(&lock);
  ASSERT_EQ(0, errno);
  ASSERT_EQ(EFAULT, *(int *)(tls + 0x3c));
}

BENCH(clone, bench) {
  errno_t *volatile ep;
  char *volatile tp;
  EZBENCH2("__errno_location", donothing, (ep = (__errno_location())));
  EZBENCH2("__get_tls", donothing, (tp = __get_tls()));
}
