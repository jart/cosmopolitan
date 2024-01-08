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
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

/**
 * stack overflow recovery technique #1
 * overflow the gigantic main process stack
 * simple but it can upset kernels / libraries
 */

jmp_buf recover;
volatile bool smashed_stack;

void CrashHandler(int sig, siginfo_t *si, void *ctx) {
  struct sigaltstack ss;
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  ASSERT_EQ(SS_ONSTACK, ss.ss_flags);
  kprintf("kprintf avoids overflowing %G %p\n", si->si_signo, si->si_addr);
  smashed_stack = true;
  ASSERT_TRUE(__is_stack_overflow(si, ctx));
  longjmp(recover, 123);
}

void SetUp(void) {

  // tune down the main process's stack size to a reasonable amount
  // some operating systems, e.g. freebsd, will do things like have
  // 500mb RLIMIT_STACK by default, even on machines with 400mb RAM
  if (!IsWindows() && !IsXnu()) {
    struct rlimit rl;
    getrlimit(RLIMIT_STACK, &rl);
    rl.rlim_cur = MIN(rl.rlim_cur, 2 * 1024 * 1024);
    ASSERT_SYS(0, 0, setrlimit(RLIMIT_STACK, &rl));
  }

  // set up the signal handler and alternative stack
  struct sigaction sa;
  struct sigaltstack ss;
  ss.ss_flags = 0;
  ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 8192;
  ss.ss_sp = _mapanon(ss.ss_size);
  ASSERT_SYS(0, 0, sigaltstack(&ss, 0));
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // <-- important
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = CrashHandler;
  sigaction(SIGBUS, &sa, 0);
  sigaction(SIGSEGV, &sa, 0);
}

int StackOverflow(int f(), int n) {
  if (n < INT_MAX) {
    return f(f, n + 1) - 1;
  } else {
    return INT_MAX;
  }
}

int (*pStackOverflow)(int (*)(), int) = StackOverflow;

TEST(stackoverflow, standardStack_altStack_process_longjmp) {
  if (IsTiny()) return;  // TODO(jart): why?

  int jumpcode;
  if (!(jumpcode = setjmp(recover))) {
    exit(pStackOverflow(pStackOverflow, 0));
  }
  ASSERT_EQ(123, jumpcode);
  ASSERT_TRUE(smashed_stack);

  // here's where longjmp() gets us into trouble
  struct sigaltstack ss;
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  if (IsXnu() || IsNetbsd()) {
    ASSERT_EQ(SS_ONSTACK, ss.ss_flags);  // wut
  } else {
    ASSERT_EQ(0, ss.ss_flags);
  }
}
