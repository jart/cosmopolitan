/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/pledge.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/uc.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
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

void TearDown(void) {
  sigset_t ss;
  sigprocmask(SIG_SETMASK, 0, &ss);
  ASSERT_TRUE(sigisemptyset(&ss));
}

////////////////////////////////////////////////////////////////////////////////
// test that signal handlers expose cpu state, and let it be changed arbitrarily

void *Gateway(void *arg) {
  __builtin_trap();
}

void PromisedLand(void *arg) {
  sigset_t ss;
  CheckStackIsAligned();
  sigprocmask(SIG_SETMASK, 0, &ss);
  ASSERT_TRUE(sigismember(&ss, SIGUSR1));
  pthread_exit(arg);
}

void Teleporter(int sig, struct siginfo *si, void *ctx) {
  ucontext_t *uc = ctx;
  sigaddset(&uc->uc_sigmask, SIGUSR1);
  uc->uc_mcontext.PC = (uintptr_t)PromisedLand;
  uc->uc_mcontext.SP &= -16;
#ifdef __x86_64__
  uc->uc_mcontext.SP -= 8;
#endif
}

TEST(sigaction, handlersCanMutateMachineState) {
  void *rc;
  sigset_t ss;
  pthread_t t;
  struct sigaction oldill, oldtrap;
  struct sigaction sa = {.sa_sigaction = Teleporter, .sa_flags = SA_SIGINFO};
  sigprocmask(SIG_SETMASK, 0, &ss);
  ASSERT_FALSE(sigismember(&ss, SIGUSR1));
  ASSERT_SYS(0, 0, sigaction(SIGILL, &sa, &oldill));
  ASSERT_SYS(0, 0, sigaction(SIGTRAP, &sa, &oldtrap));
  ASSERT_EQ(0, pthread_create(&t, 0, Gateway, (void *)42L));
  ASSERT_EQ(0, pthread_join(t, &rc));
  ASSERT_EQ(42, (uintptr_t)rc);
  ASSERT_SYS(0, 0, sigaction(SIGILL, &oldill, 0));
  ASSERT_SYS(0, 0, sigaction(SIGTRAP, &oldtrap, 0));
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
  if (IsNetbsd()) return;  // TODO: what's up with runitd on netbsd?
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
  EXPECT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldmask, 0));
}

#ifdef __x86_64__

////////////////////////////////////////////////////////////////////////////////
// test int3 crash
// we expect this to be recoverable by default

volatile int trapeax;

void OnTrap(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
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

void OnFpe(int sig, struct siginfo *si, void *vctx) {
  struct ucontext *ctx = vctx;
  CheckStackIsAligned();
  SkipOverFaultingInstruction(ctx);
  ctx->uc_mcontext.rax = 42;
  ctx->uc_mcontext.rdx = 0;
}

dontubsan void ubsanTrumpsSystemsEngineering(void) {
  struct sigaction saint = {.sa_sigaction = OnFpe, .sa_flags = SA_SIGINFO};
  EXPECT_SYS(0, 0, sigaction(SIGFPE, &saint, &oldsa));
  volatile long x = 0;
  EXPECT_EQ(42, 666 / x); /* systems engineering trumps math */
  EXPECT_SYS(0, 0, sigaction(SIGFPE, &oldsa, NULL));
}

TEST(sigaction, sigFpe_handlerCanEditProcessStateAndRecoverExecution) {
  ubsanTrumpsSystemsEngineering();
}

#endif /* __x86_64__ */

static unsigned OnSignalCnt = 0;
void OnSignal(int sig, siginfo_t *si, void *ctx) {
  OnSignalCnt++;
}

TEST(sigaction, ignoringSignalDiscardsSignal) {
  struct sigaction sa = {.sa_sigaction = OnSignal, .sa_flags = SA_SIGINFO};
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, NULL));
  sigset_t blocked, oldmask;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGUSR1);
  ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &blocked, &oldmask));
  ASSERT_EQ(0, raise(SIGUSR1));
  ASSERT_NE(SIG_ERR, signal(SIGUSR1, SIG_IGN));
  ASSERT_EQ(0, sigaction(SIGUSR1, &sa, NULL));
  ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &oldmask, NULL));
  EXPECT_EQ(0, OnSignalCnt);
}

TEST(sigaction, autoZombieSlayer) {
  if (IsWindows()) return;
  if (IsCygwin()) return;
  int pid;
  struct sigaction sa;
  // make sure we're starting in expected state
  ASSERT_SYS(0, 0, sigaction(SIGCHLD, 0, &sa));
  ASSERT_EQ(SIG_DFL, sa.sa_handler);
  // verify child becomes zombie
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) _Exit(0);
  ASSERT_SYS(0, pid, wait(0));
  // enable automatic zombie slayer
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);
  ASSERT_SYS(0, 0, sigaction(SIGCHLD, &sa, &sa));
  // verify it works
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) _Exit(0);
  // XXX: WSL does the wrong thing here.
  if (__iswsl1()) usleep(10);
  ASSERT_SYS(ECHILD, -1, wait(0));
  // clean up
  ASSERT_SYS(0, 0, sigaction(SIGCHLD, &sa, 0));
}

TEST(sigaction, enosys_returnsErrnoRatherThanSigsysByDefault) {
  if (IsTiny()) return;     // systemfive.S disables the fix w/ tiny
  if (IsOpenbsd()) return;  // TODO: Why does OpenBSD raise SIGABRT?
  ASSERT_SYS(ENOSYS, -1, sys_bogus());
}

