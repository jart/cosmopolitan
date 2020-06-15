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
#include "libc/errno.h"
#include "libc/str/str.h"

/**
 * Decodes UTF-16 character.
 *
 * @param s is a NUL-terminated string
 * @return number of bytes (NUL counts as 1) or -1 w/ errno
 * @note synchronization is performed to skip leading continuations;
 *     canonicalization and validation are performed to some extent
 */
unsigned(getutf16)(const char16_t *p, wint_t *wc) {
  unsigned skip = 0;
  while ((p[skip] & UTF16_MASK) == UTF16_CONT) skip++;
  if ((p[skip] & UTF16_MASK) != UTF16_MOAR) {
    *wc = p[skip];
    return skip + 1;
  } else if ((p[skip + 1] & UTF16_MASK) != UTF16_CONT) {
    *wc = INVALID_CODEPOINT;
    return -1;
  } else {
    *wc = 0x10000 + ((((unsigned)p[skip + 0] - 0xd800) << 10) +
                     ((unsigned)p[skip + 1] - 0xdc00));
    return skip + 2;
  }
}
