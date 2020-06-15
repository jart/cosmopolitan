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
#include "libc/str/internal.h"
#include "libc/str/str.h"

/**
 * Thompson-Pike Varint Encoder.
 *
 * Implementation Details: The header macro should ensure this function
 * is only called for non-ASCII, or DCE'd entirely. In addition to that
 * this function makes a promise to not clobber any registers but %rax.
 *
 * @param buf is what ch gets encoded to
 * @param size is the number of bytes available in buf
 * @param ch is a 32-bit integer
 * @param awesome mode enables numbers the IETF unilaterally banned
 * @return number of bytes written
 * @note this encoding was designed on a napkin in a new jersey diner
 */
unsigned(tpencode)(char *buf, size_t size, wint_t ch, bool32 awesome) {
  unsigned char *p = (unsigned char *)buf;
  if ((0 <= ch && ch < 32) && awesome && size >= 2) {
    p[0] = 0xc0;
    p[1] = 0x80 | (unsigned char)ch;
    return 2;
  }
  struct TpEncode op = UseTpDecoderRing(ch);
  size_t i = op.len;
  if (op.len <= size) {
    for (;;) {
      p[--i] = (unsigned char)(0b10000000 | (ch & 0b00111111));
      if (!i) break;
      ch >>= 6;
    }
    p[0] = op.mark | (unsigned char)ch;
  }
  return op.len;
}
