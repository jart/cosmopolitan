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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/intrin/spinlock.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"

/**
 * @fileoverview UNIX signals for the New Technology, Part 1.
 * @threadsafe
 */

struct Signals __sig;  // TODO(jart): Need TLS

/**
 * Changes signal mask for main thread.
 * @return 0 on success, or -1 w/ errno
 */
textwindows int __sig_mask(int how, const sigset_t *neu, sigset_t *old) {
  int i;
  uint64_t a, b;
  if (how == SIG_BLOCK || how == SIG_UNBLOCK || how == SIG_SETMASK) {
    cthread_spinlock(&__sig_lock);
    if (old) {
      *old = __sig.mask;
    }
    if (neu) {
      for (i = 0; i < ARRAYLEN(__sig.mask.__bits); ++i) {
        if (how == SIG_BLOCK) {
          __sig.mask.__bits[i] |= neu->__bits[i];
        } else if (how == SIG_UNBLOCK) {
          __sig.mask.__bits[i] &= ~neu->__bits[i];
        } else {
          __sig.mask.__bits[i] = neu->__bits[i];
        }
      }
      __sig.mask.__bits[0] &= ~(SIGKILL | SIGSTOP);
    }
    cthread_spunlock(&__sig_lock);
    return 0;
  } else {
    return einval();
  }
}
