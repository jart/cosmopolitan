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
#include "libc/fmt/fmt.h"
#include "libc/limits.h"
#include "libc/str/str.h"

size_t mbsrtowcs(wchar_t *dest, const char **src, size_t len, mbstate_t *ps) {
  /* TODO(jart): Need to fix. */
  const unsigned char *s = (const void *)*src;
  size_t wn0 = len;
  unsigned c = 0;
  if (!dest) return strlen((const char *)s);
  for (;;) {
    if (!len) {
      *src = (const void *)s;
      return wn0;
    }
    if (!*s) break;
    c = *s++;
    *dest++ = c;
    len--;
  }
  *dest = 0;
  *src = 0;
  return wn0 - len;
}
