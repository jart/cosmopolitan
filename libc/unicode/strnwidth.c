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
#include "libc/str/thompike.h"
#include "libc/unicode/unicode.h"

/**
 * Returns monospace display width of UTF-8 string.
 *
 * - Control codes are discounted
 *
 * - ANSI escape sequences are discounted
 *
 * - East asian glyphs, emoji, etc. count as two
 *
 * @param s is NUL-terminated string
 * @param n is max bytes to consider
 * @return monospace display width
 */
int strnwidth(const char *s, size_t n) {
  wint_t c, w;
  unsigned l, r;
  enum { kAscii, kUtf8, kEsc, kCsi } t;
  for (w = r = t = l = 0; n--;) {
    if ((c = *s++ & 0xff)) {
      switch (t) {
        case kAscii:
          if (0x20 <= c && c <= 0x7E || c == '\t') {
            ++l;
          } else if (c == 033) {
            t = kEsc;
          } else if (c >= 0300) {
            t = kUtf8;
            w = ThomPikeByte(c);
            r = ThomPikeLen(c) - 1;
          }
          break;
        case kUtf8:
          if (ThomPikeCont(c)) {
            w = ThomPikeMerge(w, c);
            if (--r) break;
          }
          l += MAX(0, wcwidth(w));
          t = kAscii;
          break;
        case kEsc:
          if (c == '[') {
            t = kCsi;
          } else if (!(040 <= c && c < 060)) {
            t = kAscii;
          }
          break;
        case kCsi:
          if (!(060 <= c && c < 0100)) {
            t = kAscii;
          }
          break;
        default:
          unreachable;
      }
    } else {
      break;
    }
  }
  return l;
}
