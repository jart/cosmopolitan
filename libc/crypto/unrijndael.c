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
#include "libc/bits/emmintrin.internal.h"
#include "libc/crypto/rijndael.h"
#include "libc/nexgen32e/x86feature.h"

forceinline aes_block_t unrijndael$westmere(uint32_t n, aes_block_t x,
                                            const struct Rijndael *ctx) {
  x ^= ctx->rk[n--].xmm;
  do {
    asm("aesdec\t%2,%0" : "=x"(x) : "0"(x), "m"(ctx->rk[n].xmm));
  } while (--n);
  asm("aesdeclast\t%2,%0" : "=x"(x) : "0"(x), "m"(ctx->rk[n].xmm));
  return x;
}

static noinline aes_block_t unrijndael$pure(uint32_t n, aes_block_t x,
                                            const struct Rijndael *ctx) {
  uint32_t j;
  __v16qu b1, b2;
  x ^= ctx->rk[n--].xmm;
  do {
    b2 = b1 = (__v16qu)x;
    for (j = 0; j < 16; ++j) {
      b2[j] = kAesSboxInverse[b1[j % 4 + j * 13 / 4 % 4 * 4]];
    }
    x = (aes_block_t)b2;
    if (n) x = InvMixColumns(x);
    x ^= ctx->rk[n].xmm;
  } while (n--);
  return x;
}

/**
 * Decrypts paragraph w/ AES.
 *
 * @param n is 14 for AES-256, 12 for AES-192, and 10 for AES-128
 * @param x is 128-bit chunk of ciphertext to decrypt
 * @param ctx was initialized by unrijndaelinit()
 * @return result of transformation
 */
aes_block_t unrijndael(uint32_t n, aes_block_t x, const struct Rijndael *ctx) {
  if (X86_HAVE(AES)) {
    return unrijndael$westmere(n, x, ctx);
  } else {
    return unrijndael$pure(n, x, ctx);
  }
}
