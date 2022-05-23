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
#include "libc/log/backtrace.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

char *stack, *tls;
int x, me, tid, *childetid;
_Atomic(int) thechilde;

void SetUp(void) {
  x = 0;
  me = gettid();
  tls = calloc(1, 64);
  __initialize_tls(tls);
  *(int *)(tls + 0x3c) = 31337;
  childetid = (int *)(tls + 0x38);
  ASSERT_NE(MAP_FAILED, (stack = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                                      MAP_STACK | MAP_ANONYMOUS, -1, 0)));
}

void TearDown(void) {
  EXPECT_SYS(0, 0, munmap(stack, GetStackSize()));
  free(tls);
}

int DoNothing(void *arg) {
  CheckStackIsAligned();
  return 0;
}

int CloneTest1(void *arg) {
  intptr_t rsp, top, bot;
  CheckStackIsAligned();
  rsp = (intptr_t)__builtin_frame_address(0);
  bot = (intptr_t)stack;
  top = bot + GetStackSize();
  ASSERT_GT(rsp, bot);        // check we're on stack
  ASSERT_LT(rsp, top);        // check we're on stack
  ASSERT_GT(rsp, top - 256);  // check we're near top of stack
  ASSERT_TRUE(IS2POW(GetStackSize()));
  ASSERT_EQ(0, bot & (GetStackSize() - 1));
  x = 42;
  if (!IsWindows()) {
    ASSERT_EQ(31337, errno);
  } else {
    errno = 31337;
    ASSERT_EQ(31337, errno);
  }
  ASSERT_EQ(23, (intptr_t)arg);
  ASSERT_NE(gettid(), getpid());
  ASSERT_EQ(gettid(), *childetid);  // CLONE_CHILD_SETTID
  return 0;
}

TEST(clone, test1) {
  int ptid = 0;
  *childetid = -1;
  _seizelock(childetid);
  ASSERT_NE(-1, (tid = clone(CloneTest1, stack, GetStackSize(),
                             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                                 CLONE_SIGHAND | CLONE_PARENT_SETTID |
                                 CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID |
                                 CLONE_SETTLS,
                             (void *)23, &ptid, tls, 64, childetid)));
  _spinlock(childetid);  // CLONE_CHILD_CLEARTID
  ASSERT_NE(gettid(), tid);
  ASSERT_EQ(tid, ptid);
  ASSERT_EQ(42, x);
  ASSERT_NE(me, tid);
  ASSERT_EQ(0, errno);
  errno = 31337;
  ASSERT_EQ(31337, errno);
  errno = 0;
}

int CloneTestSys(void *arg) {
  CheckStackIsAligned();
  thechilde = gettid();
  ASSERT_EQ(31337, errno);
  open(0, 0);
  ASSERT_EQ(EFAULT, errno);
  return 0;
}

TEST(clone, tlsSystemCallsErrno_wontClobberMainThreadBecauseTls) {
  ASSERT_EQ(0, errno);
  ASSERT_EQ(31337, *(int *)(tls + 0x3c));
  _seizelock(childetid);
  ASSERT_NE(-1, (tid = clone(CloneTestSys, stack, GetStackSize(),
                             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                                 CLONE_SIGHAND | CLONE_CHILD_SETTID |
                                 CLONE_CHILD_CLEARTID | CLONE_SETTLS,
                             (void *)23, 0, tls, 64, childetid)));
  _spinlock(childetid);  // CLONE_CHILD_CLEARTID
  ASSERT_EQ(0, errno);
  ASSERT_EQ(EFAULT, *(int *)(tls + 0x3c));
}

BENCH(clone, bench) {
  errno_t *volatile ep;
  char *volatile tp;
  EZBENCH2("__errno_location", donothing, (ep = __errno_location()));
  EZBENCH2("__get_tls", donothing, (tp = __get_tls()));
}
