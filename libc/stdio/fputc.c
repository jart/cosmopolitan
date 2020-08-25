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
#include "libc/calls/calls.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

/**
 * Writes byte to stream.
 *
 * @return c (as unsigned char) if written or -1 w/ errno
 */
noinstrument int fputc(int c, FILE *f) {
  if (c != -1) {
    c &= 0xff;
    f->buf[f->end] = c;
    f->end = (f->end + 1) & (f->size - 1);
    if (unlikely(f->beg == f->end || f->bufmode == _IONBF ||
                 (f->bufmode == _IOLBF && c == '\n'))) {
      if (f->writer) {
        return f->writer(f);
      } else if (f->beg == f->end) {
        return fseteof(f);
      }
    }
    return c;
  } else {
    return fseteof(f);
  }
}
