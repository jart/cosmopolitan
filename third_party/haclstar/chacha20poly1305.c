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
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/loop.h"
#include "third_party/haclstar/mask.h"
__static_yoink("hacl_star_notice");

static inline void
poly1305_padded_32(uint64_t ctx[static 25], uint32_t len, const uint8_t* text)
{
    uint32_t n = len / 16U;
    uint32_t r = len % 16U;
    const uint8_t* blocks = text;
    const uint8_t* rem = text + n * 16U;
    uint64_t* pre0 = ctx + 5U;
    uint64_t* acc0 = ctx;
    uint32_t nb = n * 16U / 16U;
    uint32_t rem1 = n * 16U % 16U;
    for (uint32_t i = 0U; i < nb; i++) {
        const uint8_t* block = blocks + i * 16U;
        uint64_t e[5U] = { 0U };
        uint64_t u0 = READ64LE(block);
        uint64_t lo = u0;
        uint64_t u = READ64LE(block + 8U);
        uint64_t hi = u;
        uint64_t f0 = lo;
        uint64_t f1 = hi;
        uint64_t f010 = f0 & 0x3ffffffULL;
        uint64_t f110 = f0 >> 26U & 0x3ffffffULL;
        uint64_t f20 = f0 >> 52U | (f1 & 0x3fffULL) << 12U;
        uint64_t f30 = f1 >> 14U & 0x3ffffffULL;
        uint64_t f40 = f1 >> 40U;
        uint64_t f01 = f010;
        uint64_t f111 = f110;
        uint64_t f2 = f20;
        uint64_t f3 = f30;
        uint64_t f41 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f41;
        uint64_t b = 0x1000000ULL;
        uint64_t mask = b;
        uint64_t f4 = e[4U];
        e[4U] = f4 | mask;
        uint64_t* r1 = pre0;
        uint64_t* r5 = pre0 + 5U;
        uint64_t r0 = r1[0U];
        uint64_t r11 = r1[1U];
        uint64_t r2 = r1[2U];
        uint64_t r3 = r1[3U];
        uint64_t r4 = r1[4U];
        uint64_t r51 = r5[1U];
        uint64_t r52 = r5[2U];
        uint64_t r53 = r5[3U];
        uint64_t r54 = r5[4U];
        uint64_t f10 = e[0U];
        uint64_t f11 = e[1U];
        uint64_t f12 = e[2U];
        uint64_t f13 = e[3U];
        uint64_t f14 = e[4U];
        uint64_t a0 = acc0[0U];
        uint64_t a1 = acc0[1U];
        uint64_t a2 = acc0[2U];
        uint64_t a3 = acc0[3U];
        uint64_t a4 = acc0[4U];
        uint64_t a01 = a0 + f10;
        uint64_t a11 = a1 + f11;
        uint64_t a21 = a2 + f12;
        uint64_t a31 = a3 + f13;
        uint64_t a41 = a4 + f14;
        uint64_t a02 = r0 * a01;
        uint64_t a12 = r11 * a01;
        uint64_t a22 = r2 * a01;
        uint64_t a32 = r3 * a01;
        uint64_t a42 = r4 * a01;
        uint64_t a03 = a02 + r54 * a11;
        uint64_t a13 = a12 + r0 * a11;
        uint64_t a23 = a22 + r11 * a11;
        uint64_t a33 = a32 + r2 * a11;
        uint64_t a43 = a42 + r3 * a11;
        uint64_t a04 = a03 + r53 * a21;
        uint64_t a14 = a13 + r54 * a21;
        uint64_t a24 = a23 + r0 * a21;
        uint64_t a34 = a33 + r11 * a21;
        uint64_t a44 = a43 + r2 * a21;
        uint64_t a05 = a04 + r52 * a31;
        uint64_t a15 = a14 + r53 * a31;
        uint64_t a25 = a24 + r54 * a31;
        uint64_t a35 = a34 + r0 * a31;
        uint64_t a45 = a44 + r11 * a31;
        uint64_t a06 = a05 + r51 * a41;
        uint64_t a16 = a15 + r52 * a41;
        uint64_t a26 = a25 + r53 * a41;
        uint64_t a36 = a35 + r54 * a41;
        uint64_t a46 = a45 + r0 * a41;
        uint64_t t0 = a06;
        uint64_t t1 = a16;
        uint64_t t2 = a26;
        uint64_t t3 = a36;
        uint64_t t4 = a46;
        uint64_t mask26 = 0x3ffffffULL;
        uint64_t z0 = t0 >> 26U;
        uint64_t z1 = t3 >> 26U;
        uint64_t x0 = t0 & mask26;
        uint64_t x3 = t3 & mask26;
        uint64_t x1 = t1 + z0;
        uint64_t x4 = t4 + z1;
        uint64_t z01 = x1 >> 26U;
        uint64_t z11 = x4 >> 26U;
        uint64_t t = z11 << 2U;
        uint64_t z12 = z11 + t;
        uint64_t x11 = x1 & mask26;
        uint64_t x41 = x4 & mask26;
        uint64_t x2 = t2 + z01;
        uint64_t x01 = x0 + z12;
        uint64_t z02 = x2 >> 26U;
        uint64_t z13 = x01 >> 26U;
        uint64_t x21 = x2 & mask26;
        uint64_t x02 = x01 & mask26;
        uint64_t x31 = x3 + z02;
        uint64_t x12 = x11 + z13;
        uint64_t z03 = x31 >> 26U;
        uint64_t x32 = x31 & mask26;
        uint64_t x42 = x41 + z03;
        uint64_t o0 = x02;
        uint64_t o1 = x12;
        uint64_t o2 = x21;
        uint64_t o3 = x32;
        uint64_t o4 = x42;
        acc0[0U] = o0;
        acc0[1U] = o1;
        acc0[2U] = o2;
        acc0[3U] = o3;
        acc0[4U] = o4;
    }
    if (rem1 > 0U) {
        const uint8_t* last = blocks + nb * 16U;
        uint64_t e[5U] = { 0U };
        uint8_t tmp[16U] = { 0U };
        memcpy(tmp, last, rem1 * sizeof(uint8_t));
        uint64_t u0 = READ64LE(tmp);
        uint64_t lo = u0;
        uint64_t u = READ64LE(tmp + 8U);
        uint64_t hi = u;
        uint64_t f0 = lo;
        uint64_t f1 = hi;
        uint64_t f010 = f0 & 0x3ffffffULL;
        uint64_t f110 = f0 >> 26U & 0x3ffffffULL;
        uint64_t f20 = f0 >> 52U | (f1 & 0x3fffULL) << 12U;
        uint64_t f30 = f1 >> 14U & 0x3ffffffULL;
        uint64_t f40 = f1 >> 40U;
        uint64_t f01 = f010;
        uint64_t f111 = f110;
        uint64_t f2 = f20;
        uint64_t f3 = f30;
        uint64_t f4 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f4;
        uint64_t b = 1ULL << rem1 * 8U % 26U;
        uint64_t mask = b;
        uint64_t fi = e[rem1 * 8U / 26U];
        e[rem1 * 8U / 26U] = fi | mask;
        uint64_t* r1 = pre0;
        uint64_t* r5 = pre0 + 5U;
        uint64_t r0 = r1[0U];
        uint64_t r11 = r1[1U];
        uint64_t r2 = r1[2U];
        uint64_t r3 = r1[3U];
        uint64_t r4 = r1[4U];
        uint64_t r51 = r5[1U];
        uint64_t r52 = r5[2U];
        uint64_t r53 = r5[3U];
        uint64_t r54 = r5[4U];
        uint64_t f10 = e[0U];
        uint64_t f11 = e[1U];
        uint64_t f12 = e[2U];
        uint64_t f13 = e[3U];
        uint64_t f14 = e[4U];
        uint64_t a0 = acc0[0U];
        uint64_t a1 = acc0[1U];
        uint64_t a2 = acc0[2U];
        uint64_t a3 = acc0[3U];
        uint64_t a4 = acc0[4U];
        uint64_t a01 = a0 + f10;
        uint64_t a11 = a1 + f11;
        uint64_t a21 = a2 + f12;
        uint64_t a31 = a3 + f13;
        uint64_t a41 = a4 + f14;
        uint64_t a02 = r0 * a01;
        uint64_t a12 = r11 * a01;
        uint64_t a22 = r2 * a01;
        uint64_t a32 = r3 * a01;
        uint64_t a42 = r4 * a01;
        uint64_t a03 = a02 + r54 * a11;
        uint64_t a13 = a12 + r0 * a11;
        uint64_t a23 = a22 + r11 * a11;
        uint64_t a33 = a32 + r2 * a11;
        uint64_t a43 = a42 + r3 * a11;
        uint64_t a04 = a03 + r53 * a21;
        uint64_t a14 = a13 + r54 * a21;
        uint64_t a24 = a23 + r0 * a21;
        uint64_t a34 = a33 + r11 * a21;
        uint64_t a44 = a43 + r2 * a21;
        uint64_t a05 = a04 + r52 * a31;
        uint64_t a15 = a14 + r53 * a31;
        uint64_t a25 = a24 + r54 * a31;
        uint64_t a35 = a34 + r0 * a31;
        uint64_t a45 = a44 + r11 * a31;
        uint64_t a06 = a05 + r51 * a41;
        uint64_t a16 = a15 + r52 * a41;
        uint64_t a26 = a25 + r53 * a41;
        uint64_t a36 = a35 + r54 * a41;
        uint64_t a46 = a45 + r0 * a41;
        uint64_t t0 = a06;
        uint64_t t1 = a16;
        uint64_t t2 = a26;
        uint64_t t3 = a36;
        uint64_t t4 = a46;
        uint64_t mask26 = 0x3ffffffULL;
        uint64_t z0 = t0 >> 26U;
        uint64_t z1 = t3 >> 26U;
        uint64_t x0 = t0 & mask26;
        uint64_t x3 = t3 & mask26;
        uint64_t x1 = t1 + z0;
        uint64_t x4 = t4 + z1;
        uint64_t z01 = x1 >> 26U;
        uint64_t z11 = x4 >> 26U;
        uint64_t t = z11 << 2U;
        uint64_t z12 = z11 + t;
        uint64_t x11 = x1 & mask26;
        uint64_t x41 = x4 & mask26;
        uint64_t x2 = t2 + z01;
        uint64_t x01 = x0 + z12;
        uint64_t z02 = x2 >> 26U;
        uint64_t z13 = x01 >> 26U;
        uint64_t x21 = x2 & mask26;
        uint64_t x02 = x01 & mask26;
        uint64_t x31 = x3 + z02;
        uint64_t x12 = x11 + z13;
        uint64_t z03 = x31 >> 26U;
        uint64_t x32 = x31 & mask26;
        uint64_t x42 = x41 + z03;
        uint64_t o0 = x02;
        uint64_t o1 = x12;
        uint64_t o2 = x21;
        uint64_t o3 = x32;
        uint64_t o4 = x42;
        acc0[0U] = o0;
        acc0[1U] = o1;
        acc0[2U] = o2;
        acc0[3U] = o3;
        acc0[4U] = o4;
    }
    uint8_t tmp[16U] = { 0U };
    memcpy(tmp, rem, r * sizeof(uint8_t));
    if (r > 0U) {
        uint64_t* pre = ctx + 5U;
        uint64_t* acc = ctx;
        uint64_t e[5U] = { 0U };
        uint64_t u0 = READ64LE(tmp);
        uint64_t lo = u0;
        uint64_t u = READ64LE(tmp + 8U);
        uint64_t hi = u;
        uint64_t f0 = lo;
        uint64_t f1 = hi;
        uint64_t f010 = f0 & 0x3ffffffULL;
        uint64_t f110 = f0 >> 26U & 0x3ffffffULL;
        uint64_t f20 = f0 >> 52U | (f1 & 0x3fffULL) << 12U;
        uint64_t f30 = f1 >> 14U & 0x3ffffffULL;
        uint64_t f40 = f1 >> 40U;
        uint64_t f01 = f010;
        uint64_t f111 = f110;
        uint64_t f2 = f20;
        uint64_t f3 = f30;
        uint64_t f41 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f41;
        uint64_t b = 0x1000000ULL;
        uint64_t mask = b;
        uint64_t f4 = e[4U];
        e[4U] = f4 | mask;
        uint64_t* r1 = pre;
        uint64_t* r5 = pre + 5U;
        uint64_t r0 = r1[0U];
        uint64_t r11 = r1[1U];
        uint64_t r2 = r1[2U];
        uint64_t r3 = r1[3U];
        uint64_t r4 = r1[4U];
        uint64_t r51 = r5[1U];
        uint64_t r52 = r5[2U];
        uint64_t r53 = r5[3U];
        uint64_t r54 = r5[4U];
        uint64_t f10 = e[0U];
        uint64_t f11 = e[1U];
        uint64_t f12 = e[2U];
        uint64_t f13 = e[3U];
        uint64_t f14 = e[4U];
        uint64_t a0 = acc[0U];
        uint64_t a1 = acc[1U];
        uint64_t a2 = acc[2U];
        uint64_t a3 = acc[3U];
        uint64_t a4 = acc[4U];
        uint64_t a01 = a0 + f10;
        uint64_t a11 = a1 + f11;
        uint64_t a21 = a2 + f12;
        uint64_t a31 = a3 + f13;
        uint64_t a41 = a4 + f14;
        uint64_t a02 = r0 * a01;
        uint64_t a12 = r11 * a01;
        uint64_t a22 = r2 * a01;
        uint64_t a32 = r3 * a01;
        uint64_t a42 = r4 * a01;
        uint64_t a03 = a02 + r54 * a11;
        uint64_t a13 = a12 + r0 * a11;
        uint64_t a23 = a22 + r11 * a11;
        uint64_t a33 = a32 + r2 * a11;
        uint64_t a43 = a42 + r3 * a11;
        uint64_t a04 = a03 + r53 * a21;
        uint64_t a14 = a13 + r54 * a21;
        uint64_t a24 = a23 + r0 * a21;
        uint64_t a34 = a33 + r11 * a21;
        uint64_t a44 = a43 + r2 * a21;
        uint64_t a05 = a04 + r52 * a31;
        uint64_t a15 = a14 + r53 * a31;
        uint64_t a25 = a24 + r54 * a31;
        uint64_t a35 = a34 + r0 * a31;
        uint64_t a45 = a44 + r11 * a31;
        uint64_t a06 = a05 + r51 * a41;
        uint64_t a16 = a15 + r52 * a41;
        uint64_t a26 = a25 + r53 * a41;
        uint64_t a36 = a35 + r54 * a41;
        uint64_t a46 = a45 + r0 * a41;
        uint64_t t0 = a06;
        uint64_t t1 = a16;
        uint64_t t2 = a26;
        uint64_t t3 = a36;
        uint64_t t4 = a46;
        uint64_t mask26 = 0x3ffffffULL;
        uint64_t z0 = t0 >> 26U;
        uint64_t z1 = t3 >> 26U;
        uint64_t x0 = t0 & mask26;
        uint64_t x3 = t3 & mask26;
        uint64_t x1 = t1 + z0;
        uint64_t x4 = t4 + z1;
        uint64_t z01 = x1 >> 26U;
        uint64_t z11 = x4 >> 26U;
        uint64_t t = z11 << 2U;
        uint64_t z12 = z11 + t;
        uint64_t x11 = x1 & mask26;
        uint64_t x41 = x4 & mask26;
        uint64_t x2 = t2 + z01;
        uint64_t x01 = x0 + z12;
        uint64_t z02 = x2 >> 26U;
        uint64_t z13 = x01 >> 26U;
        uint64_t x21 = x2 & mask26;
        uint64_t x02 = x01 & mask26;
        uint64_t x31 = x3 + z02;
        uint64_t x12 = x11 + z13;
        uint64_t z03 = x31 >> 26U;
        uint64_t x32 = x31 & mask26;
        uint64_t x42 = x41 + z03;
        uint64_t o0 = x02;
        uint64_t o1 = x12;
        uint64_t o2 = x21;
        uint64_t o3 = x32;
        uint64_t o4 = x42;
        acc[0U] = o0;
        acc[1U] = o1;
        acc[2U] = o2;
        acc[3U] = o3;
        acc[4U] = o4;
        return;
    }
}

