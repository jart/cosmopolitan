/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Google LLC                                                    │
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
#include "libc/str/blake2.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"

#define ROR(v, n) (((v) >> (n)) | ((v) << (64 - (n))))

asm(".ident\t\"\\n\\n\
boringssl blake2b (ISC License)\\n\
Copyright 2021 Google LLC\"");
asm(".include \"libc/disclaimer.inc\"");

// https://tools.ietf.org/html/rfc7693#section-2.6
static const uint64_t kIV[8] = {
    0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b,
    0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f,
    0x1f83d9abfb41bd6b, 0x5be0cd19137e2179,
};

// https://tools.ietf.org/html/rfc7693#section-2.7
static const uint8_t kSigma[10][16] = {
    // clang-format off
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
    // clang-format on
};

// https://tools.ietf.org/html/rfc7693#section-3.1
static void Blake2bMix(uint64_t v[16], int a, int b, int c, int d, uint64_t x,
                       uint64_t y) {
  v[a] = v[a] + v[b] + x;
  v[d] = ROR(v[d] ^ v[a], 32);
  v[c] = v[c] + v[d];
  v[b] = ROR(v[b] ^ v[c], 24);
  v[a] = v[a] + v[b] + y;
  v[d] = ROR(v[d] ^ v[a], 16);
  v[c] = v[c] + v[d];
  v[b] = ROR(v[b] ^ v[c], 63);
}

static void Blake2bTransform(
    struct Blake2b *b2b, const uint64_t w[BLAKE2B_CBLOCK / sizeof(uint64_t)],
    size_t num_bytes, int is_final_block) {
  // https://tools.ietf.org/html/rfc7693#section-3.2
  uint64_t v[16];
  _Static_assert(sizeof(v) == sizeof(b2b->h) + sizeof(kIV), "");
  memcpy(v, b2b->h, sizeof(b2b->h));
  memcpy(&v[8], kIV, sizeof(kIV));
  b2b->t_low += num_bytes;
  if (b2b->t_low < num_bytes) {
    b2b->t_high++;
  }
  v[12] ^= b2b->t_low;
  v[13] ^= b2b->t_high;
  if (is_final_block) {
    v[14] = ~v[14];
  }
  for (int i = 0; i < 12; i++) {
    Blake2bMix(v, 0, 4, 8, 12, w[kSigma[i % 10][0]], w[kSigma[i % 10][1]]);
    Blake2bMix(v, 1, 5, 9, 13, w[kSigma[i % 10][2]], w[kSigma[i % 10][3]]);
    Blake2bMix(v, 2, 6, 10, 14, w[kSigma[i % 10][4]], w[kSigma[i % 10][5]]);
    Blake2bMix(v, 3, 7, 11, 15, w[kSigma[i % 10][6]], w[kSigma[i % 10][7]]);
    Blake2bMix(v, 0, 5, 10, 15, w[kSigma[i % 10][8]], w[kSigma[i % 10][9]]);
    Blake2bMix(v, 1, 6, 11, 12, w[kSigma[i % 10][10]], w[kSigma[i % 10][11]]);
    Blake2bMix(v, 2, 7, 8, 13, w[kSigma[i % 10][12]], w[kSigma[i % 10][13]]);
    Blake2bMix(v, 3, 4, 9, 14, w[kSigma[i % 10][14]], w[kSigma[i % 10][15]]);
  }
  for (size_t i = 0; i < ARRAYLEN(b2b->h); i++) {
    b2b->h[i] ^= v[i];
    b2b->h[i] ^= v[i + 8];
  }
}

int BLAKE2B256_Init(struct Blake2b *b2b) {
  bzero(b2b, sizeof(struct Blake2b));
  _Static_assert(sizeof(kIV) == sizeof(b2b->h), "");
  memcpy(&b2b->h, kIV, sizeof(kIV));
  // https://tools.ietf.org/html/rfc7693#section-2.5
  b2b->h[0] ^= 0x01010000 | BLAKE2B256_DIGEST_LENGTH;
  return 0;
}

int BLAKE2B256_Process(struct Blake2b *b2b,
                       const uint64_t data[BLAKE2B_CBLOCK / 8]) {
  Blake2bTransform(b2b, data, BLAKE2B_CBLOCK, /*is_final_block=*/0);
  return 0;
}

int BLAKE2B256_Update(struct Blake2b *b2b, const void *in_data, size_t len) {
  const uint8_t *data = (const uint8_t *)in_data;
  size_t todo = sizeof(b2b->block.bytes) - b2b->block_used;
  if (todo > len) {
    todo = len;
  }
  if (todo) memcpy(&b2b->block.bytes[b2b->block_used], data, todo);
  b2b->block_used += todo;
  data += todo;
  len -= todo;
  if (!len) {
    return 0;
  }
  // More input remains therefore we must have filled |b2b->block|.
  unassert(b2b->block_used == BLAKE2B_CBLOCK);
  Blake2bTransform(b2b, b2b->block.words, BLAKE2B_CBLOCK,
                   /*is_final_block=*/0);
  b2b->block_used = 0;
  while (len > BLAKE2B_CBLOCK) {
    uint64_t block_words[BLAKE2B_CBLOCK / sizeof(uint64_t)];
    memcpy(block_words, data, sizeof(block_words));
    Blake2bTransform(b2b, block_words, BLAKE2B_CBLOCK, /*is_final_block=*/0);
    data += BLAKE2B_CBLOCK;
    len -= BLAKE2B_CBLOCK;
  }
  if (len) memcpy(b2b->block.bytes, data, len);
  b2b->block_used = len;
  return 0;
}

int BLAKE2B256_Final(struct Blake2b *b2b,
                     uint8_t out[BLAKE2B256_DIGEST_LENGTH]) {
  bzero(&b2b->block.bytes[b2b->block_used],
        sizeof(b2b->block.bytes) - b2b->block_used);
  Blake2bTransform(b2b, b2b->block.words, b2b->block_used,
                   /*is_final_block=*/1);
  _Static_assert(BLAKE2B256_DIGEST_LENGTH <= sizeof(b2b->h), "");
  memcpy(out, b2b->h, BLAKE2B256_DIGEST_LENGTH);
  return 0;
}

/**
 * Computes blake2b 256bit message digest.
 *
 *     blake2b256 n=0                     191 nanoseconds
 *     blake2b256 n=8                      23 ns/byte         40,719 kb/s
 *     blake2b256 n=31                      6 ns/byte            153 mb/s
 *     blake2b256 n=32                      6 ns/byte            158 mb/s
 *     blake2b256 n=63                      3 ns/byte            312 mb/s
 *     blake2b256 n=64                      3 ns/byte            317 mb/s
 *     blake2b256 n=128                     1 ns/byte            640 mb/s
 *     blake2b256 n=256                     1 ns/byte            662 mb/s
 *     blake2b256 n=22851                   1 ns/byte            683 mb/s
 *
 * @param data is binary memory to hash
 * @param len is bytes in `data`
 * @param out receives 32 byte binary digest
 * @return 0 on success (always successful)
 */
int BLAKE2B256(const void *data, size_t len,
               uint8_t out[BLAKE2B256_DIGEST_LENGTH]) {
  struct Blake2b ctx;
  BLAKE2B256_Init(&ctx);
  BLAKE2B256_Update(&ctx, data, len);
  BLAKE2B256_Final(&ctx, out);
  return 0;
}
