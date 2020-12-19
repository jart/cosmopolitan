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
#include "libc/str/str.h"

/**
 * Prepares static search buffer.
 *
 * 1. If SRC is too long, it's truncated and *not* NUL-terminated.
 * 2. If SRC is too short, the remainder is zero-filled.
 *
 * Please note this function isn't designed to prevent untrustworthy
 * data from modifying memory without authorization; the memccpy()
 * function can be used for that purpose.
 *
 * @return dest
 * @see stpncpy(), memccpy()
 * @asyncsignalsafe
 */
char *strncpy(char *dest, const char *src, size_t stride) {
  size_t i;
  for (i = 0; i < stride; ++i) {
    if (!(dest[i] = src[i])) break;
  }
  memset(dest + i, 0, stride - i);
  return dest;
}
