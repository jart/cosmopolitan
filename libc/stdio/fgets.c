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
 * Reads content from stream.
 *
 * This function is similar to getline() except it'll truncate lines
 * exceeding size. The line ending marker is included and may be removed
 * using chomp().
 */
char *fgets(char *s, int size, FILE *f) {
  int c;
  char *p;
  p = s;
  if (size > 0) {
    while (--size > 0) {
      if ((c = getc(f)) == -1) {
        if (ferror(f) == EINTR) continue;
        break;
      }
      *p++ = c & 0xff;
      if (c == '\n') break;
    }
    *p = '\0';
  }
  return p > s ? s : NULL;
}
