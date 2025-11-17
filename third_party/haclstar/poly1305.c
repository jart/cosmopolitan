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
#include "libc/mem/mem.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/macros.h"
#include "third_party/haclstar/mask.h"
#include "third_party/haclstar/optional.h"
__static_yoink("hacl_star_notice");

struct Hacl_MAC_Poly1305_state_t_s
{
    uint64_t* block_state;
    uint8_t* buf;
    uint64_t total_len;
    uint8_t* p_key;
};

void
Hacl_MAC_Poly1305_poly1305_init(uint64_t ctx[static 25],
                                const uint8_t key[static 32])
{
    uint64_t* acc = ctx;
    uint64_t* pre = ctx + 5U;
    const uint8_t* kr = key;
    acc[0U] = 0ULL;
    acc[1U] = 0ULL;
    acc[2U] = 0ULL;
    acc[3U] = 0ULL;
    acc[4U] = 0ULL;
    uint64_t u0 = READ64LE(kr);
    uint64_t lo = u0;
    uint64_t u = READ64LE(kr + 8U);
    uint64_t hi = u;
    uint64_t mask0 = 0x0ffffffc0fffffffULL;
    uint64_t mask1 = 0x0ffffffc0ffffffcULL;
    uint64_t lo1 = lo & mask0;
    uint64_t hi1 = hi & mask1;
    uint64_t* r = pre;
    uint64_t* r5 = pre + 5U;
    uint64_t* rn = pre + 10U;
    uint64_t* rn_5 = pre + 15U;
    uint64_t r_vec0 = lo1;
    uint64_t r_vec1 = hi1;
    uint64_t f00 = r_vec0 & 0x3ffffffULL;
    uint64_t f10 = r_vec0 >> 26U & 0x3ffffffULL;
    uint64_t f20 = r_vec0 >> 52U | (r_vec1 & 0x3fffULL) << 12U;
    uint64_t f30 = r_vec1 >> 14U & 0x3ffffffULL;
    uint64_t f40 = r_vec1 >> 40U;
    uint64_t f0 = f00;
    uint64_t f1 = f10;
    uint64_t f2 = f20;
    uint64_t f3 = f30;
    uint64_t f4 = f40;
    r[0U] = f0;
    r[1U] = f1;
    r[2U] = f2;
    r[3U] = f3;
    r[4U] = f4;
    uint64_t f200 = r[0U];
    uint64_t f21 = r[1U];
    uint64_t f22 = r[2U];
    uint64_t f23 = r[3U];
    uint64_t f24 = r[4U];
    r5[0U] = f200 * 5ULL;
    r5[1U] = f21 * 5ULL;
    r5[2U] = f22 * 5ULL;
    r5[3U] = f23 * 5ULL;
    r5[4U] = f24 * 5ULL;
    rn[0U] = r[0U];
    rn[1U] = r[1U];
    rn[2U] = r[2U];
    rn[3U] = r[3U];
    rn[4U] = r[4U];
    rn_5[0U] = r5[0U];
    rn_5[1U] = r5[1U];
    rn_5[2U] = r5[2U];
    rn_5[3U] = r5[3U];
    rn_5[4U] = r5[4U];
}

static void
poly1305_update(uint64_t ctx[static 25], uint32_t len, uint8_t* text)
{
    uint64_t* pre = ctx + 5U;
    uint64_t* acc = ctx;
    uint32_t nb = len / 16U;
    uint32_t rem = len % 16U;
    for (uint32_t i = 0U; i < nb; i++) {
        uint8_t* block = text + i * 16U;
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
    }
    if (rem > 0U) {
        uint8_t* last = text + nb * 16U;
        uint64_t e[5U] = { 0U };
        uint8_t tmp[16U] = { 0U };
        memcpy(tmp, last, rem * sizeof(uint8_t));
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
        uint64_t b = 1ULL << rem * 8U % 26U;
        uint64_t mask = b;
        uint64_t fi = e[rem * 8U / 26U];
        e[rem * 8U / 26U] = fi | mask;
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
        return;
    }
}

