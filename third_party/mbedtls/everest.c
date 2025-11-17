/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/serialize.h"
#include "libc/serialize.h"
#include "third_party/mbedtls/endian.h"

#define DW(x)     (uint128_t)(x)
#define EQ(x, y)  ((((x ^ y) | (~(x ^ y) + 1)) >> 63) - 1)
#define GTE(x, y) (((x ^ ((x ^ y) | ((x - y) ^ y))) >> 63) - 1)

forceinline void BignumCopy(uint64_t o[5], uint64_t p[5]) {
  for (unsigned i = 0; i < 5; ++i) {
    o[i] = p[i];
  }
}

forceinline void BignumFsum(uint64_t o[5], uint64_t p[5]) {
  for (unsigned i = 0; i < 5; ++i) {
    o[i] += p[i];
  }
}

forceinline void BignumTrunc(uint64_t o[5], uint128_t p[5]) {
  for (unsigned i = 0; i < 5; ++i) {
    o[i] = p[i];
  }
}

forceinline void BignumCarry(uint64_t p[5]) {
  for (unsigned i = 0; i < 4; ++i) {
    p[i + 1] += p[i] >> 51;
    p[i] &= 0x7ffffffffffff;
  }
}

forceinline void BignumCarryWide(uint128_t p[5]) {
  for (unsigned i = 0; i < 4; ++i) {
    p[i + 1] += p[i] >> 51;
    p[i] &= 0x7ffffffffffff;
  }
}

static void BignumFmulReduce(uint128_t o[5], uint64_t p[5], uint64_t q[5]) {
  uint64_t t;
  unsigned i, j;
  for (i = 0;; ++i) {
    for (j = 0; j < 5; ++j) {
      o[j] += DW(p[j]) * q[i];
    }
    if (i == 4) break;
    t = p[4] * 19;
    p[4] = p[3];
    p[3] = p[2];
    p[2] = p[1];
    p[1] = p[0];
    p[0] = t;
  }
}

static void BignumFmul(uint64_t o[5], uint64_t p[5], uint64_t q[5]) {
  uint128_t t[5] = {0};
  uint64_t u[5] = {p[0], p[1], p[2], p[3], p[4]};
  BignumFmulReduce(t, u, q);
  BignumCarryWide(t);
  t[0] += DW(19) * (uint64_t)(t[4] >> 51);
  BignumTrunc(o, t);
  o[1] += o[0] >> 51;
  o[4] &= 0x7ffffffffffff;
  o[0] &= 0x7ffffffffffff;
}

static void BignumFsquare(uint128_t t[5], uint64_t p[5]) {
  t[0] = DW(p[0] * 1) * p[0] + DW(p[4] * 38) * p[1] + DW(p[2] * 38) * p[3];
  t[1] = DW(p[0] * 2) * p[1] + DW(p[4] * 38) * p[2] + DW(p[3] * 19) * p[3];
  t[2] = DW(p[0] * 2) * p[2] + DW(p[1] * 01) * p[1] + DW(p[4] * 38) * p[3];
  t[3] = DW(p[0] * 2) * p[3] + DW(p[1] * 02) * p[2] + DW(p[4]) * (p[4] * 19);
  t[4] = DW(p[0] * 2) * p[4] + DW(p[1] * 02) * p[3] + DW(p[2]) * p[2];
}

static void BignumFsqa(uint64_t o[5], uint32_t n) {
  uint128_t t[5];
  for (unsigned i = 0; i < n; ++i) {
    BignumFsquare(t, o);
    BignumCarryWide(t);
    t[0] += DW(19) * (uint64_t)(t[4] >> 51);
    BignumTrunc(o, t);
    o[1] += o[0] >> 51;
    o[4] &= 0x7ffffffffffff;
    o[0] &= 0x7ffffffffffff;
  }
}

static void BignumFsqr(uint64_t o[5], uint64_t p[5], uint32_t n) {
  BignumCopy(o, p);
  BignumFsqa(o, n);
}

static void BignumCrecip(uint64_t o[5], uint64_t z[5]) {
  uint64_t b[4][5];
  BignumFsqr(b[0], z, 1);
  BignumFsqr(b[1], b[0], 2);
  BignumFmul(b[2], b[1], z);
  BignumFmul(b[0], b[2], b[0]);
  BignumFsqr(b[1], b[0], 1);
  BignumFmul(b[2], b[1], b[2]);
  BignumFsqr(b[1], b[2], 5);
  BignumFmul(b[2], b[1], b[2]);
  BignumFsqr(b[1], b[2], 10);
  BignumFmul(b[3], b[1], b[2]);
  BignumFsqr(b[1], b[3], 20);
  BignumFmul(b[1], b[1], b[3]);
  BignumFsqa(b[1], 10);
  BignumFmul(b[2], b[1], b[2]);
  BignumFsqr(b[1], b[2], 50);
  BignumFmul(b[3], b[1], b[2]);
  BignumFsqr(b[1], b[3], 100);
  BignumFmul(b[1], b[1], b[3]);
  BignumFsqa(b[1], 50);
  BignumFmul(b[1], b[1], b[2]);
  BignumFsqa(b[1], 5);
  BignumFmul(o, b[1], b[0]);
}

static void BignumFdif(uint64_t a[5], uint64_t b[5]) {
  a[0] = b[0] + 0x3fffffffffff68 - a[0];
  a[1] = b[1] + 0x3ffffffffffff8 - a[1];
  a[2] = b[2] + 0x3ffffffffffff8 - a[2];
  a[3] = b[3] + 0x3ffffffffffff8 - a[3];
  a[4] = b[4] + 0x3ffffffffffff8 - a[4];
}

