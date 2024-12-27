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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigaltstack.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/cosmo.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/maps.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/ss.h"
#include "libc/thread/thread.h"

/**
 * stack overflow recovery technique #2
 * longjmp out of signal back into thread
 * simple but it can upset kernels / libraries
 */

sigjmp_buf recover;
atomic_bool is_done;
atomic_bool smashed_stack;
atomic_bool clobbered_other_thread;

void CrashHandler(int sig, siginfo_t *si, void *ctx) {
  struct sigaltstack ss;
  unassert(!sigaltstack(0, &ss));
  unassert(SS_ONSTACK == ss.ss_flags);
  kprintf("kprintf avoids overflowing %G si_addr=%lx sp=%lx\n", si->si_signo,
          si->si_addr, ((ucontext_t *)ctx)->uc_mcontext.SP);
  smashed_stack = true;
  unassert(__is_stack_overflow(si, ctx));
  siglongjmp(recover, 123);
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
  ss.ss_size = sysconf(_SC_MINSIGSTKSZ) + 2048;
  ss.ss_sp = gc(malloc(ss.ss_size));
  unassert(!sigaltstack(&ss, 0));
  sa.sa_flags = SA_SIGINFO | SA_ONSTACK;  // <-- important
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = CrashHandler;
  sigaction(SIGBUS, &sa, &o1);
  sigaction(SIGSEGV, &sa, &o2);
  if (!(jumpcode = sigsetjmp(recover, 1)))
    exit(StackOverflow(1));
  unassert(123 == jumpcode);
  sigaction(SIGSEGV, &o2, 0);
  sigaction(SIGBUS, &o1, 0);
  // here's where longjmp() gets us into trouble
  unassert(!sigaltstack(0, &ss));
  if (IsXnu() || IsNetbsd()) {
    unassert(SS_ONSTACK == ss.ss_flags);  // wut
  } else {
    unassert(!ss.ss_flags);
  }
  return 0;
}

void *InnocentThread(void *arg) {
  atomic_long dont_clobber_me_bro = 0;
  while (!is_done)
    if (dont_clobber_me_bro)
      clobbered_other_thread = true;
  pthread_exit(0);
}

int main() {
  pthread_t th, in;
  struct sigaltstack ss;
  for (int i = 0; i < 2; ++i) {
    is_done = false;
    smashed_stack = false;
    unassert(!pthread_create(&th, 0, MyPosixThread, 0));
    unassert(!pthread_create(&in, 0, InnocentThread, 0));
    unassert(!pthread_join(th, 0));
    unassert(smashed_stack);
    unassert(!sigaltstack(0, &ss));
    unassert(ss.ss_flags == SS_DISABLE);
    unassert(!clobbered_other_thread);
    is_done = true;
    unassert(!pthread_join(in, 0));
  }
}
