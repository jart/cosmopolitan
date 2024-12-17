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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

textwindows int __sig_mask(int how, const sigset_t *neu, sigset_t *old) {
  if (how != SIG_BLOCK && how != SIG_UNBLOCK && how != SIG_SETMASK)
    return einval();
  sigset_t oldmask;
  atomic_ulong *mask = &__get_tls()->tib_sigmask;
  if (neu) {
    if (how == SIG_BLOCK) {
      oldmask = atomic_fetch_or(mask, *neu);
    } else if (how == SIG_UNBLOCK) {
      oldmask = atomic_fetch_and(mask, ~*neu);
    } else {
      oldmask = atomic_exchange(mask, *neu);
    }
    if (_weaken(__sig_check))
      _weaken(__sig_check)();
  } else {
    oldmask = atomic_load(mask);
  }
  if (old)
    *old = oldmask;
  return 0;
}

#endif /* __x86_64__ */
