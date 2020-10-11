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
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"

/**
 * Transcodes UTF-8 to UTF-16.
 *
 * @param dst is output buffer
 * @param dstsize is shorts in dst
 * @param src is NUL-terminated UTF-8 input string
 * @return number of shorts written excluding NUL
 */
size_t tprecode8to16(char16_t *dst, size_t dstsize, const char *src) {
  size_t i;
  unsigned n;
  uint64_t w;
  wint_t x, y;
  i = 0;
  if (dstsize) {
    for (;;) {
      if (!(x = *src++ & 0xff)) break;
      if (ThomPikeCont(x)) continue;
      if (!isascii(x)) {
        n = ThomPikeLen(x);
        x = ThomPikeByte(x);
        while (--n) {
          if (!(y = *src++ & 0xff)) goto stop;
          x = ThomPikeMerge(x, y);
        }
      }
      w = EncodeUtf16(x);
      while (w && i + 1 < dstsize) {
        dst[i++] = w & 0xFFFF;
        w >>= 16;
      }
    }
  stop:
    dst[i] = 0;
  }
  return i;
}
