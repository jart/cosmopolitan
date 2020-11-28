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
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/limits.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/sysv/errfuns.h"

static size_t SumIovecLen(const struct iovec *v, size_t n) {
  size_t i, sum;
  for (sum = i = 0; i < n; ++i) {
    sum += v[i].iov_len;
  }
  return sum;
}

textwindows ssize_t write$nt(struct Fd *fd, const struct iovec *iov,
                             size_t iovlen, ssize_t opt_offset) {
  uint32_t wrote;
  struct NtOverlapped overlap;
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (WriteFile(fd->handle, iovlen ? iov[0].iov_base : NULL,
                iovlen ? clampio(iov[0].iov_len) : 0, &wrote,
                offset2overlap(opt_offset, &overlap))) {
    if (!wrote) assert(SumIovecLen(iov, iovlen) > 0);
    FlushFileBuffers(fd->handle);
    return wrote;
  } else {
    return __winerr();
  }
}
