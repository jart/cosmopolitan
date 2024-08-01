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
 * stack overflow recovery technique #2
 * longjmp out of signal back into thread
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

int StackOverflow(int d) {
  char A[8];
  for (int i = 0; i < sizeof(A); i++)
    A[i] = d + i;
  if (__veil("r", d))
    return StackOverflow(d + 1) + A[d % sizeof(A)];
  return 0;
}

void *MyPosixThread(void *arg) {
  int jumpcode;
  struct sigaction sa, o1, o2;
  struct sigaltstack ss;
  ss.ss_flags = 0;
  ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 4096;
  ss.ss_sp = gc(malloc(ss.ss_size));
  ASSERT_SYS(0, 0, sigaltstack(&ss, 0));
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // <-- important
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = CrashHandler;
  sigaction(SIGBUS, &sa, &o1);
  sigaction(SIGSEGV, &sa, &o2);
  if (!(jumpcode = setjmp(recover))) {
    exit(StackOverflow(0));
  }
  ASSERT_EQ(123, jumpcode);
  sigaction(SIGSEGV, &o2, 0);
  sigaction(SIGBUS, &o1, 0);
  // here's where longjmp() gets us into trouble
  ASSERT_SYS(0, 0, sigaltstack(0, &ss));
  if (IsXnu() || IsNetbsd()) {
    ASSERT_EQ(SS_ONSTACK, ss.ss_flags);  // wut
  } else {
    ASSERT_EQ(0, ss.ss_flags);
  }
  return 0;
}

TEST(stackoverflow, standardStack_altStack_thread_longjmp) {
  pthread_t th;
  struct sigaltstack ss;
  for (int i = 0; i < 2; ++i) {
    smashed_stack = false;
    pthread_create(&th, 0, MyPosixThread, 0);
    pthread_join(th, 0);
    ASSERT_TRUE(smashed_stack);
    // this should be SS_DISABLE but ShowCrashReports() creates an alt stack
    ASSERT_SYS(0, 0, sigaltstack(0, &ss));
    ASSERT_EQ(0, ss.ss_flags);
  }
}
