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
#include "libc/fmt/conv.h"

/**
 * Decodes a GNU-style varint from a buffer.
 *
 * The GNU Assembler is able to encode numbers this way, since it's used
 * by the DWARF debug format.
 */
int unsleb128(const void *buf, size_t size, int64_t *out) {
  int b;
  int64_t r, w;
  unsigned char c;
  const unsigned char *p, *pe;
  pe = (p = buf) + size;
  r = b = 0;
  do {
    if (size && p == pe) return -1;
    c = *p++;
    w = c & 0x7f;
    r |= w << b;
    b += 7;
  } while (c & 0x80);
  if (c & 0x40) r |= -1ull << b;
  if (out) *out = r;
  return p - (const unsigned char *)buf;
}
