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
#include "libc/bits/xmmintrin.h"
#include "libc/crypto/rijndael.h"
#include "libc/nexgen32e/x86feature.h"

static void unrijndaelinit$westmere(struct Rijndael *ctx, uint32_t n,
                                    aes_block_t k1, aes_block_t k2) {
  uint32_t i;
  aes_block_t x;
  assert(n > 1);
  rijndaelinit(ctx, n, k1, k2);
  i = 1;
  do {
    x = ctx->rk[i].xmm;
    asm("aesimc\t%1,%0" : "=x"(x) : "0"(x));
    ctx->rk[i].xmm = x;
  } while (++i < n);
  XMM_DESTROY(x);
}

static relegated noinline void unrijndaelinit$pure(struct Rijndael *ctx,
                                                   uint32_t n, aes_block_t k1,
                                                   aes_block_t k2) {
  uint32_t i;
  aes_block_t x;
  assert(n > 1);
  rijndaelinit(ctx, n, k1, k2);
  i = 1;
  do {
    x = ctx->rk[i].xmm;
    x = InvMixColumns(x);
    ctx->rk[i].xmm = x;
  } while (++i < n);
  XMM_DESTROY(x);
}

/**
 * Computes key schedule for unrijndael().
 *
 * @param rk receives round keys
 * @param n is 14 for AES-256, 12 for AES-192, and 10 for AES-128
 * @param k1/k2 holds the master key
 */
void unrijndaelinit(struct Rijndael *ctx, uint32_t n, aes_block_t k1,
                    aes_block_t k2) {
  if (X86_HAVE(AES)) {
    return unrijndaelinit$westmere(ctx, n, k1, k2);
  } else {
    return unrijndaelinit$pure(ctx, n, k1, k2);
  }
}
