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
 * data from modifying memory without authorization. Consider trying
 * memccpy() for that purpose.
 *
 * @return dest + stride
 * @see stncpy(), memccpy()
 * @asyncsignalsafe
 */
char *stpncpy(char *dest, const char *src, size_t stride) {
  char *p;
  if ((p = memccpy(dest, src, '\0', stride))) {
    memset(p, 0, dest + stride - p);
  }
  return dest + stride;
}
