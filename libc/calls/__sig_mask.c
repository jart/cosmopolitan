/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"

#define GetSigBit(x) (1ull << (((x)-1) & 63))

textwindows int __sig_mask(int how, const sigset_t *neu, sigset_t *old) {
  uint64_t x, y, *mask;
  if (how == SIG_BLOCK || how == SIG_UNBLOCK || how == SIG_SETMASK) {
    if (__tls_enabled) {
      mask = &__get_tls()->tib_sigmask;
    } else {
      mask = &__sig.sigmask;
    }
    if (old) {
      old->__bits[0] = *mask;
      old->__bits[1] = 0;
    }
    if (neu) {
      x = *mask;
      y = neu->__bits[0];
      if (how == SIG_BLOCK) {
        x |= y;
      } else if (how == SIG_UNBLOCK) {
        x &= ~y;
      } else {
        x = y;
      }
      x &= ~(0
#define M(x) | GetSigBit(x)
#include "libc/intrin/sigisprecious.inc"
      );
      *mask = x;
    }
    return 0;
  } else {
    return einval();
  }
}
