/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/unicode/unicode.h"

extern const uint8_t kEastAsianWidth[];
extern const uint32_t kEastAsianWidthBits;
extern const uint8_t kCombiningChars[];
extern const uint32_t kCombiningCharsBits;

/**
 * Returns cell width of monospace character.
 */
int wcwidth(wchar_t ucs) {
  if (ucs == 0) return 0;
  if ((0 <= ucs && ucs < 32) || (0x7f <= ucs && ucs < 0xA0)) {
    return -1;
  } else if ((0 <= ucs && ucs < kCombiningCharsBits) &&
             !!(kCombiningChars[ucs >> 3] & (1 << (ucs & 7)))) {
    return 0;
  } else if (0 <= ucs && ucs < kEastAsianWidthBits) {
    return 1 + !!(kEastAsianWidth[ucs >> 3] & (1 << (ucs & 7)));
  } else {
    return 1;
  }
}
