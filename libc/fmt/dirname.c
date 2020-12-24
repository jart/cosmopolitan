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
#include "libc/fmt/conv.h"
#include "libc/str/str.h"

#define ISDELIM(c) (c == '/' || c == '\\' || c == '.')

/**
 * Returns directory portion of path.
 * @param s is mutated
 */
char *dirname(char *s) {
  size_t i, n;
  if (!(n = strlen(s))) return s;
  while (n && ISDELIM(s[n - 1])) --n;
  if (n) {
    while (n && !ISDELIM(s[n - 1])) --n;
    if (n) {
      while (n && ISDELIM(s[n - 1])) --n;
      if (!n) ++n;
    } else {
      s[n++] = '.';
    }
  } else {
    ++n;
  }
  s[n] = '\0';
  return s;
}
