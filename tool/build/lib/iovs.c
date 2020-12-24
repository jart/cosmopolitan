/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