void
Hacl_MAC_Poly1305_poly1305_finish(uint8_t tag[static 16],
                                  const uint8_t key[static 32],
                                  uint64_t ctx[static 25])
{
    uint64_t* acc = ctx;
    const uint8_t* ks = key + 16U;
    uint64_t f0 = acc[0U];
    uint64_t f13 = acc[1U];
    uint64_t f23 = acc[2U];
    uint64_t f33 = acc[3U];
    uint64_t f40 = acc[4U];
    uint64_t l0 = f0 + 0ULL;
    uint64_t tmp00 = l0 & 0x3ffffffULL;
    uint64_t c00 = l0 >> 26U;
    uint64_t l1 = f13 + c00;
    uint64_t tmp10 = l1 & 0x3ffffffULL;
    uint64_t c10 = l1 >> 26U;
    uint64_t l2 = f23 + c10;
    uint64_t tmp20 = l2 & 0x3ffffffULL;
    uint64_t c20 = l2 >> 26U;
    uint64_t l3 = f33 + c20;
    uint64_t tmp30 = l3 & 0x3ffffffULL;
    uint64_t c30 = l3 >> 26U;
    uint64_t l4 = f40 + c30;
    uint64_t tmp40 = l4 & 0x3ffffffULL;
    uint64_t c40 = l4 >> 26U;
    uint64_t f010 = tmp00 + c40 * 5ULL;
    uint64_t f110 = tmp10;
    uint64_t f210 = tmp20;
    uint64_t f310 = tmp30;
    uint64_t f410 = tmp40;
    uint64_t l = f010 + 0ULL;
    uint64_t tmp0 = l & 0x3ffffffULL;
    uint64_t c0 = l >> 26U;
    uint64_t l5 = f110 + c0;
    uint64_t tmp1 = l5 & 0x3ffffffULL;
    uint64_t c1 = l5 >> 26U;
    uint64_t l6 = f210 + c1;
    uint64_t tmp2 = l6 & 0x3ffffffULL;
    uint64_t c2 = l6 >> 26U;
    uint64_t l7 = f310 + c2;
    uint64_t tmp3 = l7 & 0x3ffffffULL;
    uint64_t c3 = l7 >> 26U;
    uint64_t l8 = f410 + c3;
    uint64_t tmp4 = l8 & 0x3ffffffULL;
    uint64_t c4 = l8 >> 26U;
    uint64_t f02 = tmp0 + c4 * 5ULL;
    uint64_t f12 = tmp1;
    uint64_t f22 = tmp2;
    uint64_t f32 = tmp3;
    uint64_t f42 = tmp4;
    uint64_t mh = 0x3ffffffULL;
    uint64_t ml = 0x3fffffbULL;
    uint64_t mask = FStar_UInt64_eq_mask(f42, mh);
    uint64_t mask1 = mask & FStar_UInt64_eq_mask(f32, mh);
    uint64_t mask2 = mask1 & FStar_UInt64_eq_mask(f22, mh);
    uint64_t mask3 = mask2 & FStar_UInt64_eq_mask(f12, mh);
    uint64_t mask4 = mask3 & ~~FStar_UInt64_gte_mask(f02, ml);
    uint64_t ph = mask4 & mh;
    uint64_t pl = mask4 & ml;
    uint64_t o0 = f02 - pl;
    uint64_t o1 = f12 - ph;
    uint64_t o2 = f22 - ph;
    uint64_t o3 = f32 - ph;
    uint64_t o4 = f42 - ph;
    uint64_t f011 = o0;
    uint64_t f111 = o1;
    uint64_t f211 = o2;
    uint64_t f311 = o3;
    uint64_t f411 = o4;
    acc[0U] = f011;
    acc[1U] = f111;
    acc[2U] = f211;
    acc[3U] = f311;
    acc[4U] = f411;
    uint64_t f00 = acc[0U];
    uint64_t f1 = acc[1U];
    uint64_t f2 = acc[2U];
    uint64_t f3 = acc[3U];
    uint64_t f4 = acc[4U];
    uint64_t f01 = f00;
    uint64_t f112 = f1;
    uint64_t f212 = f2;
    uint64_t f312 = f3;
    uint64_t f41 = f4;
    uint64_t lo = (f01 | f112 << 26U) | f212 << 52U;
    uint64_t hi = (f212 >> 12U | f312 << 14U) | f41 << 40U;
    uint64_t f10 = lo;
    uint64_t f11 = hi;
    uint64_t u0 = READ64LE(ks);
    uint64_t lo0 = u0;
    uint64_t u = READ64LE(ks + 8U);
    uint64_t hi0 = u;
    uint64_t f20 = lo0;
    uint64_t f21 = hi0;
    uint64_t r0 = f10 + f20;
    uint64_t r1 = f11 + f21;
    uint64_t c = (r0 ^ ((r0 ^ f20) | ((r0 - f20) ^ f20))) >> 63U;
    uint64_t r11 = r1 + c;
    uint64_t f30 = r0;
    uint64_t f31 = r11;
    WRITE64LE(tag, f30);
    WRITE64LE(tag + 8U, f31);
}

