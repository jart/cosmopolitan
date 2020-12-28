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
#include "libc/bits/xmmintrin.internal.h"
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
