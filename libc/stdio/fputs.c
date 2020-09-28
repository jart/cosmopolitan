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
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * Writes string to stream.
 *
 * Writing stops at the NUL-terminator, which isn't included in output.
 * This function blocks until the full string is written, unless an
 * unrecoverable error happens.
 *
 * @param s is a NUL-terminated string that's non-NULL
 * @param f is an open stream
 * @return strlen(s) or -1 w/ errno on error
 */
int fputs(const char *s, FILE *f) {
  int i, n, m;
  n = strlen(s);
  for (i = 0; i < n; ++i) {
    if (fputc(s[i], f) == -1) {
      if (ferror(f) == EINTR) continue;
      if (feof(f)) errno = f->state = EPIPE;
      return -1;
    }
  }
  return n;
}
