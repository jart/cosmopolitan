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
#include "libc/conv/conv.h"
#include "libc/str/str.h"

/**
 * Decodes 128-bit unsigned integer from ASCII string.
 *
 * This is a more restricted form of strtoimax() that's useful for folks
 * needing to decode numbers in the range [1^127, 1^128).
 */
uintmax_t strtoumax(const char *s, char **endptr, int base) {
  const unsigned char *p = (const unsigned char *)s;
  uintmax_t res = 0;

  while (isspace(*p)) {
    p++;
  }

  if (!base) {
    if (*p == '0') {
      p++;
      if (*p == 'x' || *p == 'X') {
        p++;
        base = 16;
      } else if (*p == 'b' || *p == 'B') {
        p++;
        base = 2;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  }

  for (;;) {
    unsigned diglet = kBase36[*p];
    if (!diglet || diglet > base) break;
    p++;
    res *= base;
    res += diglet - 1;
  }

  if (endptr) *endptr = (char *)p;
  return res;
}
