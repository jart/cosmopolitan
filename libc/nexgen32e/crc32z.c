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
#include "libc/bits/safemacros.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"

/**
 * Computes Phil Katz CRC-32 used by zip/zlib/gzip/etc.
 */
uint32_t crc32_z(uint32_t h, const void *data, size_t size) {
  const unsigned char *p, *pe;
  size_t skip;
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
