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
#include "libc/errno.h"
#include "libc/stdio/stdio.h"

/**
 * Reads UTF-8 content from stream into UTF-32 buffer.
 */
wchar_t *fgetws(wchar_t *s, int size, FILE *f) {
  wchar_t *p = s;
  if (size > 0) {
    while (--size > 0) {
      wint_t c;
      if ((c = fgetwc(f)) == -1) {
        if (ferror(f) == EINTR) continue;
        break;
      }
      *p++ = c;
      if (c == '\n') break;
    }
    *p = '\0';
  }
  return (intptr_t)p > (intptr_t)s ? s : NULL;
}
