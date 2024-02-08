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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

/**
 * stack overflow recovery technique #3
 * rewrite thread cpu state to call pthread_exit
 * this method returns gracefully from signal handlers
 * unfortunately it relies on cpu architecture knowledge
 */

volatile bool smashed_stack;

void Exiter(void *rc) {
  struct sigaltstack ss;
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  ASSERT_EQ(0, ss.ss_flags);
  pthread_exit(rc);
}

void CrashHandler(int sig, siginfo_t *si, void *arg) {
  ucontext_t *ctx = arg;
  struct sigaltstack ss;
  ASSERT_FALSE(smashed_stack);
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  ASSERT_EQ(SS_ONSTACK, ss.ss_flags);
  kprintf("kprintf avoids overflowing %G %p\n", si->si_signo, si->si_addr);
  smashed_stack = true;
  // EXPECT_TRUE(__is_stack_overflow(si, ctx));
  //
  // the backtrace will look like this
  //
  //   0x000000000042561d: pthread_exit at pthread_exit.c:99
  //   0x0000000000418777: Exiter at stackoverflow2_test.c:40
  //   0x00000000004186d8: CrashHandler at stackoverflow2_test.c:49
  //   0x000000000041872a: StackOverflow at stackoverflow2_test.c:53
  //   0x000000000041872a: StackOverflow at stackoverflow2_test.c:53
  //   0x000000000041872a: StackOverflow at stackoverflow2_test.c:53
  //   ...
  //
  ctx->uc_mcontext.ARG0 = 123;
  ctx->uc_mcontext.PC = (long)Exiter;
  ctx->uc_mcontext.SP += 32768;
  ctx->uc_mcontext.SP &= -16;
#ifdef __x86_64__
  *(long *)(ctx->uc_mcontext.SP -= 8) = ctx->uc_mcontext.PC;
#elif defined(__aarch64__)
  *(long *)(ctx->uc_mcontext.SP -= 8) = ctx->uc_mcontext.regs[30];
  *(long *)(ctx->uc_mcontext.SP -= 8) = ctx->uc_mcontext.regs[29];
  ctx->uc_mcontext.BP = ctx->uc_mcontext.SP;
#else
#error "unsupported architecture"
#endif
}

int StackOverflow(int f(), int n) {
  if (n < INT_MAX) {
    return f(f, n + 1) - 1;
  } else {
    return INT_MAX;
  }
}

int (*pStackOverflow)(int (*)(), int) = StackOverflow;

void *MyPosixThread(void *arg) {
  struct sigaction sa;
  struct sigaltstack ss;
  ss.ss_flags = 0;
  ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 4096;
  ss.ss_sp = gc(malloc(ss.ss_size));
  ASSERT_SYS(0, 0, sigaltstack(&ss, 0));
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // <-- important
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = CrashHandler;
  sigaction(SIGBUS, &sa, 0);
  sigaction(SIGSEGV, &sa, 0);
  exit(pStackOverflow(pStackOverflow, 0));
  return 0;
}

TEST(stackoverflow, standardStack_altStack_thread_teleport) {
  void *res;
  pthread_t th;
  struct sigaltstack ss;
  smashed_stack = false;
  pthread_create(&th, 0, MyPosixThread, 0);
  pthread_join(th, &res);
  ASSERT_EQ((void *)123L, res);
  ASSERT_TRUE(smashed_stack);
  // this should be SS_DISABLE but ShowCrashReports() creates an alt stack
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  ASSERT_EQ(0, ss.ss_flags);
}
