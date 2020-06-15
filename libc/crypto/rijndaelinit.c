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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/xmmintrin.h"
#include "libc/crypto/rijndael.h"
#include "libc/dce.h"
#include "libc/str/internal.h"

static const uint8_t Rcon[11] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10,
                                 0x20, 0x40, 0x80, 0x1b, 0x36};

forceinline uint32_t SubRot(uint32_t t) {
  uint32_t j;
  for (j = 0; j < 4; j++) {
    t = (t & -256) | kAesSbox[t & 255];
    t = ROR(t, 8);
  }
  return t;
}

/**
 * Computes key schedule for rijndael().
 *
 * @param ctx receives round keys
 * @param n is 14 for AES-256, 12 for AES-192, and 10 for AES-128
 * @param k1/k2 holds the master key
 */
void rijndaelinit(struct Rijndael *ctx, uint32_t n, aes_block_t k1,
                  aes_block_t k2) {
#define Nk (n - 6)
#define W(i) (ctx->rk[(i) / 4].u32[(i) % 4])
#define K(i) ((i) < 4 ? k1[i] : k2[(i)-4])
  uint32_t i, t;
  ctx->rk[0].xmm = k1;
  ctx->rk[1].xmm = k2;
  for (i = Nk; i < 4 * (n + 1); ++i) {
    t = W(i - 1);
    if (i % Nk == 0) {
      t = ROR(t, 8);
      t = SubRot(t);
      t ^= Rcon[i / Nk];
    } else if (Nk > 6 && i % Nk == 4) {
      t = SubRot(t);
    }
    W(i) = W(i - Nk) ^ t;
  }
  XMM_DESTROY(k1);
  XMM_DESTROY(k2);
}
