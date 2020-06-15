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
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * Appends at most 𝑛 bytes from 𝑠 to 𝑑.
 *
 * @param 𝑑 is both a NUL-terminated string and a buffer, needing
 *     an ARRAYLEN() of at least strlen(𝑑)+strnlen(𝑠,𝑛)+1
 * @param 𝑠 is character array which needn't be NUL-terminated
 * @param 𝑛 is maximum number of characters from s to copy
 * @return 𝑑
 * @note 𝑑 and 𝑠 can't overlap
 * @asyncsignaslenafe
 */
char *strncat(char *d, const char *s, size_t n) {
  size_t o;
  if (!memccpy(d + (o = strlen(d)), s, '\0', n)) {
    d[o + n] = '\0';
  }
  return d;
}
