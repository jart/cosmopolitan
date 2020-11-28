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
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/o.h"

/**
 * Repositions open file stream.
 *
 * This function flushes the buffer (unless it's currently in the EOF
 * state) and then calls lseek() on the underlying file. If the stream
 * is in the EOF state, this function can be used to restore it without
 * needing to reopen the file.
 *
 * @param f is a non-null stream handle
 * @param offset is the byte delta
 * @param whence can be SEET_SET, SEEK_CUR, or SEEK_END
 * @returns new offset or -1 on error
 */
long fseek(FILE *f, long offset, int whence) {
  int skew;
  int64_t newpos;
  if (f->fd != -1) {
    if (whence == SEEK_CUR && f->beg < f->end) {
      offset -= f->end - f->beg;
    }
    if (f->beg && !f->end) {
      f->writer(f);
    }
    if ((newpos = lseek(f->fd, offset, whence)) != -1) {
      f->state = 0;
      f->beg = 0;
      f->end = 0;
      return newpos;
    } else {
      f->state = errno == ESPIPE ? EBADF : errno;
      return -1;
    }
  } else {
    f->beg = offset % f->size;
    return -1;
  }
}