static inline void
poly1305_do_32(uint8_t k[static 64],
               uint32_t aadlen,
               const uint8_t* aad,
               uint32_t mlen,
               const uint8_t* m,
               uint8_t out[static 16])
{
    uint64_t ctx[25U] = { 0U };
    uint8_t block[16U] = { 0U };
    Hacl_MAC_Poly1305_poly1305_init(ctx, k);
    if (aadlen != 0U) {
        poly1305_padded_32(ctx, aadlen, aad);
    }
    if (mlen != 0U) {
        poly1305_padded_32(ctx, mlen, m);
    }
    WRITE64LE(block, (uint64_t)aadlen);
    WRITE64LE(block + 8U, (uint64_t)mlen);
    uint64_t* pre = ctx + 5U;
    uint64_t* acc = ctx;
    uint64_t e[5U] = { 0U };
    uint64_t u0 = READ64LE(block);
    uint64_t lo = u0;
    uint64_t u = READ64LE(block + 8U);
    uint64_t hi = u;
    uint64_t f0 = lo;
    uint64_t f1 = hi;
    uint64_t f010 = f0 & 0x3ffffffULL;
    uint64_t f110 = f0 >> 26U & 0x3ffffffULL;
    uint64_t f20 = f0 >> 52U | (f1 & 0x3fffULL) << 12U;
    uint64_t f30 = f1 >> 14U & 0x3ffffffULL;
    uint64_t f40 = f1 >> 40U;
    uint64_t f01 = f010;
    uint64_t f111 = f110;
    uint64_t f2 = f20;
    uint64_t f3 = f30;
    uint64_t f41 = f40;
    e[0U] = f01;
    e[1U] = f111;
    e[2U] = f2;
    e[3U] = f3;
    e[4U] = f41;
    uint64_t b = 0x1000000ULL;
    uint64_t mask = b;
    uint64_t f4 = e[4U];
    e[4U] = f4 | mask;
    uint64_t* r = pre;
    uint64_t* r5 = pre + 5U;
    uint64_t r0 = r[0U];
    uint64_t r1 = r[1U];
    uint64_t r2 = r[2U];
    uint64_t r3 = r[3U];
    uint64_t r4 = r[4U];
    uint64_t r51 = r5[1U];
    uint64_t r52 = r5[2U];
    uint64_t r53 = r5[3U];
    uint64_t r54 = r5[4U];
    uint64_t f10 = e[0U];
    uint64_t f11 = e[1U];
    uint64_t f12 = e[2U];
    uint64_t f13 = e[3U];
    uint64_t f14 = e[4U];
    uint64_t a0 = acc[0U];
    uint64_t a1 = acc[1U];
    uint64_t a2 = acc[2U];
    uint64_t a3 = acc[3U];
    uint64_t a4 = acc[4U];
    uint64_t a01 = a0 + f10;
    uint64_t a11 = a1 + f11;
    uint64_t a21 = a2 + f12;
    uint64_t a31 = a3 + f13;
    uint64_t a41 = a4 + f14;
    uint64_t a02 = r0 * a01;
    uint64_t a12 = r1 * a01;
    uint64_t a22 = r2 * a01;
    uint64_t a32 = r3 * a01;
    uint64_t a42 = r4 * a01;
    uint64_t a03 = a02 + r54 * a11;
    uint64_t a13 = a12 + r0 * a11;
    uint64_t a23 = a22 + r1 * a11;
    uint64_t a33 = a32 + r2 * a11;
    uint64_t a43 = a42 + r3 * a11;
    uint64_t a04 = a03 + r53 * a21;
    uint64_t a14 = a13 + r54 * a21;
    uint64_t a24 = a23 + r0 * a21;
    uint64_t a34 = a33 + r1 * a21;
    uint64_t a44 = a43 + r2 * a21;
    uint64_t a05 = a04 + r52 * a31;
    uint64_t a15 = a14 + r53 * a31;
    uint64_t a25 = a24 + r54 * a31;
    uint64_t a35 = a34 + r0 * a31;
    uint64_t a45 = a44 + r1 * a31;
    uint64_t a06 = a05 + r51 * a41;
    uint64_t a16 = a15 + r52 * a41;
    uint64_t a26 = a25 + r53 * a41;
    uint64_t a36 = a35 + r54 * a41;
    uint64_t a46 = a45 + r0 * a41;
    uint64_t t0 = a06;
    uint64_t t1 = a16;
    uint64_t t2 = a26;
    uint64_t t3 = a36;
    uint64_t t4 = a46;
    uint64_t mask26 = 0x3ffffffULL;
    uint64_t z0 = t0 >> 26U;
    uint64_t z1 = t3 >> 26U;
    uint64_t x0 = t0 & mask26;
    uint64_t x3 = t3 & mask26;
    uint64_t x1 = t1 + z0;
    uint64_t x4 = t4 + z1;
    uint64_t z01 = x1 >> 26U;
    uint64_t z11 = x4 >> 26U;
    uint64_t t = z11 << 2U;
    uint64_t z12 = z11 + t;
    uint64_t x11 = x1 & mask26;
    uint64_t x41 = x4 & mask26;
    uint64_t x2 = t2 + z01;
    uint64_t x01 = x0 + z12;
    uint64_t z02 = x2 >> 26U;
    uint64_t z13 = x01 >> 26U;
    uint64_t x21 = x2 & mask26;
    uint64_t x02 = x01 & mask26;
    uint64_t x31 = x3 + z02;
    uint64_t x12 = x11 + z13;
    uint64_t z03 = x31 >> 26U;
    uint64_t x32 = x31 & mask26;
    uint64_t x42 = x41 + z03;
    uint64_t o0 = x02;
    uint64_t o1 = x12;
    uint64_t o2 = x21;
    uint64_t o3 = x32;
    uint64_t o4 = x42;
    acc[0U] = o0;
    acc[1U] = o1;
    acc[2U] = o2;
    acc[3U] = o3;
    acc[4U] = o4;
    Hacl_MAC_Poly1305_poly1305_finish(out, k, ctx);
}

