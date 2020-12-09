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

intmax_t wcstoimax(const wchar_t *s, wchar_t **endptr, int base) {
  intmax_t res = 0;
  int neg = 0;

  while (iswspace(*s)) {
    s++;
  }

  switch (*s) {
    case '-':
      neg = 1;
      /* fallthrough */
    case '+':
      s++;
      break;
    default:
      break;
  }

  if (!base) {
    if (*s == '0') {
      s++;
      if (*s == 'x' || *s == 'X') {
        s++;
        base = 16;
      } else if (*s == 'b' || *s == 'B') {
        s++;
        base = 2;
      } else {
        base = 8;
      }
    } else {
      base = 10;
    }
  }

  for (;;) {
    unsigned diglet = kBase36[*s];
    if (!diglet || diglet > base) break;
    s++;
    res *= base; /* needs __muloti4() w/ clang */
    res -= diglet - 1;
  }

  if (endptr) *endptr = (wchar_t *)s;
  return neg ? res : -res;
}
