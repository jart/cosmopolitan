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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/xmmintrin.internal.h"
#include "libc/crypto/rijndael.h"
#include "libc/dce.h"
#include "libc/str/internal.h"

#define ROR(w, k) (CheckUnsigned(w) >> (k) | (w) << (sizeof(w) * 8 - (k)))

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
#define Nk   (n - 6)
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
