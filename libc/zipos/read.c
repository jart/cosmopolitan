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
#include "libc/assert.h"
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/str/str.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Reads data from zip store object.
 *
 * @return [1..size] bytes on success, 0 on EOF, or -1 w/ errno; with
 *     exception of size==0, in which case return zero means no error
 * @asyncsignalsafe
 */
ssize_t __zipos_read(struct ZiposHandle *h, const struct iovec *iov,
                     size_t iovlen, ssize_t opt_offset) {
  size_t i, b, x, y;
  x = y = opt_offset != -1 ? opt_offset : h->pos;
  for (i = 0; i < iovlen && y < h->size; ++i, y += b) {
    b = min(iov[i].iov_len, h->size - y);
    memcpy(iov[i].iov_base, h->mem + y, b);
  }
  if (opt_offset == -1) h->pos = y;
  return y - x;
}
