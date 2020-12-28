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
