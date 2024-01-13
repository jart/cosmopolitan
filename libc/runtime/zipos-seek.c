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
#include "libc/calls/calls.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/likely.h"
#include "libc/limits.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdckdint.h"
#include "libc/sysv/errfuns.h"

static int64_t Seek(int64_t pos, int64_t offset) {
  int64_t rc;
  if (!ckd_add(&rc, pos, offset)) {
    if (rc >= 0) {
      return rc;
    } else {
      return einval();
    }
  } else {
    return eoverflow();
  }
}

/**
 * Changes current position of zip file handle.
 *
 * @param offset is the relative byte count
 * @param whence can be SEEK_SET, SEEK_CUR, or SEEK_END
 * @return new position relative to beginning, or -1 on error
 * @asyncsignalsafe
 */
int64_t __zipos_seek(struct ZiposHandle *h, int64_t offset, unsigned whence) {
  int64_t pos, new_pos;
Restart:
  pos = atomic_load_explicit(&h->pos, memory_order_relaxed);
  do {
    if (UNLIKELY(pos == SIZE_MAX)) {
      goto Restart;
    }
    switch (whence) {
      case SEEK_SET:
        new_pos = Seek(0, offset);
        break;
      case SEEK_CUR:
        new_pos = Seek(pos, offset);
        break;
      case SEEK_END:
        new_pos = Seek(h->size, offset);
        break;
      default:
        new_pos = einval();
    }
  } while (new_pos >= 0 && !LIKELY(atomic_compare_exchange_weak_explicit(
                               &h->pos, &pos, new_pos, memory_order_release,
                               memory_order_relaxed)));
  return new_pos;
}
