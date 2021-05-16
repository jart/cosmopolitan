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
#include "libc/calls/sigbits.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/xed/x86.h"

struct sigaction oldsa;

volatile bool gotsigint;

void OnSigInt(int sig) {
  gotsigint = true;
}

void SetUp(void) {
  gotsigint = false;
  /* TODO(jart): Windows needs huge signal overhaul */
  if (IsWindows()) exit(0);
}

TEST(sigaction, test) {
  int pid, status;
  sigset_t block, ignore, oldmask;
  struct sigaction saint = {.sa_handler = OnSigInt};
  sigemptyset(&block);
  sigaddset(&block, SIGINT);
  EXPECT_NE(-1, sigprocmask(SIG_BLOCK, &block, &oldmask));
  sigfillset(&ignore);
  sigdelset(&ignore, SIGINT);
  EXPECT_NE(-1, sigaction(SIGINT, &saint, &oldsa));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    EXPECT_NE(-1, kill(getppid(), SIGINT));
    EXPECT_EQ(-1, sigsuspend(&ignore));
    EXPECT_EQ(EINTR, errno);
    EXPECT_TRUE(gotsigint);
    _exit(0);
  }
  EXPECT_EQ(-1, sigsuspend(&ignore));
  EXPECT_NE(-1, kill(pid, SIGINT));
  EXPECT_NE(-1, waitpid(pid, &status, 0));
  EXPECT_EQ(1, WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));
  EXPECT_EQ(0, WTERMSIG(status));
  EXPECT_NE(-1, sigprocmask(SIG_SETMASK, &oldmask, NULL));
  EXPECT_NE(-1, sigaction(SIGINT, &oldsa, NULL));
}

TEST(sigaction, raise) {
  struct sigaction saint = {.sa_handler = OnSigInt};
  EXPECT_NE(-1, sigaction(SIGINT, &saint, &oldsa));
  ASSERT_FALSE(gotsigint);
  EXPECT_NE(-1, raise(SIGINT));
  ASSERT_TRUE(gotsigint);
  EXPECT_NE(-1, sigaction(SIGINT, &oldsa, NULL));
}

volatile int trapeax;

void OnTrap(int sig, struct siginfo *si, struct ucontext *ctx) {
  trapeax = ctx->uc_mcontext.rax;
}

TEST(sigaction, debugBreak_handlerCanReadCpuState) {
  struct sigaction saint = {.sa_sigaction = OnTrap, .sa_flags = SA_SIGINFO};
  EXPECT_NE(-1, sigaction(SIGTRAP, &saint, &oldsa));
  asm("int3" : /* no outputs */ : "a"(0x31337));
  EXPECT_EQ(0x31337, trapeax);
  EXPECT_NE(-1, sigaction(SIGTRAP, &oldsa, NULL));
}

void OnFpe(int sig, struct siginfo *si, struct ucontext *ctx) {
  struct XedDecodedInst xedd;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
  ctx->uc_mcontext.rax = 42;
  ctx->uc_mcontext.rdx = 0;
}

noubsan void ubsanTrumpsSystemsEngineering(void) {
  struct sigaction saint = {.sa_sigaction = OnFpe, .sa_flags = SA_SIGINFO};
  EXPECT_NE(-1, sigaction(SIGFPE, &saint, &oldsa));
  volatile long x = 0;
  EXPECT_EQ(42, 666 / x); /* systems engineering trumps math */
  EXPECT_NE(-1, sigaction(SIGFPE, &oldsa, NULL));
}

TEST(sigaction, sigFpe_handlerCanEditProcessStateAndRecoverExecution) {
  ubsanTrumpsSystemsEngineering();
}
