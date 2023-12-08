/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bsf.h"
#include "libc/intrin/cxaatexit.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"

/**
 * Triggers global destructors.
 *
 * They're called in LIFO order. If a destructor adds more destructors,
 * then those destructors will be called immediately following, before
 * iteration continues.
 *
 * @param pred can be null to match all
 */
void __cxa_finalize(void *pred) {
  void *fp, *arg;
  unsigned i, mask;
  struct CxaAtexitBlock *b, *b2;
StartOver:
  __cxa_lock();
StartOverLocked:
  if ((b = __cxa_blocks.p)) {
    for (;;) {
      mask = b->mask;
      while (mask) {
        i = _bsf(mask);
        mask &= ~(1u << i);
        if (!pred || pred == b->p[i].pred) {
          b->mask &= ~(1u << i);
          if ((fp = b->p[i].fp)) {
            arg = b->p[i].arg;
            __cxa_unlock();
            STRACE("__cxa_finalize(%t, %p)", fp, arg);
            ((void (*)(void *))fp)(arg);
            goto StartOver;
          }
        }
      }
      if (!pred) {
        b2 = b->next;
        if (b2) {
          if (_weaken(free)) {
            _weaken(free)(b);
          }
        }
        __cxa_blocks.p = b2;
        goto StartOverLocked;
      } else {
        if (b->next) {
          b = b->next;
        } else {
          break;
        }
      }
    }
  }
  __cxa_unlock();
}
