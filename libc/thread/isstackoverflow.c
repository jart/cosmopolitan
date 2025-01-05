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
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/ucontext.internal.h"
#include "libc/calls/ucontext.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/thread.h"

/**
 * Returns true if signal is caused by stack overflow.
 */
char __is_stack_overflow(siginfo_t *si, void *arg) {

  // sanity check
  ucontext_t *uc = arg;
  if (!si || !uc)
    return false;
  if (si->si_signo != SIGSEGV &&  //
      si->si_signo != SIGBUS)
    return false;

  // get stack information
  pthread_attr_t attr;
  if (pthread_getattr_np(pthread_self(), &attr))
    return false;
  size_t guardsize;
  if (pthread_attr_getguardsize(&attr, &guardsize))
    return false;
  void *stackaddr;
  size_t stacksize;
  if (pthread_attr_getstack(&attr, &stackaddr, &stacksize))
    return false;

  // determine if faulting address is inside guard region
  char *x = (char *)si->si_addr;
  char *lo = (char *)stackaddr - guardsize;
  char *hi = (char *)stackaddr;
  return lo <= x && x < hi;
}
