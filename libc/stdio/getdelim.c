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
#include "libc/mem/mem.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"

#define GETDELIM_MAX 65536

/**
 * Reads string from stream.
 *
 * @param line is the caller's buffer (in/out) which is extended
 *     automatically. *line may be NULL but only if *n is 0;
 *     NUL-termination is guaranteed FTMP
 * @param n is the capacity of line (in/out)
 * @param delim is the stop char (and NUL is implicitly too)
 * @return number of bytes read, including delim, excluding NUL, or -1
 *     w/ errno on EOF or error; see ferror() and feof()
 * @note this function can't punt EINTR to caller
 * @see getline(), gettok_r()
 */
ssize_t getdelim(char **line, size_t *n, int delim, FILE *f) {
  STATIC_YOINK("realloc");
  assert((*line && *n) || (!*line && !*n));
  ssize_t rc = -1;
  size_t i = 0;
  int c;
  for (;;) {
    if ((c = getc(f)) == -1) {
      if (ferror(f) == EINTR) continue;
      if (feof(f) && i) rc = i;
      break;
    }
    if (i + 2 >= *n && !__grow(line, n, 1, 0)) {
      __fseterrno(f);
      break;
    }
    if (((*line)[i++] = c) == delim) {
      rc = i;
      break;
    }
  }
  if (*line && i < *n) (*line)[i] = '\0';
  return rc;
}
