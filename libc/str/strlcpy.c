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
#include "libc/bits/safemacros.internal.h"
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * Copies string, the BSD way.
 *
 * @param d is buffer which needn't be initialized
 * @param s is a NUL-terminated string
 * @param n is byte capacity of d
 * @return strlen(s)
 * @note d and s can't overlap
 * @note we prefer memccpy()
 */
size_t strlcpy(char *d, const char *s, size_t n) {
  size_t slen, actual;
  slen = strlen(s);
  if (n) {
    actual = MIN(n, slen);
    memcpy(d, s, actual);
    d[actual] = '\0';
  }
  return slen;
}
