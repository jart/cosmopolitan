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
#include "libc/calls/ucontext.h"
#include "libc/calls/struct/sigset.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/sig.h"

int __tailcontext(const ucontext_t *);

static int __contextmask(const sigset_t *opt_set, sigset_t *opt_out_oldset) {
  if (!__interruptible) return 0;
  // signal handling functions might exist
  // now context switching needs to go 14x slower
  return sigprocmask(SIG_SETMASK, opt_set, opt_out_oldset);
}

/**
 * Sets machine context.
 *
 * This function goes 14x slower if sigaction() has ever been used to
 * install a signal handling function. If you don't care about signal
 * safety and just want fast fibers, then you may override the global
 * variable `__interruptible` to disable the sigprocmask() calls, for
 * pure userspace context switching.
 *
 * @return -1 on error w/ errno, otherwise won't return unless sent back
 * @see swapcontext()
 * @see makecontext()
 * @see getcontext()
 */
int setcontext(const ucontext_t *uc) {
  if (__contextmask(&uc->uc_sigmask, 0)) return -1;
  return __tailcontext(uc);
}

int __getcontextsig(ucontext_t *uc) {
  return __contextmask(0, &uc->uc_sigmask);
}

int __swapcontextsig(ucontext_t *x, const ucontext_t *y) {
  return __contextmask(&y->uc_sigmask, &x->uc_sigmask);
}
