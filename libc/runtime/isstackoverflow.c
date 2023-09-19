/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"

/**
 * Returns true if signal is likely a stack overflow.
 */
char __is_stack_overflow(siginfo_t *si, void *ucontext) {

  // check if signal has the information we need
  ucontext_t *uc = ucontext;
  if (!si) return false;
  if (!uc) return false;
  if (si->si_signo != SIGSEGV && si->si_signo != SIGBUS) return false;

  // with threads we know exactly where the guard page is
  int pagesz = getauxval(AT_PAGESZ);
  uintptr_t addr = (uintptr_t)si->si_addr;
  struct PosixThread *pt = _pthread_self();
  if (pt->attr.__stacksize) {
    uintptr_t stack = (uintptr_t)pt->attr.__stackaddr;
    uintptr_t guard = pt->attr.__guardsize;
    uintptr_t bot, top;
    if (guard) {
      bot = stack;
      top = bot + guard;
    } else {
      bot = stack - pagesz;
      top = stack;
    }
    return addr >= bot && addr < top;
  }

  // it's easy to guess with the main stack
  // even though it's hard to know its exact boundaries
  uintptr_t sp = uc->uc_mcontext.SP;
  return addr <= sp && addr >= sp - pagesz;
}