/**
  Encrypt a message `input` with key `key`.

  The arguments `key`, `nonce`, `data`, and `data_len` are same in
  encryption/decryption. Note: Encryption and decryption can be executed
  in-place, i.e., `input` and `output` can point to the same memory.

  @param output Pointer to `input_len` bytes of memory where the
      ciphertext is written to.

  @param tag Pointer to 16 bytes of memory where the message
      authentication code is written to. This must be given to the
      decrypt operation. It's safe to transmit this value in clear text.

  @param input Pointer to `input_len` bytes of memory where the message
      is read from.

  @param input_len Length of the message.

  @param data Pointer to `data_len` bytes of associated data. This holds
      information you want to authenticate, but not encrypt. It might
      point to packet headers or metadata, that both sides already have
      in their possession, and need assurances against tampering.

  @param data_len Length of the associated data.

  @param key Pointer to 32 bytes of memory holding symmetric encryption
      key that was exchanged earlier using frodo1344 or curve25519.

  @param nonce Pointer to 12 bytes of memory where the AEAD nonce is
      read from. All encryption values must have a unique nonce, or
      you'll compromise the security of all messages encrypted with that
      key-nonce pair. One way you could implement this for a message
      stream is by having both legs start their nonces at zero, then
      increment it like a counter for each encrypt operation.
*/
void
Hacl_AEAD_Chacha20Poly1305_encrypt(uint8_t* output,
                                   uint8_t tag[16],
                                   const uint8_t* input,
                                   uint32_t input_len,
                                   const uint8_t* data,
                                   uint32_t data_len,
                                   const uint8_t key[32],
                                   const uint8_t nonce[12])
{

    /* [jart] add runtime dispatching for avx2 implementation which goes
       twenty gigabits per core on the AMD Ryzen Threadripper PRO 7995WX */
    void Hacl_AEAD_Chacha20Poly1305_Simd256_encrypt(uint8_t*,
                                                    uint8_t[16],
                                                    const uint8_t*,
                                                    uint32_t,
                                                    const uint8_t*,
                                                    uint32_t,
                                                    const uint8_t[32],
                                                    const uint8_t[12]);
    if (X86_HAVE(AVX2))
        return Hacl_AEAD_Chacha20Poly1305_Simd256_encrypt(
          output, tag, input, input_len, data, data_len, key, nonce);

    /* [jart] add runtime dispatching for sse4 implementation which goes
       ten gigabits per core on mighty AMD Ryzen Threadripper PRO 7995WX */
    void Hacl_AEAD_Chacha20Poly1305_Simd128_encrypt(uint8_t*,
                                                    uint8_t[16],
                                                    const uint8_t*,
                                                    uint32_t,
                                                    const uint8_t*,
                                                    uint32_t,
                                                    const uint8_t[32],
                                                    const uint8_t[12]);
    if (IsAarch64() || X86_HAVE(SSE4_2))
        return Hacl_AEAD_Chacha20Poly1305_Simd128_encrypt(
          output, tag, input, input_len, data, data_len, key, nonce);

    /* [jart] this goes ~4 gbit/sec on AMD Ryzen Threadripper PRO 7995WX */
    Hacl_Chacha20_chacha20_encrypt(input_len, output, input, key, nonce, 1U);
    uint8_t tmp[64U] = { 0U };
    Hacl_Chacha20_chacha20_encrypt(64U, tmp, tmp, key, nonce, 0U);
    uint8_t* key1 = tmp;
    poly1305_do_32(key1, data_len, data, input_len, output, tag);
}

