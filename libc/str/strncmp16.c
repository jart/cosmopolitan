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
 * Compares NUL-terminated UCS-2 strings w/ limit.
 *
 * @param a is first non-null NUL-terminated char16 string pointer
 * @param b is second non-null NUL-terminated char16 string pointer
 * @return is <0, 0, or >0 based on uint8_t comparison
 * @asyncsignalsafe
 */
int strncmp16(const char16_t *a, const char16_t *b, size_t n) {
  size_t i = 0;
  if (!n-- || a == b) return 0;
  while (i < n && a[i] == b[i] && b[i]) ++i;
  return a[i] - b[i];
}
