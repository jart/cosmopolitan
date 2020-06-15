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
#include "libc/mem/mem.h"

/**
 * Formats string, allocating needed memory.
 *
 * @param *strp is output-only and must be free'd, even on error; this
 *     behavior was adopted to help put your needs first in terms of
 *     portability, since that's guaranteed to work with all libraries
 * @return bytes written (excluding NUL) or -1 w/ errno
 * @see xasprintf() for a better API
 */
int(asprintf)(char **strp, const char *fmt, ...) {
  int res;
  va_list va;
  va_start(va, fmt);
  res = (vasprintf)(strp, fmt, va);
  va_end(va);
  return res;
}
