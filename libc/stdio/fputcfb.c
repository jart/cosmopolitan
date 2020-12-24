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
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

static noinline int slowpath(int c, FILE *f) {
  if (f->beg < f->size) {
    c &= 0xff;
    f->buf[f->beg++] = c;
    if (f->beg == f->size) {
      if (f->writer) {
        if (f->writer(f) == -1) return -1;
      } else if (f->beg == f->size) {
        f->beg = 0;
      }
    }
    return c;
  } else {
    return __fseteof(f);
  }
}

/**
 * Writes byte to stream.
 *
 * @return c (as unsigned char) if written or -1 w/ errno
 */
noinstrument int fputcfb(int c, FILE *f) {
  if (f->beg + 1 < f->size) {
    c &= 0xff;
    f->buf[f->beg++] = c;
    return c;
  } else {
    return slowpath(c, f);
  }
}
