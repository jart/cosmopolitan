/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/likely.h"
#include "libc/limits.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/tls.h"
#include "libc/zip.internal.h"

static ssize_t __zipos_read_impl(struct ZiposHandle *h, const struct iovec *iov,
                                 size_t iovlen, ssize_t opt_offset) {
  int i;
  int64_t b, x, y, start_pos;
  if (h->cfile == ZIPOS_SYNTHETIC_DIRECTORY ||
      S_ISDIR(GetZipCfileMode(h->zipos->map + h->cfile))) {
    return eisdir();
  }
  if (opt_offset == -1) {
  Restart:
    start_pos = atomic_load_explicit(&h->pos, memory_order_relaxed);
    do {
      if (UNLIKELY(start_pos == SIZE_MAX)) {
        goto Restart;
      }
    } while (!LIKELY(atomic_compare_exchange_weak_explicit(
        &h->pos, &start_pos, SIZE_MAX, memory_order_acquire,
        memory_order_relaxed)));
    x = y = start_pos;
  } else {
    x = y = opt_offset;
  }
  for (i = 0; i < iovlen && y < h->size; ++i, y += b) {
    b = MIN(iov[i].iov_len, h->size - y);
    if (b) memcpy(iov[i].iov_base, h->mem + y, b);
  }
  if (opt_offset == -1) {
    unassert(y != SIZE_MAX);
    atomic_store_explicit(&h->pos, y, memory_order_release);
  }
  return y - x;
}

/**
 * Reads data from zip store object.
 *
 * @return [1..size] bytes on success, 0 on EOF, or -1 w/ errno; with
 *     exception of size==0, in which case return zero means no error
 * @asyncsignalsafe
 */
ssize_t __zipos_read(struct ZiposHandle *h, const struct iovec *iov,
                     size_t iovlen, ssize_t opt_offset) {
  unassert(opt_offset >= 0 || opt_offset == -1);
  return __zipos_read_impl(h, iov, iovlen, opt_offset);
}
