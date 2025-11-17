/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  HACL*                                                                       │
│  Copyright (c) 2016-2022 INRIA, CMU and Microsoft Corporation                │
│  Copyright (c) 2022-2023 HACL* Contributors                                  │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/loop.h"
__static_yoink("hacl_star_notice");

static const uint64_t Hacl_Hash_SHA2_k384_512[80U] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
    0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
    0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
    0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
    0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
    0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
    0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
    0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
    0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
    0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
    0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

void
sha512_update(const uint8_t b[128], uint64_t hash[8])
{
    uint64_t hash_old[8U] = { 0U };
    uint64_t ws[16U] = { 0U };
    memcpy(hash_old, hash, 8U * sizeof(uint64_t));
    const uint8_t* b10 = b;
    uint64_t u = READ64BE(b10);
    ws[0U] = u;
    uint64_t u0 = READ64BE(b10 + 8U);
    ws[1U] = u0;
    uint64_t u1 = READ64BE(b10 + 16U);
    ws[2U] = u1;
    uint64_t u2 = READ64BE(b10 + 24U);
    ws[3U] = u2;
    uint64_t u3 = READ64BE(b10 + 32U);
    ws[4U] = u3;
    uint64_t u4 = READ64BE(b10 + 40U);
    ws[5U] = u4;
    uint64_t u5 = READ64BE(b10 + 48U);
    ws[6U] = u5;
    uint64_t u6 = READ64BE(b10 + 56U);
    ws[7U] = u6;
    uint64_t u7 = READ64BE(b10 + 64U);
    ws[8U] = u7;
    uint64_t u8 = READ64BE(b10 + 72U);
    ws[9U] = u8;
    uint64_t u9 = READ64BE(b10 + 80U);
    ws[10U] = u9;
    uint64_t u10 = READ64BE(b10 + 88U);
    ws[11U] = u10;
    uint64_t u11 = READ64BE(b10 + 96U);
    ws[12U] = u11;
    uint64_t u12 = READ64BE(b10 + 104U);
    ws[13U] = u12;
    uint64_t u13 = READ64BE(b10 + 112U);
    ws[14U] = u13;
    uint64_t u14 = READ64BE(b10 + 120U);
    ws[15U] = u14;
    KRML_MAYBE_FOR5(
      i0,
      0U,
      5U,
      1U,
      KRML_MAYBE_FOR16(
        i, 0U, 16U, 1U, uint64_t k_t = Hacl_Hash_SHA2_k384_512[16U * i0 + i];
        uint64_t ws_t = ws[i];
        uint64_t a0 = hash[0U];
        uint64_t b0 = hash[1U];
        uint64_t c0 = hash[2U];
        uint64_t d0 = hash[3U];
        uint64_t e0 = hash[4U];
        uint64_t f0 = hash[5U];
        uint64_t g0 = hash[6U];
        uint64_t h02 = hash[7U];
        uint64_t k_e_t = k_t;
        uint64_t t1 = h02 +
                      ((e0 << 50U | e0 >> 14U) ^
                       ((e0 << 46U | e0 >> 18U) ^ (e0 << 23U | e0 >> 41U))) +
                      ((e0 & f0) ^ (~e0 & g0)) + k_e_t + ws_t;
        uint64_t t2 = ((a0 << 36U | a0 >> 28U) ^
                       ((a0 << 30U | a0 >> 34U) ^ (a0 << 25U | a0 >> 39U))) +
                      ((a0 & b0) ^ ((a0 & c0) ^ (b0 & c0)));
        uint64_t a1 = t1 + t2;
        uint64_t b1 = a0;
        uint64_t c1 = b0;
        uint64_t d1 = c0;
        uint64_t e1 = d0 + t1;
        uint64_t f1 = e0;
        uint64_t g1 = f0;
        uint64_t h12 = g0;
        hash[0U] = a1;
        hash[1U] = b1;
        hash[2U] = c1;
        hash[3U] = d1;
        hash[4U] = e1;
        hash[5U] = f1;
        hash[6U] = g1;
        hash[7U] = h12;);
      if (i0 < 4U) {
          KRML_MAYBE_FOR16(i, 0U, 16U, 1U, uint64_t t16 = ws[i];
                           uint64_t t15 = ws[(i + 1U) % 16U];
                           uint64_t t7 = ws[(i + 9U) % 16U];
                           uint64_t t2 = ws[(i + 14U) % 16U];
                           uint64_t s1 = (t2 << 45U | t2 >> 19U) ^
                                         ((t2 << 3U | t2 >> 61U) ^ t2 >> 6U);
                           uint64_t s0 = (t15 << 63U | t15 >> 1U) ^
                                         ((t15 << 56U | t15 >> 8U) ^ t15 >> 7U);
                           ws[i] = s1 + t7 + s0 + t16;);
      });
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint64_t* os = hash;
                    uint64_t x = hash[i] + hash_old[i];
                    os[i] = x;);
}
