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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/stack.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/spawn.h"

int x, me, tid;
atomic_int thechilde;

void SetUpOnce(void) {
  __enable_threads();
  ASSERT_SYS(0, 0, pledge("stdio rpath", 0));
}

int Hog(void *arg, int tid) {
  return 0;
}

void SetUp(void) {
  x = 0;
  me = gettid();
}

void TearDown(void) {
}

int DoNothing(void *arg) {
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST THREADS WORK

int CloneTest1(void *arg, int tid) {
  intptr_t rsp, top, bot;
  CheckStackIsAligned();
  // PrintBacktraceUsingSymbols(2, __builtin_frame_address(0),
  // GetSymbolTable());
  rsp = (intptr_t)__builtin_frame_address(0);
  bot = ROUNDDOWN((intptr_t)rsp, GetStackSize());
  top = bot + GetStackSize();
  ASSERT_GT(rsp, bot);        // check we're on stack
  ASSERT_LT(rsp, top);        // check we're on stack
  ASSERT_GT(rsp, top - 256);  // check we're near top of stack
  ASSERT_TRUE(IS2POW(GetStackSize()));
  ASSERT_EQ(0, bot & (GetStackSize() - 1));
  x = 42;
  ASSERT_EQ(23, (intptr_t)arg);
  ASSERT_NE(gettid(), getpid());
  return 0;
}

TEST(clone, test1) {
  int ptid = 0;
  struct spawn th;
  ASSERT_SYS(0, 0, _spawn(CloneTest1, (void *)23, &th));
  ASSERT_SYS(0, 0, _join(&th));
  ASSERT_NE(gettid(), tid);
  ASSERT_EQ(tid, ptid);
  ASSERT_EQ(42, x);
  ASSERT_NE(me, tid);
  ASSERT_EQ(0, errno);
  errno = 31337;
  ASSERT_EQ(31337, errno);
  errno = 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST THREADS CAN ISSUE SYSTEM CALLS WITH INDEPENDENT ERRNOS

atomic_int sysbarrier;

int CloneTestSys(void *arg, int tid) {
  int i, id = (intptr_t)arg;
  CheckStackIsAligned();
  while (!sysbarrier) asm("pause");
  for (i = 0; i < 20; ++i) {
    switch (id % 3) {
      case 0:
        errno = 123;
        open(0, 0);
        asm("pause");
        ASSERT_EQ(EFAULT, errno);
        break;
      case 1:
        errno = 123;
        dup(-1);
        asm("pause");
        ASSERT_EQ(EBADF, errno);
        break;
      case 2:
        errno = 123;
        dup3(0, 0, 0);
        asm("pause");
        ASSERT_EQ(EINVAL, errno);
        break;
      default:
        unreachable;
    }
  }
  return 0;
}

TEST(clone, tlsSystemCallsErrno_wontClobberMainThreadBecauseTls) {
  int i;
  struct spawn th[8];
  ASSERT_EQ(0, errno);
  for (i = 0; i < 8; ++i) {
    ASSERT_SYS(0, 0, _spawn(CloneTestSys, (void *)(intptr_t)i, th + i));
  }
  sysbarrier = 1;
  for (i = 0; i < 8; ++i) {
    ASSERT_SYS(0, 0, _join(th + i));
  }
  ASSERT_EQ(0, errno);
}
