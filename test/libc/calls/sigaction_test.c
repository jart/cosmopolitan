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
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "third_party/xed/x86.h"

struct sigaction oldsa;
volatile bool gotsigint;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath proc", 0));
}

void OnSigInt(int sig) {
  CheckStackIsAligned();
  gotsigint = true;
}

void SetUp(void) {
  gotsigint = false;
}

////////////////////////////////////////////////////////////////////////////////
// test raise()

TEST(sigaction, raise) {
  struct sigaction saint = {.sa_handler = OnSigInt};
  EXPECT_SYS(0, 0, sigaction(SIGINT, &saint, &oldsa));
  ASSERT_FALSE(gotsigint);
  EXPECT_NE(-1, raise(SIGINT));
  ASSERT_TRUE(gotsigint);
  EXPECT_SYS(0, 0, sigaction(SIGINT, &oldsa, NULL));
}

////////////////////////////////////////////////////////////////////////////////
// test kill()

TEST(sigaction, testPingPongParentChildWithSigint) {
  int pid, status;
  sigset_t blockint, oldmask;
  struct sigaction oldint;
  struct sigaction ignoreint = {.sa_handler = SIG_IGN};
  struct sigaction catchint = {.sa_handler = OnSigInt};
  if (IsWindows()) {
    // this works if it's run by itself on the command prompt. but it
    // doesn't currently work if it's launched as a subprocess of some
    // kind of runner. todo(fixme!)
    return;
  }
  EXPECT_SYS(0, 0, sigemptyset(&blockint));
  EXPECT_SYS(0, 0, sigaddset(&blockint, SIGINT));
  EXPECT_SYS(0, 0, sigprocmask(SIG_BLOCK, &blockint, &oldmask));
  EXPECT_SYS(0, 0, sigaction(SIGINT, &catchint, &oldint));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    // ping
    EXPECT_SYS(0, 0, kill(getppid(), SIGINT));
    EXPECT_FALSE(gotsigint);
    // pong
    EXPECT_SYS(0, 0, sigaction(SIGINT, &catchint, 0));
    EXPECT_SYS(EINTR, -1, sigsuspend(0));
    EXPECT_TRUE(gotsigint);
    _exit(0);
  }
  // pong
  EXPECT_FALSE(gotsigint);
  EXPECT_SYS(0, 0, sigaction(SIGINT, &catchint, 0));
  EXPECT_SYS(EINTR, -1, sigsuspend(0));
  EXPECT_TRUE(gotsigint);
  // ping
  EXPECT_SYS(0, 0, sigaction(SIGINT, &ignoreint, 0));
  EXPECT_SYS(0, 0, kill(pid, SIGINT));
  // cleanup
  EXPECT_NE(-1, wait4(pid, &status, 0, 0));
  EXPECT_EQ(1, WIFEXITED(status));
  EXPECT_EQ(0, WEXITSTATUS(status));
  EXPECT_EQ(0, WTERMSIG(status));
  EXPECT_SYS(0, 0, sigaction(SIGINT, &oldint, 0));
  EXPECT_SYS(0, 0, sigprocmask(SIG_BLOCK, &oldmask, 0));
}

////////////////////////////////////////////////////////////////////////////////
// test int3 crash
// we expect this to be recoverable by default

volatile int trapeax;

void OnTrap(int sig, struct siginfo *si, struct ucontext *ctx) {
  CheckStackIsAligned();
  trapeax = ctx->uc_mcontext.rax;
}

TEST(sigaction, debugBreak_handlerCanReadCpuState) {
  struct sigaction saint = {.sa_sigaction = OnTrap, .sa_flags = SA_SIGINFO};
  EXPECT_NE(-1, sigaction(SIGTRAP, &saint, &oldsa));
  asm("int3" : /* no outputs */ : "a"(0x31337));
  EXPECT_EQ(0x31337, trapeax);
  EXPECT_SYS(0, 0, sigaction(SIGTRAP, &oldsa, NULL));
}

////////////////////////////////////////////////////////////////////////////////
// test fpu crash (unrecoverable)
// test signal handler can modify cpu registers (now it's recoverable!)

void SkipOverFaultingInstruction(struct ucontext *ctx) {
  CheckStackIsAligned();
  struct XedDecodedInst xedd;
  xed_decoded_inst_zero_set_mode(&xedd, XED_MACHINE_MODE_LONG_64);
  xed_instruction_length_decode(&xedd, (void *)ctx->uc_mcontext.rip, 15);
  ctx->uc_mcontext.rip += xedd.length;
}

void OnFpe(int sig, struct siginfo *si, struct ucontext *ctx) {
  CheckStackIsAligned();
  SkipOverFaultingInstruction(ctx);
  ctx->uc_mcontext.rax = 42;
  ctx->uc_mcontext.rdx = 0;
}

noubsan void ubsanTrumpsSystemsEngineering(void) {
  struct sigaction saint = {.sa_sigaction = OnFpe, .sa_flags = SA_SIGINFO};
  EXPECT_SYS(0, 0, sigaction(SIGFPE, &saint, &oldsa));
  volatile long x = 0;
  EXPECT_EQ(42, 666 / x); /* systems engineering trumps math */
  EXPECT_SYS(0, 0, sigaction(SIGFPE, &oldsa, NULL));
}

TEST(sigaction, sigFpe_handlerCanEditProcessStateAndRecoverExecution) {
  ubsanTrumpsSystemsEngineering();
}
