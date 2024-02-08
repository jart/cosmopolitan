/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#include "libc/x/x.h"
#include "third_party/libcxx/math.h"

bool gotsome;
ucontext_t uc, goback;
extern long __klog_handle;

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void check_args(long x0, long x1, long x2, long x3, long x4, long x5, double f0,
                double f1, double f2, double f3, double f4, double f5) {
  EXPECT_EQ(LONG_MIN + 100, x0);
  EXPECT_EQ(LONG_MIN + 101, x1);
  EXPECT_EQ(LONG_MIN + 102, x2);
  EXPECT_EQ(LONG_MIN + 103, x3);
  EXPECT_EQ(LONG_MIN + 104, x4);
  EXPECT_EQ(LONG_MIN + 105, x5);
  EXPECT_TRUE(20. == f0);
  EXPECT_TRUE(21. == f1);
  EXPECT_TRUE(22. == f2);
  EXPECT_TRUE(23. == f3);
  EXPECT_TRUE(24. == f4);
  EXPECT_TRUE(25. == f5);
  gotsome = true;
}

TEST(makecontext, args) {
  char stack[1024];
  getcontext(&uc);
  uc.uc_link = &goback;
  uc.uc_stack.ss_sp = stack;
  uc.uc_stack.ss_size = sizeof(stack);
  makecontext(&uc, check_args, 12, 20., 21., 22., LONG_MIN + 100,
              LONG_MIN + 101, LONG_MIN + 102, LONG_MIN + 103, LONG_MIN + 104,
              LONG_MIN + 105, 23., 24., 25.);
  swapcontext(&goback, &uc);
  EXPECT_TRUE(gotsome);
}

dontasan dontubsan void itsatrap(int x, int y) {
  *(int *)(intptr_t)x = scalbn(x, y);
}

TEST(makecontext, crash) {
  SPAWN(fork);
  getcontext(&uc);
  uc.uc_link = 0;
  uc.uc_stack.ss_sp = NewCosmoStack();
  uc.uc_stack.ss_size = GetStackSize();
  makecontext(&uc, exit, 1, 42);
  setcontext(&uc);
  EXITS(42);
}

TEST(makecontext, backtrace) {
  if (IsTiny()) return;  // doesn't print full crash report
  SPAWN(fork);
  if (IsWindows()) {
    __klog_handle =
        CreateFile(u"log", kNtFileAppendData,
                   kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete, 0,
                   kNtOpenAlways, kNtFileAttributeNormal, 0);
  } else {
    __klog_handle = creat("log", 0644);
  }
  getcontext(&uc);
  uc.uc_link = 0;
  uc.uc_stack.ss_sp = NewCosmoStack();
  uc.uc_stack.ss_size = GetStackSize();
  makecontext(&uc, itsatrap, 2, 123, 456);
  setcontext(&uc);
  TERMS(SIGSEGV);
  if (!GetSymbolTable()) return;
  char *log = gc(xslurp("log", 0));
  EXPECT_NE(0, strstr(log, "itsatrap"));
  EXPECT_NE(0, strstr(log, "runcontext"));
  EXPECT_NE(0, strstr(log, "makecontext_backtrace"));
}
