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
#include "libc/crypto/rijndael.h"
#include "libc/dce.h"

static noinline aes_block_t xtime(aes_block_t x) {
  return ((x ^ (x & 0x80808080)) << 1) ^ (((x & 0x80808080) >> 7) * 0x1b);
}

static aes_block_t gf256mulx4(aes_block_t x, aes_block_t c) {
  return ((((c >> 0 & 0x01010101) * 0xff) & x) ^
          (((c >> 1 & 0x01010101) * 0xff) & xtime(x)) ^
          (((c >> 2 & 0x01010101) * 0xff) & xtime(xtime(x))) ^
          (((c >> 3 & 0x01010101) * 0xff) & xtime(xtime(xtime(x)))) ^
          (((c >> 4 & 0x01010101) * 0xff) & xtime(xtime(xtime(x)))));
}

/**
 * Applies inverse of Rijndael MixColumns() transformation.
 * @see FIPS-197
 */
aes_block_t InvMixColumns(aes_block_t x) {
  uint32_t i;
  aes_block_t y = {0, 0, 0, 0};
  aes_block_t c = {0x090D0B0E, 0x090D0B0E, 0x090D0B0E, 0x090D0B0E};
  for (i = 0; i < 4; ++i) {
    y ^= gf256mulx4((x & 0xff) * 0x01010101,
                    (((c >> 000) & 0xff) << 000 | ((c >> 010) & 0xff) << 030 |
                     ((c >> 020) & 0xff) << 020 | ((c >> 030) & 0xff) << 010));
    x = x << 8 | x >> 24;
    c = c << 8 | c >> 24;
  }
  return y;
}
