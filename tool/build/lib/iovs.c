/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/str/str.h"
#include "tool/build/lib/iovs.h"

/**
 * Appends memory region to i/o vector builder.
 */
int AppendIovs(struct Iovs *ib, void *base, size_t len) {
  unsigned i, n;
  struct iovec *p;
  if (len) {
    p = ib->p;
    i = ib->i;
    n = ib->n;
    if (i && (intptr_t)base == (intptr_t)p[i - 1].iov_base + p[i - 1].iov_len) {
      p[i - 1].iov_len += len;
    } else {
      if (__builtin_expect(i == n, 0)) {
        n += n >> 1;
        if (p == ib->init) {
          if (!(p = malloc(sizeof(struct iovec) * n))) return -1;
          memcpy(p, ib->init, sizeof(ib->init));
        } else {
          if (!(p = realloc(p, sizeof(struct iovec) * n))) return -1;
        }
        ib->p = p;
        ib->n = n;
      }
      p[i].iov_base = base;
      p[i].iov_len = len;
      ++ib->i;
    }
  }
  return 0;
}
