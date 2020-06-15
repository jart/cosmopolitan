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
#include "libc/stdio/stdio.h"

/**
 * Reads line from stream.
 *
 * This function delegates to getdelim(), which provides further
 * documentation. Concerning lines, please note the \n or \r\n are
 * included in results, and can be removed with chomp().
 *
 * @param line is the caller's buffer (in/out) which is extended
 *     automatically. *line may be NULL but only if *n is 0;
 *     NUL-termination is guaranteed FTMP
 * @return number of bytes read, including delim, excluding NUL, or -1
 *     w/ errno on EOF or error; see ferror() and feof()
 * @see xgetline(), getdelim(), gettok_r()
 */
ssize_t getline(char **line, size_t *n, FILE *f) {
  return getdelim(line, n, '\n', f);
}
