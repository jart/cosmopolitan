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
#include "libc/calls/struct/iovec.h"
#include "libc/macros.h"
#include "libc/sock/internal.h"
#include "libc/sysv/consts/iov.h"

/**
 * Converts I/O vector from System Five to WIN32 ABI.
 *
 * @return effective iovlen
 * @see IOV_MAX
 */
textwindows size_t iovec2nt(struct iovec$nt iovnt[hasatleast 16],
                            const struct iovec *iov, size_t iovlen) {
  size_t i, limit;
  for (limit = 0x7ffff000, i = 0; i < MIN(16, iovlen); ++i) {
    iovnt[i].buf = iov[i].iov_base;
    if (iov[i].iov_len < limit) {
      limit -= (iovnt[i].len = iov[i].iov_len);
    } else {
      iovnt[i].len = limit;
      break;
    }
  }
  return i;
}
