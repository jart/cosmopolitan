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
#include "libc/calls/ucontext.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"

int __tailcontext(const ucontext_t *);

/**
 * Sets machine context.
 *
 * @return -1 on error w/ errno, otherwise won't return unless sent back
 * @see swapcontext()
 * @see makecontext()
 * @see getcontext()
 */
int setcontext(const ucontext_t *uc) {
  if (IsWindows()) {
    atomic_store_explicit(&__get_tls()->tib_sigmask, uc->uc_sigmask,
                          memory_order_release);
  } else {
    sys_sigprocmask(SIG_SETMASK, &uc->uc_sigmask, 0);
  }
  return __tailcontext(uc);
}

int __getcontextsig(ucontext_t *uc) {
  if (IsWindows()) {
    uc->uc_sigmask =
        atomic_load_explicit(&__get_tls()->tib_sigmask, memory_order_acquire);
    return 0;
  } else {
    return sys_sigprocmask(SIG_SETMASK, 0, &uc->uc_sigmask);
  }
}

int __swapcontextsig(ucontext_t *x, const ucontext_t *y) {
  if (IsWindows()) {
    x->uc_sigmask = atomic_exchange_explicit(
        &__get_tls()->tib_sigmask, y->uc_sigmask, memory_order_acquire);
    return 0;
  } else {
    return sys_sigprocmask(SIG_SETMASK, &y->uc_sigmask, &x->uc_sigmask);
  }
}
