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
#include "libc/bits/safemacros.h"
#include "libc/str/str.h"

/**
 * Appends string SRC to DEST, the BSD way.
 *
 * @param dest is a buffer holding a NUL-terminated string
 * @param src is a NUL-terminated string
 * @param size is byte capacity of dest
 * @return strlen(dest) + strlen(src)
 * @note dest and src can't overlap
 * @see strncat()
 */
size_t strlcat(char *dest, const char *src, size_t size) {
  size_t destlen = strnlen(dest, size);
  size_t srclen = strlen(src);
  if (size) {
    memcpy(&dest[destlen], src, min(srclen, size - destlen));
    dest[min(destlen + srclen, size - 1)] = '\0';
  }
  return destlen + srclen;
}
