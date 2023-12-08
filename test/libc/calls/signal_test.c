/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void OnUsr1(int sig) {
  _exit(0);
}

void SetUpOnce(void) {
  sigset_t ss;
  sigprocmask(SIG_SETMASK, 0, &ss);
  ASSERT_SYS(0, 0, pledge("stdio proc", 0));
}

TEST(signal, test) {
  ASSERT_NE(SIG_ERR, signal(SIGUSR1, OnUsr1));
  ASSERT_NE(-1, raise(SIGUSR1));
  __die();
}

////////////////////////////////////////////////////////////////////////////////
// signal round-trip delivery takes about 1µs

void OnSigTrap(int sig, struct siginfo *si, void *ctx) {
}

void TrapBench(int n) {
  for (int i = 0; i < n; ++i) {
    __builtin_trap();
  }
}

BENCH(signal, trapBench) {
  struct sigaction old;
  struct sigaction sabus = {.sa_sigaction = OnSigTrap};
  ASSERT_SYS(0, 0, sigaction(SIGTRAP, &sabus, &old));
  EZBENCH_N("signal trap", 16, TrapBench(16));
  EZBENCH_N("signal trap", 256, TrapBench(256));
  EZBENCH_N("signal trap", 1024, TrapBench(1024));
  sigaction(SIGTRAP, &old, 0);
}

BENCH(signal, trapBenchSiginfo) {
  struct sigaction old;
  struct sigaction sabus = {.sa_sigaction = OnSigTrap, .sa_flags = SA_SIGINFO};
  ASSERT_SYS(0, 0, sigaction(SIGTRAP, &sabus, &old));
  EZBENCH_N("siginfo trap", 16, TrapBench(16));
  EZBENCH_N("siginfo trap", 256, TrapBench(256));
  EZBENCH_N("siginfo trap", 1024, TrapBench(1024));
  sigaction(SIGTRAP, &old, 0);
}

#ifdef __x86_64__

void OnSigHlt(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
  ctx->uc_mcontext.rip += 1;
}

void HltBench(int n) {
  for (int i = 0; i < n; ++i) {
    asm("hlt");
  }
}

BENCH(signal, hltBenchSiginfo) {
  struct sigaction old[2];
  struct sigaction sabus = {.sa_sigaction = OnSigHlt, .sa_flags = SA_SIGINFO};
  ASSERT_SYS(0, 0, sigaction(SIGSEGV, &sabus, old + 0));
  ASSERT_SYS(0, 0, sigaction(SIGBUS, &sabus, old + 1));
  EZBENCH_N("siginfo hlt", 16, HltBench(16));
  EZBENCH_N("siginfo hlt", 256, HltBench(256));
  EZBENCH_N("siginfo hlt", 1024, HltBench(1024));
  sigaction(SIGSEGV, old + 0, 0);
  sigaction(SIGBUS, old + 1, 0);
}

#endif /* __x86_64__ */