/**
  Decrypt a ciphertext `input` with key `key`.

  The arguments `key`, `nonce`, `data`, and `data_len` are same in
  encryption/decryption. Note: Encryption and decryption can be executed
  in-place, i.e., `output` and `input` can point to the same memory.

  If decryption succeeds, the resulting plaintext is stored in `output`
  and the function returns the success code 0. If decryption fails, the
  array `output` remains unchanged and the function returns the error
  code 1.

  @param output Pointer to `input_len` bytes of memory where the message
      is written to.

  @param input Pointer to `input_len` bytes of memory where the
      ciphertext is read from.

  @param input_len Length of the ciphertext.

  @param data Pointer to `data_len` bytes of memory where the associated
      data is read from.

  @param data_len Length of the associated data.

  @param key Pointer to 32 bytes of memory where the AEAD key is read from.

  @param nonce Pointer to 12 bytes of memory where the AEAD nonce is read from.

  @param tag Pointer to 16 bytes of memory where the mac is read from.

  @returns 0 on succeess; 1 on failure.
*/
uint32_t
Hacl_AEAD_Chacha20Poly1305_decrypt(uint8_t* output,
                                   const uint8_t* input,
                                   uint32_t input_len,
                                   const uint8_t* data,
                                   uint32_t data_len,
                                   const uint8_t key[32],
                                   const uint8_t nonce[12],
                                   const uint8_t tag[16])
{

    /* [jart] add runtime dispatching for decrypt operation which has
              nearly identical performance properties as your encrypt */

    uint32_t Hacl_AEAD_Chacha20Poly1305_Simd256_decrypt(uint8_t*,
                                                        const uint8_t*,
                                                        uint32_t,
                                                        const uint8_t*,
                                                        uint32_t,
                                                        const uint8_t[32],
                                                        const uint8_t[12],
                                                        const uint8_t[16]);
    if (X86_HAVE(AVX2))
        return Hacl_AEAD_Chacha20Poly1305_Simd256_decrypt(
          output, input, input_len, data, data_len, key, nonce, tag);

    uint32_t Hacl_AEAD_Chacha20Poly1305_Simd128_decrypt(uint8_t*,
                                                        const uint8_t*,
                                                        uint32_t,
                                                        const uint8_t*,
                                                        uint32_t,
                                                        const uint8_t[32],
                                                        const uint8_t[12],
                                                        const uint8_t[16]);
    if (IsAarch64() || X86_HAVE(SSE4_2))
        return Hacl_AEAD_Chacha20Poly1305_Simd128_decrypt(
          output, input, input_len, data, data_len, key, nonce, tag);

    uint8_t computed_tag[16U] = { 0U };
    uint8_t tmp[64U] = { 0U };
    Hacl_Chacha20_chacha20_encrypt(64U, tmp, tmp, key, nonce, 0U);
    uint8_t* key1 = tmp;
    poly1305_do_32(key1, data_len, data, input_len, input, computed_tag);
    uint8_t res = 255U;
    KRML_MAYBE_FOR16(i,
                     0U,
                     16U,
                     1U,
                     uint8_t uu____0 =
                       FStar_UInt8_eq_mask(computed_tag[i], tag[i]);
                     res = (uint32_t)uu____0 & (uint32_t)res;);
    uint8_t z = res;
    if (z == 255U) {
        Hacl_Chacha20_chacha20_encrypt(
          input_len, output, input, key, nonce, 1U);
        return 0U;
    }
    return 1U;
}
