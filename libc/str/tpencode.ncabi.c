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
#include "libc/str/internal.h"
#include "libc/str/tpenc.h"
#include "libc/str/tpencode.internal.h"

/* TODO: DELETE */

/**
 * Thompson-Pike Varint Encoder.
 *
 * Implementation Details: The header macro should ensure this function
 * is only called for non-ASCII, or DCE'd entirely. In addition to that
 * this function makes a promise to not clobber any registers but %rax.
 *
 * @param p is what ch gets encoded to
 * @param size is the number of bytes available in buf
 * @param ch is a 32-bit integer
 * @param awesome mode enables numbers the IETF unilaterally banned
 * @return number of bytes written
 * @note this encoding was designed on a napkin in a new jersey diner
 * @deprecated
 */
unsigned(tpencode)(char *p, size_t size, wint_t wc, bool32 awesome) {
  int i, j;
  unsigned long w;
  if ((0 <= wc && wc < 32) && awesome && size >= 2) {
    p[0] = 0xc0;
    p[1] = 0x80;
    p[1] |= wc;
    return 2;
  }
  i = 0;
  w = tpenc(wc);
  do {
    if (!size--) break;
    p[i++] = w & 0xff;
  } while (w >>= 8);
  return i;
}
