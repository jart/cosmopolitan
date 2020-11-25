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
#include "libc/bits/safemacros.internal.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"

static uint32_t kCrc32Tab[256];

/**
 * Computes Phil Katz CRC-32 used by zip/zlib/gzip/etc.
 *
 *     x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
 *     0b100000100110000010001110110110111
 *     bitreverse32(0x104c11db7)
 *
 * @param h is initial value
 */
uint32_t crc32_z(uint32_t h, const void *data, size_t size) {
  const unsigned char *p, *pe;
  static bool once;
  size_t skip;
  if (!once) {
    crc32init(kCrc32Tab, 0xedb88320);
    once = true;
  }
  if (data) {
    h ^= 0xffffffff;
    if (size >= 64 && X86_HAVE(PCLMUL)) {
      h = crc32$pclmul(h, data, size); /* 51x faster */
      skip = rounddown(size, 16);
    } else {
      skip = 0;
    }
    p = (const unsigned char *)data + skip;
    pe = (const unsigned char *)data + size;
    while (p < pe) {
      h = h >> 8 ^ kCrc32Tab[(h & 0xff) ^ *p++];
    }
    return h ^ 0xffffffff;
  } else {
    return 0;
  }
}
