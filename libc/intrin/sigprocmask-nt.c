/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

textwindows int __sig_mask(int how, const sigset_t *neu, sigset_t *old) {

  // validate api usage
  if (how != SIG_BLOCK && how != SIG_UNBLOCK && how != SIG_SETMASK) {
    return einval();
  }

  // get address of sigset to modify
  _Atomic(uint64_t) *mask = &__get_tls()->tib_sigmask;

  // handle read-only case
  sigset_t oldmask;
  if (neu) {
    if (how == SIG_BLOCK) {
      oldmask = atomic_fetch_or_explicit(mask, *neu, memory_order_acq_rel);
    } else if (how == SIG_UNBLOCK) {
      oldmask = atomic_fetch_and_explicit(mask, ~*neu, memory_order_acq_rel);
    } else {  // SIG_SETMASK
      oldmask = atomic_exchange_explicit(mask, *neu, memory_order_acq_rel);
    }
  } else {
    oldmask = atomic_load_explicit(mask, memory_order_acquire);
  }

  // return old signal mask to caller
  if (old) {
    *old = oldmask;
  }

  if (_weaken(__sig_check)) {
    _weaken(__sig_check)();
  }

  return 0;
}

#endif /* __x86_64__ */