Hacl_MAC_Poly1305_state_t*
Hacl_MAC_Poly1305_malloc(const uint8_t key[static 32])
{
    uint8_t* buf = (uint8_t*)calloc(16U, sizeof(uint8_t));
    if (buf == NULL) {
        return NULL;
    }
    uint8_t* buf1 = buf;
    uint64_t* r1 = (uint64_t*)calloc(25U, sizeof(uint64_t));
    Hacl_Streaming_Types_optional_64 block_state;
    if (r1 == NULL) {
        block_state = ((Hacl_Streaming_Types_optional_64){
          .tag = Hacl_Streaming_Types_None });
    } else {
        block_state = ((Hacl_Streaming_Types_optional_64){
          .tag = Hacl_Streaming_Types_Some, .v = r1 });
    }
    if (block_state.tag == Hacl_Streaming_Types_None) {
        free(buf1);
        return NULL;
    }
    if (block_state.tag == Hacl_Streaming_Types_Some) {
        uint64_t* block_state1 = block_state.v;
        uint8_t* b = (uint8_t*)calloc(32U, sizeof(uint8_t));
        FStar_Pervasives_Native_option___uint8_t_ k_;
        if (b == NULL) {
            k_ = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_None });
        } else {
            k_ = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_Some, .v = b });
        }
        FStar_Pervasives_Native_option___uint8_t_ k_0;
        if (k_.tag == Hacl_Streaming_Types_None) {
            free(block_state1);
            free(buf1);
            k_0 = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_None });
        } else if (k_.tag == Hacl_Streaming_Types_Some) {
            uint8_t* k_1 = k_.v;
            memcpy(k_1, key, 32U * sizeof(uint8_t));
            k_0 = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_Some, .v = k_1 });
        } else {
            KRML_EABORT(FStar_Pervasives_Native_option___uint8_t_,
                        "unreachable (pattern matches are exhaustive in F*)");
        }
        if (k_0.tag == Hacl_Streaming_Types_None) {
            return NULL;
        }
        if (k_0.tag == Hacl_Streaming_Types_Some) {
            uint8_t* k_1 = k_0.v;
            Hacl_MAC_Poly1305_state_t s = { .block_state = block_state1,
                                            .buf = buf1,
                                            .total_len = (uint64_t)0U,
                                            .p_key = k_1 };
            Hacl_MAC_Poly1305_state_t* p = (Hacl_MAC_Poly1305_state_t*)malloc(
              sizeof(Hacl_MAC_Poly1305_state_t));
            if (p != NULL) {
                p[0U] = s;
            }
            if (p == NULL) {
                free(k_1);
                free(block_state1);
                free(buf1);
                return NULL;
            }
            Hacl_MAC_Poly1305_poly1305_init(block_state1, key);
            return p;
        }
        KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n",
                          __FILE__,
                          __LINE__,
                          "unreachable (pattern matches are exhaustive in F*)");
        KRML_HOST_EXIT(255U);
    }
    KRML_HOST_EPRINTF("KaRaMeL abort at %s:%d\n%s\n",
                      __FILE__,
                      __LINE__,
                      "unreachable (pattern matches are exhaustive in F*)");
    KRML_HOST_EXIT(255U);
}