static void BignumFscalar(uint64_t o[5], uint64_t p[5], uint64_t s) {
  unsigned i;
  uint128_t t[5];
  for (i = 0; i < 5; ++i) t[i] = DW(p[i]) * s;
  BignumCarryWide(t);
  t[0] += DW(19) * (uint64_t)(t[4] >> 51);
  t[4] &= 0x7ffffffffffff;
  BignumTrunc(o, t);
}

static void EcPointSwap(uint64_t a[2][5], uint64_t b[2][5], uint64_t m) {
  unsigned i, j;
  uint64_t x, y;
  for (i = 0; i < 2; ++i) {
    for (j = 0; j < 5; ++j) {
      x = a[i][j] ^ (-m & (a[i][j] ^ b[i][j]));
      y = b[i][j] ^ (-m & (a[i][j] ^ b[i][j]));
      a[i][j] = x;
      b[i][j] = y;
    }
  }
}

static void EcFormatFexpand(uint64_t o[5], const uint8_t p[32]) {
  o[0] = READ64LE(p + 000) >> 00 & 0x7ffffffffffff;
  o[1] = READ64LE(p + 006) >> 03 & 0x7ffffffffffff;
  o[2] = READ64LE(p + 014) >> 06 & 0x7ffffffffffff;
  o[3] = READ64LE(p + 023) >> 01 & 0x7ffffffffffff;
  o[4] = READ64LE(p + 030) >> 12 & 0x7ffffffffffff;
}

static void EcFormatFcontract(uint8_t o[32], uint64_t p[5]) {
  uint64_t m;
  BignumCarry(p);
  p[0] += 19 * (p[4] >> 51);
  p[4] &= 0x7ffffffffffff;
  BignumCarry(p);
  p[0] += 19 * (p[4] >> 51);
  p[1] += p[0] >> 51;
  p[0] &= 0x7ffffffffffff;
  p[1] &= 0x7ffffffffffff;
  p[4] &= 0x7ffffffffffff;
  m = GTE(p[0], 0x7ffffffffffed);
  m &= EQ(p[1], 0x7ffffffffffff);
  m &= EQ(p[2], 0x7ffffffffffff);
  m &= EQ(p[3], 0x7ffffffffffff);
  m &= EQ(p[4], 0x7ffffffffffff);
  p[0] -= 0x7ffffffffffed & m;
  p[1] -= 0x7ffffffffffff & m;
  p[2] -= 0x7ffffffffffff & m;
  p[3] -= 0x7ffffffffffff & m;
  p[4] -= 0x7ffffffffffff & m;
  WRITE64LE(o + 000, p[1] << 51 | p[0] >> 00);
  WRITE64LE(o + 010, p[2] << 38 | p[1] >> 13);
  WRITE64LE(o + 020, p[3] << 25 | p[2] >> 26);
  WRITE64LE(o + 030, p[4] << 12 | p[3] >> 39);
}

static void EcFormatScalarOfPoint(uint8_t o[32], uint64_t p[2][5]) {
  uint64_t t[2][5];
  BignumCrecip(t[0], p[1]);
  BignumFmul(t[1], p[0], t[0]);
  EcFormatFcontract(o, t[1]);
}

static void EcAddAndDoubleFmonty(uint64_t xz2[2][5], uint64_t xz3[2][5],
                                 uint64_t xz[2][5], uint64_t xzprime[2][5],
                                 uint64_t qx[5]) {
  uint64_t b[7][5];
  BignumCopy(b[0], xz[0]);
  BignumFsum(xz[0], xz[1]);
  BignumFdif(xz[1], b[0]);
  BignumCopy(b[0], xzprime[0]);
  BignumFsum(xzprime[0], xzprime[1]);
  BignumFdif(xzprime[1], b[0]);
  BignumFmul(b[4], xzprime[0], xz[1]);
  BignumFmul(b[5], xz[0], xzprime[1]);
  BignumCopy(b[0], b[4]);
  BignumFsum(b[4], b[5]);
  BignumFdif(b[5], b[0]);
  BignumFsqr(xz3[0], b[4], 1);
  BignumFsqr(b[6], b[5], 1);
  BignumFmul(xz3[1], b[6], qx);
  BignumFsqr(b[2], xz[0], 1);
  BignumFsqr(b[3], xz[1], 1);
  BignumFmul(xz2[0], b[2], b[3]);
  BignumFdif(b[3], b[2]);
  BignumFscalar(b[1], b[3], 121665);
  BignumFsum(b[1], b[2]);
  BignumFmul(xz2[1], b[1], b[3]);
}

/**
 * Computes elliptic curve 25519.
 */
void curve25519(uint8_t mypublic[32], const uint8_t secret[32],
                const uint8_t basepoint[32]) {
  uint32_t i, j;
  uint8_t e[32], s;
  uint64_t q[5], t[4][2][5] = {{{1}}, {{0}, {1}}};
  EcFormatFexpand(q, basepoint);
  for (j = 0; j < 32; ++j) e[j] = secret[j];
  e[0] &= 248;
  e[31] = (e[31] & 127) | 64;
  BignumCopy(t[1][0], q);
  for (i = 32; i--;) {
    for (s = e[i], j = 4; j--;) {
      EcPointSwap(t[0], t[1], s >> 7);
      EcAddAndDoubleFmonty(t[2], t[3], t[0], t[1], q);
      EcPointSwap(t[2], t[3], s >> 7);
      s <<= 1;
      EcPointSwap(t[2], t[3], s >> 7);
      EcAddAndDoubleFmonty(t[0], t[1], t[2], t[3], q);
      EcPointSwap(t[0], t[1], s >> 7);
      s <<= 1;
    }
  }
  EcFormatScalarOfPoint(mypublic, t[0]);
}
