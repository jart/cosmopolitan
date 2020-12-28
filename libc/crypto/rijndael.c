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
#include "libc/bits/emmintrin.internal.h"
#include "libc/crypto/rijndael.h"
#include "libc/nexgen32e/x86feature.h"

forceinline aes_block_t rijndael$westmere(uint32_t n, aes_block_t x,
                                          const struct Rijndael *ctx) {
  uint32_t i;
  x ^= ctx->rk[0].xmm;
  for (i = 1; i < n; ++i) {
    asm("aesenc\t%2,%0" : "=x"(x) : "0"(x), "m"(ctx->rk[i].xmm));
  }
  asm("aesenclast\t%2,%0" : "=x"(x) : "0"(x), "m"(ctx->rk[i].xmm));
  return x;
}

static noinline aes_block_t rijndael$pure(uint32_t n, aes_block_t x,
                                          const struct Rijndael *ctx) {
  uint32_t i, j;
  __v16qu b1, b2;
  aes_block_t u1, u2, u3, u4;
  x ^= ctx->rk[0].xmm;
  for (i = 1; i < n + 1; ++i) {
    b2 = b1 = (__v16qu)x;
    for (j = 0; j < 16; ++j) {
      b2[j % 4 + 13 * j / 4 % 4 * 4] = kAesSbox[b1[j]];
    }
    u1 = (aes_block_t)b2;
    if (i != n) {
      u2 = u1 >> 010 | u1 << 030;
      u3 = u1 ^ u2;
      u4 = u3 & 0x80808080;
      u3 = ((u3 ^ u4) << 1) ^ ((u4 >> 7) * 0x1b);
      u1 = u3 ^ u2 ^ (u1 >> 020 | u1 << 020) ^ (u1 >> 030 | u1 << 010);
    }
    x = ctx->rk[i].xmm ^ u1;
  }
  return x;
}

/**
 * Encrypts paragraph w/ AES.
 *
 * @param n is 14 for AES-256, 12 for AES-192, and 10 for AES-128
 * @param x is 128-bit chunk of plaintext to encrypt
 * @param ctx was initialized by rijndaelinit()
 * @return result of transformation
 */
aes_block_t rijndael(uint32_t n, aes_block_t x, const struct Rijndael *ctx) {
  if (X86_HAVE(AES)) {
    return rijndael$westmere(n, x, ctx);
  } else {
    return rijndael$pure(n, x, ctx);
  }
}
