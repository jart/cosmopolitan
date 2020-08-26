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
#include "libc/unicode/unicode.h"

/**
 * Returns cell width of monospace character.
 */
int wcwidth(wchar_t ucs) {
  if (ucs == 0) return 0;
  if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0)) {
    return -1;
  } else if (0 <= ucs && ucs < kCombiningCharsBits &&
             !!(kCombiningChars[ucs >> 3] & (1 << (ucs & 7)))) {
    return 0;
  } else if (0 <= ucs && ucs < kEastAsianWidthBits) {
    return 1 + !!(kEastAsianWidth[ucs >> 3] & (1 << (ucs & 7)));
  } else {
    return 1;
  }
}
