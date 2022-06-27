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
#include "libc/intrin/wait0.internal.h"
#include "libc/log/backtrace.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/gettls.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/clone.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

char *stack, *tls;
int x, me, tid, *childetid;
_Atomic(int) thechilde;

__attribute__((__constructor__)) static void init(void) {
  pledge("stdio rpath thread", 0);
  errno = 0;
}

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
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST ERROR NUMBERS

TEST(clone, testNullFunc_raisesEinval) {
  EXPECT_SYS(EINVAL, -1,
             clone(0, stack, GetStackSize(),
                   CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                       CLONE_SIGHAND | CLONE_SETTLS,
                   0, 0, tls, 64, 0));
}

////////////////////////////////////////////////////////////////////////////////
// TEST THREADS WORK

int CloneTest1(void *arg) {
  intptr_t rsp, top, bot;
  CheckStackIsAligned();
  // PrintBacktraceUsingSymbols(2, __builtin_frame_address(0),
  // GetSymbolTable());
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
  ASSERT_NE(-1, (tid = clone(CloneTest1, stack, GetStackSize(),
                             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES |
                                 CLONE_SIGHAND | CLONE_PARENT_SETTID |
                                 CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID |
                                 CLONE_SETTLS,
                             (void *)23, &ptid, tls, 64, childetid)));
  _wait0(childetid);  // CLONE_CHILD_CLEARTID
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

_Atomic(int) sysbarrier;

int CloneTestSys(void *arg) {
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
  char *tls[8], *stack[8];
  ASSERT_EQ(0, errno);
  for (i = 0; i < 8; ++i) {
    tls[i] = __initialize_tls(malloc(64));
    stack[i] = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
                    MAP_STACK | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(
        -1,
        (tid = clone(
             CloneTestSys, stack[i], GetStackSize(),
             CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
                 CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS,
             (void *)(intptr_t)i, 0, tls[i], 64, (int *)(tls[i] + 0x38))));
  }
  sysbarrier = 1;
  for (i = 0; i < 8; ++i) {
    _wait0((int *)(tls[i] + 0x38));
    free(tls[i]);
    munmap(stack[i], GetStackSize());
  }
  ASSERT_EQ(0, errno);
}

////////////////////////////////////////////////////////////////////////////////
// BENCHMARK

void LaunchThread(void) {
  char *tls, *stack;
  tls = __initialize_tls(malloc(64));
  __cxa_atexit(free, tls, 0);
  stack = mmap(0, GetStackSize(), PROT_READ | PROT_WRITE,
               MAP_STACK | MAP_ANONYMOUS, -1, 0);
  clone(DoNothing, stack, GetStackSize(),
        CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
            CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | CLONE_SETTLS,
        0, 0, tls, 64, (int *)(tls + 0x38));
}

BENCH(clone, bench) {
  char *volatile tp;
  errno_t *volatile ep;
  EZBENCH2("__errno_location", donothing, (ep = __errno_location()));
  EZBENCH2("__get_tls_inline", donothing, (tp = __get_tls_inline()));
  EZBENCH2("__get_tls", donothing, (tp = __get_tls()));
  EZBENCH2("clone()", donothing, LaunchThread());
}