void
Hacl_MAC_Poly1305_reset(Hacl_MAC_Poly1305_state_t* state,
                        const uint8_t key[static 32])
{
    Hacl_MAC_Poly1305_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    uint64_t* block_state = scrut.block_state;
    Hacl_MAC_Poly1305_poly1305_init(block_state, key);
    memcpy(k_, key, 32U * sizeof(uint8_t));
    uint8_t* k_1 = k_;
    Hacl_MAC_Poly1305_state_t tmp = { .block_state = block_state,
                                      .buf = buf,
                                      .total_len = (uint64_t)0U,
                                      .p_key = k_1 };
    state[0U] = tmp;
}

/**
0 = success, 1 = max length exceeded
*/
Hacl_Streaming_Types_error_code
Hacl_MAC_Poly1305_update(Hacl_MAC_Poly1305_state_t* state,
                         uint8_t* chunk,
                         uint32_t chunk_len)
{
    Hacl_MAC_Poly1305_state_t s = *state;
    uint64_t total_len = s.total_len;
    if ((uint64_t)chunk_len > 0xffffffffULL - total_len) {
        return Hacl_Streaming_Types_MaximumLengthExceeded;
    }
    uint32_t sz;
    if (total_len % (uint64_t)16U == 0ULL && total_len > 0ULL) {
        sz = 16U;
    } else {
        sz = (uint32_t)(total_len % (uint64_t)16U);
    }
    if (chunk_len <= 16U - sz) {
        Hacl_MAC_Poly1305_state_t s1 = *state;
        uint64_t* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)16U == 0ULL && total_len1 > 0ULL) {
            sz1 = 16U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)16U);
        }
        uint8_t* buf2 = buf + sz1;
        memcpy(buf2, chunk, chunk_len * sizeof(uint8_t));
        uint64_t total_len2 = total_len1 + (uint64_t)chunk_len;
        *state = ((Hacl_MAC_Poly1305_state_t){ .block_state = block_state1,
                                               .buf = buf,
                                               .total_len = total_len2,
                                               .p_key = k_1 });
    } else if (sz == 0U) {
        Hacl_MAC_Poly1305_state_t s1 = *state;
        uint64_t* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)16U == 0ULL && total_len1 > 0ULL) {
            sz1 = 16U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)16U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 16U, buf);
        }
        uint32_t ite;
        if ((uint64_t)chunk_len % (uint64_t)16U == 0ULL &&
            (uint64_t)chunk_len > 0ULL) {
            ite = 16U;
        } else {
            ite = (uint32_t)((uint64_t)chunk_len % (uint64_t)16U);
        }
        uint32_t n_blocks = (chunk_len - ite) / 16U;
        uint32_t data1_len = n_blocks * 16U;
        uint32_t data2_len = chunk_len - data1_len;
        uint8_t* data1 = chunk;
        uint8_t* data2 = chunk + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_state_t){ .block_state = block_state1,
                                               .buf = buf,
                                               .total_len = total_len1 +
                                                            (uint64_t)chunk_len,
                                               .p_key = k_1 });
    } else {
        uint32_t diff = 16U - sz;
        uint8_t* chunk1 = chunk;
        uint8_t* chunk2 = chunk + diff;
        Hacl_MAC_Poly1305_state_t s1 = *state;
        uint64_t* block_state10 = s1.block_state;
        uint8_t* buf0 = s1.buf;
        uint64_t total_len10 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz10;
        if (total_len10 % (uint64_t)16U == 0ULL && total_len10 > 0ULL) {
            sz10 = 16U;
        } else {
            sz10 = (uint32_t)(total_len10 % (uint64_t)16U);
        }
        uint8_t* buf2 = buf0 + sz10;
        memcpy(buf2, chunk1, diff * sizeof(uint8_t));
        uint64_t total_len2 = total_len10 + (uint64_t)diff;
        *state = ((Hacl_MAC_Poly1305_state_t){ .block_state = block_state10,
                                               .buf = buf0,
                                               .total_len = total_len2,
                                               .p_key = k_1 });
        Hacl_MAC_Poly1305_state_t s10 = *state;
        uint64_t* block_state1 = s10.block_state;
        uint8_t* buf = s10.buf;
        uint64_t total_len1 = s10.total_len;
        uint8_t* k_10 = s10.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)16U == 0ULL && total_len1 > 0ULL) {
            sz1 = 16U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)16U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 16U, buf);
        }
        uint32_t ite;
        if ((uint64_t)(chunk_len - diff) % (uint64_t)16U == 0ULL &&
            (uint64_t)(chunk_len - diff) > 0ULL) {
            ite = 16U;
        } else {
            ite = (uint32_t)((uint64_t)(chunk_len - diff) % (uint64_t)16U);
        }
        uint32_t n_blocks = (chunk_len - diff - ite) / 16U;
        uint32_t data1_len = n_blocks * 16U;
        uint32_t data2_len = chunk_len - diff - data1_len;
        uint8_t* data1 = chunk2;
        uint8_t* data2 = chunk2 + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_state_t){
          .block_state = block_state1,
          .buf = buf,
          .total_len = total_len1 + (uint64_t)(chunk_len - diff),
          .p_key = k_10 });
    }
    return Hacl_Streaming_Types_Success;
}