sig_atomic_t gotusr1;

void OnSigMask(int sig, struct siginfo *si, void *ctx) {
  ucontext_t *uc = ctx;
  sigaddset(&uc->uc_sigmask, sig);
  gotusr1 = true;
}

TEST(uc_sigmask, signalHandlerCanChangeSignalMaskOfTrappedThread) {
  sigset_t want, got;
  struct sigaction oldsa;
  struct sigaction sa = {.sa_sigaction = OnSigMask, .sa_flags = SA_SIGINFO};
  sigemptyset(&want);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &want, &got));
  ASSERT_FALSE(sigismember(&got, SIGUSR1));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));
  ASSERT_SYS(0, 0, raise(SIGUSR1));
  ASSERT_TRUE(gotusr1);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, 0, &got));
  ASSERT_TRUE(sigismember(&got, SIGUSR1));
  sigaddset(&want, SIGUSR1);
  ASSERT_EQ(0, errno);
  ASSERT_STREQ(DescribeSigset(0, &want), DescribeSigset(0, &got));
  ASSERT_EQ(0, errno);
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
  sigdelset(&want, SIGUSR1);
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &want, 0));
}

TEST(sig_ign, discardsPendingSignalsEvenIfBlocked) {
  sigset_t block, oldmask;
  struct sigaction sa, oldsa;
  ASSERT_SYS(0, 0, sigemptyset(&block));
  ASSERT_SYS(0, 0, sigaddset(&block, SIGUSR1));
  ASSERT_SYS(0, 0, sigprocmask(SIG_BLOCK, &block, &oldmask));
  raise(SIGUSR1);  // enqueue
  sa.sa_flags = 0;
  sa.sa_handler = SIG_IGN;
  ASSERT_SYS(0, 0, sigemptyset(&sa.sa_mask));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &sa, &oldsa));  // discard
  ASSERT_SYS(0, 0, sigprocmask(SIG_UNBLOCK, &block, 0));
  ASSERT_SYS(0, 0, sigaction(SIGUSR1, &oldsa, 0));
  ASSERT_SYS(0, 0, sigprocmask(SIG_SETMASK, &oldmask, 0));
}

void AutoMask(int sig, struct siginfo *si, void *ctx) {
  sigset_t ss;
  ucontext_t *uc = ctx;
  sigprocmask(SIG_SETMASK, 0, &ss);
  EXPECT_FALSE(sigismember(&uc->uc_sigmask, SIGUSR2));  // original mask
  EXPECT_TRUE(sigismember(&ss, SIGUSR2));               // temporary mask
}

TEST(sigaction, signalBeingDeliveredGetsAutoMasked) {
  sigset_t ss;
  struct sigaction os, sa = {.sa_sigaction = AutoMask, .sa_flags = SA_SIGINFO};
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &sa, &os));
  raise(SIGUSR2);
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &os, 0));
  sigprocmask(SIG_SETMASK, 0, &ss);
  EXPECT_FALSE(sigismember(&ss, SIGUSR2));  // original mask
}

void NoDefer(int sig, struct siginfo *si, void *ctx) {
  sigset_t ss;
  ucontext_t *uc = ctx;
  sigprocmask(SIG_SETMASK, 0, &ss);
  EXPECT_FALSE(sigismember(&uc->uc_sigmask, SIGUSR2));
  EXPECT_FALSE(sigismember(&ss, SIGUSR2));
}

TEST(sigaction, NoDefer) {
  struct sigaction os;
  struct sigaction sa = {
      .sa_sigaction = NoDefer,
      .sa_flags = SA_SIGINFO | SA_NODEFER,
  };
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &sa, &os));
  raise(SIGUSR2);
  ASSERT_SYS(0, 0, sigaction(SIGUSR2, &os, 0));
}

int *segfaults;

void OnSegfault(int sig) {
  if (++*segfaults == 10000) {
    _Exit(0);
  }
}

dontubsan dontasan int Segfault(char *p) {
  return *p;
}

int (*pSegfault)(char *) = Segfault;

TEST(sigaction, returnFromSegvHandler_loopsForever) {
  if (IsXnu()) return;  // seems busted
  segfaults = _mapshared(sizeof(*segfaults));
  SPAWN(fork);
  signal(SIGSEGV, OnSegfault);
  _Exit(pSegfault(0));
  EXITS(0);
  ASSERT_EQ(10000, *segfaults);
  munmap(segfaults, sizeof(*segfaults));
}

#if 0
TEST(sigaction, ignoreSigSegv_notPossible) {
  if (IsXnu()) return;  // seems busted
  SPAWN(fork);
  signal(SIGSEGV, SIG_IGN);
  _Exit(pSegfault(0));
  TERMS(SIGSEGV);
}

TEST(sigaction, killSigSegv_canBeIgnored) {
  int child, ws;
  if (IsWindows()) return;  // TODO
  sighandler_t old = signal(SIGSEGV, SIG_IGN);
  ASSERT_NE(-1, (child = fork()));
  while (!child) {
    pause();
  }
  ASSERT_SYS(0, 0, kill(child, SIGSEGV));
  EXPECT_SYS(0, 0, kill(child, SIGTERM));
  EXPECT_SYS(0, child, wait(&ws));
  EXPECT_EQ(SIGTERM, ws);
  signal(SIGSEGV, old);
}
#endif
