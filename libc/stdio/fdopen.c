/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Allocates stream object for already-opened file descriptor.
 *
 * @param fd existing file descriptor or -1 for plain old buffer
 * @param mode is passed to fopenflags()
 * @return new stream or NULL w/ errno
 * @error ENOMEM
 */
FILE *fdopen(int fd, const char *mode) {
  FILE *res;
  if ((res = fmemopen(NULL, BUFSIZ, mode))) {
    res->fd = fd;
    res->reader = freadbuf;
    res->writer = fwritebuf;
    if ((res->iomode & O_ACCMODE) != O_RDONLY) {
      _fflushregister(res);
    }
  }
  return res;
}
