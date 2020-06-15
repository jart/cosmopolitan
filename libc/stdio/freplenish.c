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
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

/**
 * Fills empty space in buffer with whatever's available.
 *
 * This can be used alongside functions like select() or poll() to
 * perform reads at opportune moments, thereby minimizing latency.
 */
int freplenish(FILE *f) {
  ssize_t rc;
  size_t got;
  struct iovec iov[2];

  if (f->beg == f->end) {
    f->beg = f->end = 0;
  }

  if (f->beg <= f->end) {
    if (f->beg) {
      iov[0].iov_base = f->buf + f->end;
      iov[0].iov_len = f->size - f->end;
      iov[1].iov_base = f->buf;
      iov[1].iov_len = f->beg - 1;
      rc = readv(f->fd, iov, 2);
    } else {
      rc = read(f->fd, f->buf, f->size - (f->end - f->beg) - 1);
    }
  } else {
    if (f->end + 1 == f->beg) return 0;
    rc = read(f->fd, f->buf + f->end, f->beg - f->end - 1);
  }

  if (rc != -1) {
    if (rc) {
      got = rc;
      f->end = (f->end + got) & (f->size - 1);
      return got;
    } else {
      return fseteof(f);
    }
  } else if (errno == EINTR || errno == EAGAIN) {
    return 0;
  } else {
    return fseterrno(f);
  }
}