void
Hacl_MAC_Poly1305_digest(Hacl_MAC_Poly1305_state_t* state,
                         uint8_t output[static 16])
{
    Hacl_MAC_Poly1305_state_t scrut = *state;
    uint64_t* block_state = scrut.block_state;
    uint8_t* buf_ = scrut.buf;
    uint64_t total_len = scrut.total_len;
    uint8_t* k_ = scrut.p_key;
    uint32_t r;
    if (total_len % (uint64_t)16U == 0ULL && total_len > 0ULL) {
        r = 16U;
    } else {
        r = (uint32_t)(total_len % (uint64_t)16U);
    }
    uint8_t* buf_1 = buf_;
    uint64_t r1[25U] = { 0U };
    uint64_t* tmp_block_state = r1;
    memcpy(tmp_block_state, block_state, 25U * sizeof(uint64_t));
    uint32_t ite;
    if (r % 16U == 0U && r > 0U) {
        ite = 16U;
    } else {
        ite = r % 16U;
    }
    uint8_t* buf_last = buf_1 + r - ite;
    uint8_t* buf_multi = buf_1;
    poly1305_update(tmp_block_state, 0U, buf_multi);
    poly1305_update(tmp_block_state, r, buf_last);
    uint64_t tmp[25U] = { 0U };
    memcpy(tmp, tmp_block_state, 25U * sizeof(uint64_t));
    Hacl_MAC_Poly1305_poly1305_finish(output, k_, tmp);
}

void
Hacl_MAC_Poly1305_free(Hacl_MAC_Poly1305_state_t* state)
{
    Hacl_MAC_Poly1305_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    uint64_t* block_state = scrut.block_state;
    free(k_);
    free(block_state);
    free(buf);
    free(state);
}

void
Hacl_MAC_Poly1305_mac(uint8_t output[static 16],
                      uint8_t* input,
                      uint32_t input_len,
                      uint8_t key[static 32])
{
    uint64_t ctx[25U] = { 0U };
    Hacl_MAC_Poly1305_poly1305_init(ctx, key);
    poly1305_update(ctx, input_len, input);
    Hacl_MAC_Poly1305_poly1305_finish(output, key, ctx);
}
