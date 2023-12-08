/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/bsf.h"
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/unicode.h"

/**
 * Returns monospace display width of UTF-8 string.
 *
 * - Control codes are discounted
 * - ANSI escape sequences are discounted
 * - East asian glyphs, emoji, etc. count as two
 *
 * @param s is NUL-terminated string
 * @param n is max bytes to consider
 * @param o is offset for doing tabs
 * @return monospace display width
 */
int strnwidth(const char *s, size_t n, size_t o) {
  size_t i;
  wint_t c, w;
  unsigned l, r, k;
  enum { kAscii, kUtf8, kEsc, kCsi } t;
  for (w = r = t = l = i = 0; i < n;) {
    if ((c = s[i++] & 0xff)) {
      switch (t) {
        case kAscii:
          if (0x20 <= c && c <= 0x7E) {
            ++l;
          } else if (c == '\t') {
            if ((k = (o + i - 1) & 7)) {
              l += 8 - k;
            } else {
              l += 8;
            }
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
          __builtin_unreachable();
      }
    } else {
      break;
    }
  }
  return l;
}
