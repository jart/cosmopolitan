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
#include "libc/nexgen32e/x86feature.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/loop.h"
__static_yoink("hacl_star_notice");

const uint32_t Hacl_Hash_SHA2_k224_256[64U] = {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU,
    0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U, 0xd807aa98U, 0x12835b01U,
    0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U,
    0xc19bf174U, 0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU,
    0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU, 0x983e5152U,
    0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U,
    0x06ca6351U, 0x14292967U, 0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU,
    0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U,
    0xd6990624U, 0xf40e3585U, 0x106aa070U, 0x19a4c116U, 0x1e376c08U,
    0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU,
    0x682e6ff3U, 0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U,
    0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
};

void
sha256_update(const uint8_t b[64], uint32_t hash[8])
{
#ifdef __x86_64__
    /* [jart] add runtime dispatching to assembly */
    void sha256_update_x86_64(
      uint32_t[8], const uint8_t[64], uint64_t, const uint32_t[64]);
    if (X86_HAVE(SHA))
        return sha256_update_x86_64(hash, b, 1, Hacl_Hash_SHA2_k224_256);
#endif

    uint32_t hash_old[8U] = { 0U };
    uint32_t ws[16U] = { 0U };
    memcpy(hash_old, hash, 8U * sizeof(uint32_t));
    const uint8_t* b10 = b;
    uint32_t u = READ32BE(b10);
    ws[0U] = u;
    uint32_t u0 = READ32BE(b10 + 4U);
    ws[1U] = u0;
    uint32_t u1 = READ32BE(b10 + 8U);
    ws[2U] = u1;
    uint32_t u2 = READ32BE(b10 + 12U);
    ws[3U] = u2;
    uint32_t u3 = READ32BE(b10 + 16U);
    ws[4U] = u3;
    uint32_t u4 = READ32BE(b10 + 20U);
    ws[5U] = u4;
    uint32_t u5 = READ32BE(b10 + 24U);
    ws[6U] = u5;
    uint32_t u6 = READ32BE(b10 + 28U);
    ws[7U] = u6;
    uint32_t u7 = READ32BE(b10 + 32U);
    ws[8U] = u7;
    uint32_t u8 = READ32BE(b10 + 36U);
    ws[9U] = u8;
    uint32_t u9 = READ32BE(b10 + 40U);
    ws[10U] = u9;
    uint32_t u10 = READ32BE(b10 + 44U);
    ws[11U] = u10;
    uint32_t u11 = READ32BE(b10 + 48U);
    ws[12U] = u11;
    uint32_t u12 = READ32BE(b10 + 52U);
    ws[13U] = u12;
    uint32_t u13 = READ32BE(b10 + 56U);
    ws[14U] = u13;
    uint32_t u14 = READ32BE(b10 + 60U);
    ws[15U] = u14;
    KRML_MAYBE_FOR4(
      i0,
      0U,
      4U,
      1U,
      KRML_MAYBE_FOR16(
        i, 0U, 16U, 1U, uint32_t k_t = Hacl_Hash_SHA2_k224_256[16U * i0 + i];
        uint32_t ws_t = ws[i];
        uint32_t a0 = hash[0U];
        uint32_t b0 = hash[1U];
        uint32_t c0 = hash[2U];
        uint32_t d0 = hash[3U];
        uint32_t e0 = hash[4U];
        uint32_t f0 = hash[5U];
        uint32_t g0 = hash[6U];
        uint32_t h02 = hash[7U];
        uint32_t k_e_t = k_t;
        uint32_t t1 = h02 +
                      ((e0 << 26U | e0 >> 6U) ^
                       ((e0 << 21U | e0 >> 11U) ^ (e0 << 7U | e0 >> 25U))) +
                      ((e0 & f0) ^ (~e0 & g0)) + k_e_t + ws_t;
        uint32_t t2 = ((a0 << 30U | a0 >> 2U) ^
                       ((a0 << 19U | a0 >> 13U) ^ (a0 << 10U | a0 >> 22U))) +
                      ((a0 & b0) ^ ((a0 & c0) ^ (b0 & c0)));
        uint32_t a1 = t1 + t2;
        uint32_t b1 = a0;
        uint32_t c1 = b0;
        uint32_t d1 = c0;
        uint32_t e1 = d0 + t1;
        uint32_t f1 = e0;
        uint32_t g1 = f0;
        uint32_t h12 = g0;
        hash[0U] = a1;
        hash[1U] = b1;
        hash[2U] = c1;
        hash[3U] = d1;
        hash[4U] = e1;
        hash[5U] = f1;
        hash[6U] = g1;
        hash[7U] = h12;);
      if (i0 < 3U) {
          KRML_MAYBE_FOR16(i, 0U, 16U, 1U, uint32_t t16 = ws[i];
                           uint32_t t15 = ws[(i + 1U) % 16U];
                           uint32_t t7 = ws[(i + 9U) % 16U];
                           uint32_t t2 = ws[(i + 14U) % 16U];
                           uint32_t s1 = (t2 << 15U | t2 >> 17U) ^
                                         ((t2 << 13U | t2 >> 19U) ^ t2 >> 10U);
                           uint32_t s0 =
                             (t15 << 25U | t15 >> 7U) ^
                             ((t15 << 14U | t15 >> 18U) ^ t15 >> 3U);
                           ws[i] = s1 + t7 + s0 + t16;);
      });
    KRML_MAYBE_FOR8(i, 0U, 8U, 1U, uint32_t* os = hash;
                    uint32_t x = hash[i] + hash_old[i];
                    os[i] = x;);
}
