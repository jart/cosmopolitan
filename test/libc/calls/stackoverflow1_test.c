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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/struct/rlimit.h"
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
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/rlimit.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/thread.h"

/**
 * stack overflow recovery technique #1
 * overflow the gigantic main process stack
 * simple but it can upset kernels / libraries
 */

jmp_buf recover;
atomic_bool g_isdone;
atomic_bool smashed_stack;

void CrashHandler(int sig, siginfo_t *si, void *ctx) {
  struct sigaltstack ss;
  unassert(!sigaltstack(0, &ss));
  unassert(SS_ONSTACK == ss.ss_flags);
  kprintf("kprintf avoids overflowing %G si_addr=%lx sp=%lx\n", si->si_signo,
          si->si_addr, ((ucontext_t *)ctx)->uc_mcontext.SP);
  smashed_stack = true;
  // unassert(__is_stack_overflow(si, ctx)); // fuzzy with main thread
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
    unassert(!setrlimit(RLIMIT_STACK, &rl));
  }

  // set up the signal handler and alternative stack
  struct sigaction sa;
  struct sigaltstack ss;
  ss.ss_flags = 0;
  ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 8192;
  ss.ss_sp = _mapanon(ss.ss_size);
  unassert(!sigaltstack(&ss, 0));
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // <-- important
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = CrashHandler;
  sigaction(SIGBUS, &sa, 0);
  sigaction(SIGSEGV, &sa, 0);
}

int StackOverflow(int d) {
  char A[8];
  for (int i = 0; i < sizeof(A); i++)
    A[i] = d + i;
  if (__veil("r", d))
    return StackOverflow(d + 1) + A[d % sizeof(A)];
  return 0;
}

void *innocent_thread(void *arg) {
  atomic_long dont_clobber_me_bro = 0;
  while (!g_isdone)
    unassert(!dont_clobber_me_bro);
  return 0;
}

int main() {

  // libc/intrin/stack.c is designed so that this thread's stack should
  // be allocated right beneath the main thread's stack. our goal is to
  // make sure overflowing the main stack won't clobber our poor thread
  pthread_t th;
  unassert(!pthread_create(&th, 0, innocent_thread, 0));

  SetUp();

  int jumpcode;
  if (!(jumpcode = setjmp(recover)))
    exit(StackOverflow(1));
  unassert(123 == jumpcode);
  unassert(smashed_stack);

  // join the thread
  g_isdone = true;
  unassert(!pthread_join(th, 0));

  // here's where longjmp() gets us into trouble
  struct sigaltstack ss;
  unassert(!sigaltstack(0, &ss));
  if (IsXnu() || IsNetbsd()) {
    unassert(SS_ONSTACK == ss.ss_flags);  // wut
  } else {
    unassert(0 == ss.ss_flags);
  }
}
