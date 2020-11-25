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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/internal.h"
#include "libc/errno.h"
#include "libc/rand/rand.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"

/**
 * Returns random bytes appropriate for random seeding.
 *
 * @param size should be the smallest value that meets your requirements
 * @param flags may be GRND_{RANDOM,NONBLOCK}
 * @return number of bytes copied on success; or -1 w/ errno, which
 *     indicates only that the request couldn't be serviced by the host
 *     kernel; this wrapper will still fill the buffer with random bytes
 *     from fallback sources no matter what
 */
ssize_t getrandom(void *buf, size_t size, unsigned flags) {
  ssize_t rc = getrandom$sysv(buf, size, flags);
  size_t i = rc == -1 ? 0 : (size_t)rc;
  if (i > size) abort();
  if (i < size) {
    unsigned char *p = buf;
    int olderr = errno;
    do {
      uint64_t i64 = rand64();
      memcpy(&p[i], &i64, min(sizeof(i64), size - i));
    } while ((i += sizeof(uint64_t)) < size);
    errno = olderr;
  }
  return rc;
}
