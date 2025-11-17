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
#include "third_party/haclstar/curve25519.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/int128.h"
#include "third_party/haclstar/loop.h"
#include "third_party/haclstar/mask.h"
__static_yoink("hacl_star_notice");

/* [jart] hacl* provides both 51-bit and 64-bit implementations of this
          elliptic curve. we only use the 51-bit one. the 64bit version
          upstream needs raw assembly and doesn't actually go faster */

/* [jart] inlining {fmul,fsqr}{1,2} makes curve25519 go 25% faster when
          using -O3, but it has barely any effect on its performance */

static const uint8_t g25519[32U] = { 9U };

inline void
Hacl_Impl_Curve25519_fmul(uint64_t out[static 5],
                          const uint64_t f1[static 5],
                          const uint64_t f2[static 5])
{
    uint64_t f10 = f1[0U];
    uint64_t f11 = f1[1U];
    uint64_t f12 = f1[2U];
    uint64_t f13 = f1[3U];
    uint64_t f14 = f1[4U];
    uint64_t f20 = f2[0U];
    uint64_t f21 = f2[1U];
    uint64_t f22 = f2[2U];
    uint64_t f23 = f2[3U];
    uint64_t f24 = f2[4U];
    uint64_t tmp1 = f21 * 19ULL;
    uint64_t tmp2 = f22 * 19ULL;
    uint64_t tmp3 = f23 * 19ULL;
    uint64_t tmp4 = f24 * 19ULL;
    FStar_UInt128_uint128 o00 = FStar_UInt128_mul_wide(f10, f20);
    FStar_UInt128_uint128 o10 = FStar_UInt128_mul_wide(f10, f21);
    FStar_UInt128_uint128 o20 = FStar_UInt128_mul_wide(f10, f22);
    FStar_UInt128_uint128 o30 = FStar_UInt128_mul_wide(f10, f23);
    FStar_UInt128_uint128 o40 = FStar_UInt128_mul_wide(f10, f24);
    FStar_UInt128_uint128 o01 =
      FStar_UInt128_add(o00, FStar_UInt128_mul_wide(f11, tmp4));
    FStar_UInt128_uint128 o11 =
      FStar_UInt128_add(o10, FStar_UInt128_mul_wide(f11, f20));
    FStar_UInt128_uint128 o21 =
      FStar_UInt128_add(o20, FStar_UInt128_mul_wide(f11, f21));
    FStar_UInt128_uint128 o31 =
      FStar_UInt128_add(o30, FStar_UInt128_mul_wide(f11, f22));
    FStar_UInt128_uint128 o41 =
      FStar_UInt128_add(o40, FStar_UInt128_mul_wide(f11, f23));
    FStar_UInt128_uint128 o02 =
      FStar_UInt128_add(o01, FStar_UInt128_mul_wide(f12, tmp3));
    FStar_UInt128_uint128 o12 =
      FStar_UInt128_add(o11, FStar_UInt128_mul_wide(f12, tmp4));
    FStar_UInt128_uint128 o22 =
      FStar_UInt128_add(o21, FStar_UInt128_mul_wide(f12, f20));
    FStar_UInt128_uint128 o32 =
      FStar_UInt128_add(o31, FStar_UInt128_mul_wide(f12, f21));
    FStar_UInt128_uint128 o42 =
      FStar_UInt128_add(o41, FStar_UInt128_mul_wide(f12, f22));
    FStar_UInt128_uint128 o03 =
      FStar_UInt128_add(o02, FStar_UInt128_mul_wide(f13, tmp2));
    FStar_UInt128_uint128 o13 =
      FStar_UInt128_add(o12, FStar_UInt128_mul_wide(f13, tmp3));
    FStar_UInt128_uint128 o23 =
      FStar_UInt128_add(o22, FStar_UInt128_mul_wide(f13, tmp4));
    FStar_UInt128_uint128 o33 =
      FStar_UInt128_add(o32, FStar_UInt128_mul_wide(f13, f20));
    FStar_UInt128_uint128 o43 =
      FStar_UInt128_add(o42, FStar_UInt128_mul_wide(f13, f21));
    FStar_UInt128_uint128 o04 =
      FStar_UInt128_add(o03, FStar_UInt128_mul_wide(f14, tmp1));
    FStar_UInt128_uint128 o14 =
      FStar_UInt128_add(o13, FStar_UInt128_mul_wide(f14, tmp2));
    FStar_UInt128_uint128 o24 =
      FStar_UInt128_add(o23, FStar_UInt128_mul_wide(f14, tmp3));
    FStar_UInt128_uint128 o34 =
      FStar_UInt128_add(o33, FStar_UInt128_mul_wide(f14, tmp4));
    FStar_UInt128_uint128 o44 =
      FStar_UInt128_add(o43, FStar_UInt128_mul_wide(f14, f20));
    FStar_UInt128_uint128 tmp_w0 = o04;
    FStar_UInt128_uint128 tmp_w1 = o14;
    FStar_UInt128_uint128 tmp_w2 = o24;
    FStar_UInt128_uint128 tmp_w3 = o34;
    FStar_UInt128_uint128 tmp_w4 = o44;
    FStar_UInt128_uint128 l_ =
      FStar_UInt128_add(tmp_w0, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp01 = FStar_UInt128_uint128_to_uint64(l_) & 0x7ffffffffffffULL;
    uint64_t c0 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_, 51U));
    FStar_UInt128_uint128 l_0 =
      FStar_UInt128_add(tmp_w1, FStar_UInt128_uint64_to_uint128(c0));
    uint64_t tmp11 = FStar_UInt128_uint128_to_uint64(l_0) & 0x7ffffffffffffULL;
    uint64_t c1 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_0, 51U));
    FStar_UInt128_uint128 l_1 =
      FStar_UInt128_add(tmp_w2, FStar_UInt128_uint64_to_uint128(c1));
    uint64_t tmp21 = FStar_UInt128_uint128_to_uint64(l_1) & 0x7ffffffffffffULL;
    uint64_t c2 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_1, 51U));
    FStar_UInt128_uint128 l_2 =
      FStar_UInt128_add(tmp_w3, FStar_UInt128_uint64_to_uint128(c2));
    uint64_t tmp31 = FStar_UInt128_uint128_to_uint64(l_2) & 0x7ffffffffffffULL;
    uint64_t c3 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_2, 51U));
    FStar_UInt128_uint128 l_3 =
      FStar_UInt128_add(tmp_w4, FStar_UInt128_uint64_to_uint128(c3));
    uint64_t tmp41 = FStar_UInt128_uint128_to_uint64(l_3) & 0x7ffffffffffffULL;
    uint64_t c4 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_3, 51U));
    uint64_t l_4 = tmp01 + c4 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c5 = l_4 >> 51U;
    uint64_t o0 = tmp0_;
    uint64_t o1 = tmp11 + c5;
    uint64_t o2 = tmp21;
    uint64_t o3 = tmp31;
    uint64_t o4 = tmp41;
    out[0U] = o0;
    out[1U] = o1;
    out[2U] = o2;
    out[3U] = o3;
    out[4U] = o4;
}

inline void
Hacl_Impl_Curve25519_fmul2(uint64_t out[static 10],
                           const uint64_t f1[static 10],
                           const uint64_t f2[static 10])
{
    uint64_t f10 = f1[0U];
    uint64_t f11 = f1[1U];
    uint64_t f12 = f1[2U];
    uint64_t f13 = f1[3U];
    uint64_t f14 = f1[4U];
    uint64_t f20 = f2[0U];
    uint64_t f21 = f2[1U];
    uint64_t f22 = f2[2U];
    uint64_t f23 = f2[3U];
    uint64_t f24 = f2[4U];
    uint64_t f30 = f1[5U];
    uint64_t f31 = f1[6U];
    uint64_t f32 = f1[7U];
    uint64_t f33 = f1[8U];
    uint64_t f34 = f1[9U];
    uint64_t f40 = f2[5U];
    uint64_t f41 = f2[6U];
    uint64_t f42 = f2[7U];
    uint64_t f43 = f2[8U];
    uint64_t f44 = f2[9U];
    uint64_t tmp11 = f21 * 19ULL;
    uint64_t tmp12 = f22 * 19ULL;
    uint64_t tmp13 = f23 * 19ULL;
    uint64_t tmp14 = f24 * 19ULL;
    uint64_t tmp21 = f41 * 19ULL;
    uint64_t tmp22 = f42 * 19ULL;
    uint64_t tmp23 = f43 * 19ULL;
    uint64_t tmp24 = f44 * 19ULL;
    FStar_UInt128_uint128 o00 = FStar_UInt128_mul_wide(f10, f20);
    FStar_UInt128_uint128 o15 = FStar_UInt128_mul_wide(f10, f21);
    FStar_UInt128_uint128 o25 = FStar_UInt128_mul_wide(f10, f22);
    FStar_UInt128_uint128 o30 = FStar_UInt128_mul_wide(f10, f23);
    FStar_UInt128_uint128 o40 = FStar_UInt128_mul_wide(f10, f24);
    FStar_UInt128_uint128 o010 =
      FStar_UInt128_add(o00, FStar_UInt128_mul_wide(f11, tmp14));
    FStar_UInt128_uint128 o110 =
      FStar_UInt128_add(o15, FStar_UInt128_mul_wide(f11, f20));
    FStar_UInt128_uint128 o210 =
      FStar_UInt128_add(o25, FStar_UInt128_mul_wide(f11, f21));
    FStar_UInt128_uint128 o310 =
      FStar_UInt128_add(o30, FStar_UInt128_mul_wide(f11, f22));
    FStar_UInt128_uint128 o410 =
      FStar_UInt128_add(o40, FStar_UInt128_mul_wide(f11, f23));
    FStar_UInt128_uint128 o020 =
      FStar_UInt128_add(o010, FStar_UInt128_mul_wide(f12, tmp13));
    FStar_UInt128_uint128 o120 =
      FStar_UInt128_add(o110, FStar_UInt128_mul_wide(f12, tmp14));
    FStar_UInt128_uint128 o220 =
      FStar_UInt128_add(o210, FStar_UInt128_mul_wide(f12, f20));
    FStar_UInt128_uint128 o320 =
      FStar_UInt128_add(o310, FStar_UInt128_mul_wide(f12, f21));
    FStar_UInt128_uint128 o420 =
      FStar_UInt128_add(o410, FStar_UInt128_mul_wide(f12, f22));
    FStar_UInt128_uint128 o030 =
      FStar_UInt128_add(o020, FStar_UInt128_mul_wide(f13, tmp12));
    FStar_UInt128_uint128 o130 =
      FStar_UInt128_add(o120, FStar_UInt128_mul_wide(f13, tmp13));
    FStar_UInt128_uint128 o230 =
      FStar_UInt128_add(o220, FStar_UInt128_mul_wide(f13, tmp14));
    FStar_UInt128_uint128 o330 =
      FStar_UInt128_add(o320, FStar_UInt128_mul_wide(f13, f20));
    FStar_UInt128_uint128 o430 =
      FStar_UInt128_add(o420, FStar_UInt128_mul_wide(f13, f21));
    FStar_UInt128_uint128 o040 =
      FStar_UInt128_add(o030, FStar_UInt128_mul_wide(f14, tmp11));
    FStar_UInt128_uint128 o140 =
      FStar_UInt128_add(o130, FStar_UInt128_mul_wide(f14, tmp12));
    FStar_UInt128_uint128 o240 =
      FStar_UInt128_add(o230, FStar_UInt128_mul_wide(f14, tmp13));
    FStar_UInt128_uint128 o340 =
      FStar_UInt128_add(o330, FStar_UInt128_mul_wide(f14, tmp14));
    FStar_UInt128_uint128 o440 =
      FStar_UInt128_add(o430, FStar_UInt128_mul_wide(f14, f20));
    FStar_UInt128_uint128 tmp_w10 = o040;
    FStar_UInt128_uint128 tmp_w11 = o140;
    FStar_UInt128_uint128 tmp_w12 = o240;
    FStar_UInt128_uint128 tmp_w13 = o340;
    FStar_UInt128_uint128 tmp_w14 = o440;
    FStar_UInt128_uint128 o0 = FStar_UInt128_mul_wide(f30, f40);
    FStar_UInt128_uint128 o1 = FStar_UInt128_mul_wide(f30, f41);
    FStar_UInt128_uint128 o2 = FStar_UInt128_mul_wide(f30, f42);
    FStar_UInt128_uint128 o3 = FStar_UInt128_mul_wide(f30, f43);
    FStar_UInt128_uint128 o4 = FStar_UInt128_mul_wide(f30, f44);
    FStar_UInt128_uint128 o01 =
      FStar_UInt128_add(o0, FStar_UInt128_mul_wide(f31, tmp24));
    FStar_UInt128_uint128 o111 =
      FStar_UInt128_add(o1, FStar_UInt128_mul_wide(f31, f40));
    FStar_UInt128_uint128 o211 =
      FStar_UInt128_add(o2, FStar_UInt128_mul_wide(f31, f41));
    FStar_UInt128_uint128 o31 =
      FStar_UInt128_add(o3, FStar_UInt128_mul_wide(f31, f42));
    FStar_UInt128_uint128 o41 =
      FStar_UInt128_add(o4, FStar_UInt128_mul_wide(f31, f43));
    FStar_UInt128_uint128 o02 =
      FStar_UInt128_add(o01, FStar_UInt128_mul_wide(f32, tmp23));
    FStar_UInt128_uint128 o121 =
      FStar_UInt128_add(o111, FStar_UInt128_mul_wide(f32, tmp24));
    FStar_UInt128_uint128 o221 =
      FStar_UInt128_add(o211, FStar_UInt128_mul_wide(f32, f40));
    FStar_UInt128_uint128 o32 =
      FStar_UInt128_add(o31, FStar_UInt128_mul_wide(f32, f41));
    FStar_UInt128_uint128 o42 =
      FStar_UInt128_add(o41, FStar_UInt128_mul_wide(f32, f42));
    FStar_UInt128_uint128 o03 =
      FStar_UInt128_add(o02, FStar_UInt128_mul_wide(f33, tmp22));
    FStar_UInt128_uint128 o131 =
      FStar_UInt128_add(o121, FStar_UInt128_mul_wide(f33, tmp23));
    FStar_UInt128_uint128 o231 =
      FStar_UInt128_add(o221, FStar_UInt128_mul_wide(f33, tmp24));
    FStar_UInt128_uint128 o33 =
      FStar_UInt128_add(o32, FStar_UInt128_mul_wide(f33, f40));
    FStar_UInt128_uint128 o43 =
      FStar_UInt128_add(o42, FStar_UInt128_mul_wide(f33, f41));
    FStar_UInt128_uint128 o04 =
      FStar_UInt128_add(o03, FStar_UInt128_mul_wide(f34, tmp21));
    FStar_UInt128_uint128 o141 =
      FStar_UInt128_add(o131, FStar_UInt128_mul_wide(f34, tmp22));
    FStar_UInt128_uint128 o241 =
      FStar_UInt128_add(o231, FStar_UInt128_mul_wide(f34, tmp23));
    FStar_UInt128_uint128 o34 =
      FStar_UInt128_add(o33, FStar_UInt128_mul_wide(f34, tmp24));
    FStar_UInt128_uint128 o44 =
      FStar_UInt128_add(o43, FStar_UInt128_mul_wide(f34, f40));
    FStar_UInt128_uint128 tmp_w20 = o04;
    FStar_UInt128_uint128 tmp_w21 = o141;
    FStar_UInt128_uint128 tmp_w22 = o241;
    FStar_UInt128_uint128 tmp_w23 = o34;
    FStar_UInt128_uint128 tmp_w24 = o44;
    FStar_UInt128_uint128 l_ =
      FStar_UInt128_add(tmp_w10, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp00 = FStar_UInt128_uint128_to_uint64(l_) & 0x7ffffffffffffULL;
    uint64_t c00 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_, 51U));
    FStar_UInt128_uint128 l_0 =
      FStar_UInt128_add(tmp_w11, FStar_UInt128_uint64_to_uint128(c00));
    uint64_t tmp10 = FStar_UInt128_uint128_to_uint64(l_0) & 0x7ffffffffffffULL;
    uint64_t c10 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_0, 51U));
    FStar_UInt128_uint128 l_1 =
      FStar_UInt128_add(tmp_w12, FStar_UInt128_uint64_to_uint128(c10));
    uint64_t tmp20 = FStar_UInt128_uint128_to_uint64(l_1) & 0x7ffffffffffffULL;
    uint64_t c20 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_1, 51U));
    FStar_UInt128_uint128 l_2 =
      FStar_UInt128_add(tmp_w13, FStar_UInt128_uint64_to_uint128(c20));
    uint64_t tmp30 = FStar_UInt128_uint128_to_uint64(l_2) & 0x7ffffffffffffULL;
    uint64_t c30 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_2, 51U));
    FStar_UInt128_uint128 l_3 =
      FStar_UInt128_add(tmp_w14, FStar_UInt128_uint64_to_uint128(c30));
    uint64_t tmp40 = FStar_UInt128_uint128_to_uint64(l_3) & 0x7ffffffffffffULL;
    uint64_t c40 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_3, 51U));
    uint64_t l_4 = tmp00 + c40 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c50 = l_4 >> 51U;
    uint64_t o100 = tmp0_;
    uint64_t o112 = tmp10 + c50;
    uint64_t o122 = tmp20;
    uint64_t o132 = tmp30;
    uint64_t o142 = tmp40;
    FStar_UInt128_uint128 l_5 =
      FStar_UInt128_add(tmp_w20, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp0 = FStar_UInt128_uint128_to_uint64(l_5) & 0x7ffffffffffffULL;
    uint64_t c0 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_5, 51U));
    FStar_UInt128_uint128 l_6 =
      FStar_UInt128_add(tmp_w21, FStar_UInt128_uint64_to_uint128(c0));
    uint64_t tmp1 = FStar_UInt128_uint128_to_uint64(l_6) & 0x7ffffffffffffULL;
    uint64_t c1 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_6, 51U));
    FStar_UInt128_uint128 l_7 =
      FStar_UInt128_add(tmp_w22, FStar_UInt128_uint64_to_uint128(c1));
    uint64_t tmp2 = FStar_UInt128_uint128_to_uint64(l_7) & 0x7ffffffffffffULL;
    uint64_t c2 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_7, 51U));
    FStar_UInt128_uint128 l_8 =
      FStar_UInt128_add(tmp_w23, FStar_UInt128_uint64_to_uint128(c2));
    uint64_t tmp3 = FStar_UInt128_uint128_to_uint64(l_8) & 0x7ffffffffffffULL;
    uint64_t c3 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_8, 51U));
    FStar_UInt128_uint128 l_9 =
      FStar_UInt128_add(tmp_w24, FStar_UInt128_uint64_to_uint128(c3));
    uint64_t tmp4 = FStar_UInt128_uint128_to_uint64(l_9) & 0x7ffffffffffffULL;
    uint64_t c4 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_9, 51U));
    uint64_t l_10 = tmp0 + c4 * 19ULL;
    uint64_t tmp0_0 = l_10 & 0x7ffffffffffffULL;
    uint64_t c5 = l_10 >> 51U;
    uint64_t o200 = tmp0_0;
    uint64_t o212 = tmp1 + c5;
    uint64_t o222 = tmp2;
    uint64_t o232 = tmp3;
    uint64_t o242 = tmp4;
    uint64_t o10 = o100;
    uint64_t o11 = o112;
    uint64_t o12 = o122;
    uint64_t o13 = o132;
    uint64_t o14 = o142;
    uint64_t o20 = o200;
    uint64_t o21 = o212;
    uint64_t o22 = o222;
    uint64_t o23 = o232;
    uint64_t o24 = o242;
    out[0U] = o10;
    out[1U] = o11;
    out[2U] = o12;
    out[3U] = o13;
    out[4U] = o14;
    out[5U] = o20;
    out[6U] = o21;
    out[7U] = o22;
    out[8U] = o23;
    out[9U] = o24;
}

inline void
Hacl_Impl_Curve25519_fsqr(uint64_t out[static 5], const uint64_t f[static 5])
{
    uint64_t f0 = f[0U];
    uint64_t f1 = f[1U];
    uint64_t f2 = f[2U];
    uint64_t f3 = f[3U];
    uint64_t f4 = f[4U];
    uint64_t d0 = 2ULL * f0;
    uint64_t d1 = 2ULL * f1;
    uint64_t d2 = 38ULL * f2;
    uint64_t d3 = 19ULL * f3;
    uint64_t d419 = 19ULL * f4;
    uint64_t d4 = 2ULL * d419;
    FStar_UInt128_uint128 s0 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(f0, f0),
                                          FStar_UInt128_mul_wide(d4, f1)),
                        FStar_UInt128_mul_wide(d2, f3));
    FStar_UInt128_uint128 s1 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f1),
                                          FStar_UInt128_mul_wide(d4, f2)),
                        FStar_UInt128_mul_wide(d3, f3));
    FStar_UInt128_uint128 s2 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f2),
                                          FStar_UInt128_mul_wide(f1, f1)),
                        FStar_UInt128_mul_wide(d4, f3));
    FStar_UInt128_uint128 s3 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f3),
                                          FStar_UInt128_mul_wide(d1, f2)),
                        FStar_UInt128_mul_wide(f4, d419));
    FStar_UInt128_uint128 s4 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f4),
                                          FStar_UInt128_mul_wide(d1, f3)),
                        FStar_UInt128_mul_wide(f2, f2));
    FStar_UInt128_uint128 o00 = s0;
    FStar_UInt128_uint128 o10 = s1;
    FStar_UInt128_uint128 o20 = s2;
    FStar_UInt128_uint128 o30 = s3;
    FStar_UInt128_uint128 o40 = s4;
    FStar_UInt128_uint128 l_ =
      FStar_UInt128_add(o00, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp0 = FStar_UInt128_uint128_to_uint64(l_) & 0x7ffffffffffffULL;
    uint64_t c0 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_, 51U));
    FStar_UInt128_uint128 l_0 =
      FStar_UInt128_add(o10, FStar_UInt128_uint64_to_uint128(c0));
    uint64_t tmp1 = FStar_UInt128_uint128_to_uint64(l_0) & 0x7ffffffffffffULL;
    uint64_t c1 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_0, 51U));
    FStar_UInt128_uint128 l_1 =
      FStar_UInt128_add(o20, FStar_UInt128_uint64_to_uint128(c1));
    uint64_t tmp2 = FStar_UInt128_uint128_to_uint64(l_1) & 0x7ffffffffffffULL;
    uint64_t c2 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_1, 51U));
    FStar_UInt128_uint128 l_2 =
      FStar_UInt128_add(o30, FStar_UInt128_uint64_to_uint128(c2));
    uint64_t tmp3 = FStar_UInt128_uint128_to_uint64(l_2) & 0x7ffffffffffffULL;
    uint64_t c3 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_2, 51U));
    FStar_UInt128_uint128 l_3 =
      FStar_UInt128_add(o40, FStar_UInt128_uint64_to_uint128(c3));
    uint64_t tmp4 = FStar_UInt128_uint128_to_uint64(l_3) & 0x7ffffffffffffULL;
    uint64_t c4 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_3, 51U));
    uint64_t l_4 = tmp0 + c4 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c5 = l_4 >> 51U;
    uint64_t o0 = tmp0_;
    uint64_t o1 = tmp1 + c5;
    uint64_t o2 = tmp2;
    uint64_t o3 = tmp3;
    uint64_t o4 = tmp4;
    out[0U] = o0;
    out[1U] = o1;
    out[2U] = o2;
    out[3U] = o3;
    out[4U] = o4;
}

inline void
Hacl_Impl_Curve25519_fsqr2(uint64_t out[static 10], uint64_t f[static 10])
{
    uint64_t f10 = f[0U];
    uint64_t f11 = f[1U];
    uint64_t f12 = f[2U];
    uint64_t f13 = f[3U];
    uint64_t f14 = f[4U];
    uint64_t f20 = f[5U];
    uint64_t f21 = f[6U];
    uint64_t f22 = f[7U];
    uint64_t f23 = f[8U];
    uint64_t f24 = f[9U];
    uint64_t d00 = 2ULL * f10;
    uint64_t d10 = 2ULL * f11;
    uint64_t d20 = 38ULL * f12;
    uint64_t d30 = 19ULL * f13;
    uint64_t d4190 = 19ULL * f14;
    uint64_t d40 = 2ULL * d4190;
    FStar_UInt128_uint128 s00 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(f10, f10),
                                          FStar_UInt128_mul_wide(d40, f11)),
                        FStar_UInt128_mul_wide(d20, f13));
    FStar_UInt128_uint128 s10 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d00, f11),
                                          FStar_UInt128_mul_wide(d40, f12)),
                        FStar_UInt128_mul_wide(d30, f13));
    FStar_UInt128_uint128 s20 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d00, f12),
                                          FStar_UInt128_mul_wide(f11, f11)),
                        FStar_UInt128_mul_wide(d40, f13));
    FStar_UInt128_uint128 s30 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d00, f13),
                                          FStar_UInt128_mul_wide(d10, f12)),
                        FStar_UInt128_mul_wide(f14, d4190));
    FStar_UInt128_uint128 s40 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d00, f14),
                                          FStar_UInt128_mul_wide(d10, f13)),
                        FStar_UInt128_mul_wide(f12, f12));
    FStar_UInt128_uint128 o100 = s00;
    FStar_UInt128_uint128 o110 = s10;
    FStar_UInt128_uint128 o120 = s20;
    FStar_UInt128_uint128 o130 = s30;
    FStar_UInt128_uint128 o140 = s40;
    uint64_t d0 = 2ULL * f20;
    uint64_t d1 = 2ULL * f21;
    uint64_t d2 = 38ULL * f22;
    uint64_t d3 = 19ULL * f23;
    uint64_t d419 = 19ULL * f24;
    uint64_t d4 = 2ULL * d419;
    FStar_UInt128_uint128 s0 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(f20, f20),
                                          FStar_UInt128_mul_wide(d4, f21)),
                        FStar_UInt128_mul_wide(d2, f23));
    FStar_UInt128_uint128 s1 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f21),
                                          FStar_UInt128_mul_wide(d4, f22)),
                        FStar_UInt128_mul_wide(d3, f23));
    FStar_UInt128_uint128 s2 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f22),
                                          FStar_UInt128_mul_wide(f21, f21)),
                        FStar_UInt128_mul_wide(d4, f23));
    FStar_UInt128_uint128 s3 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f23),
                                          FStar_UInt128_mul_wide(d1, f22)),
                        FStar_UInt128_mul_wide(f24, d419));
    FStar_UInt128_uint128 s4 =
      FStar_UInt128_add(FStar_UInt128_add(FStar_UInt128_mul_wide(d0, f24),
                                          FStar_UInt128_mul_wide(d1, f23)),
                        FStar_UInt128_mul_wide(f22, f22));
    FStar_UInt128_uint128 o200 = s0;
    FStar_UInt128_uint128 o210 = s1;
    FStar_UInt128_uint128 o220 = s2;
    FStar_UInt128_uint128 o230 = s3;
    FStar_UInt128_uint128 o240 = s4;
    FStar_UInt128_uint128 l_ =
      FStar_UInt128_add(o100, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp00 = FStar_UInt128_uint128_to_uint64(l_) & 0x7ffffffffffffULL;
    uint64_t c00 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_, 51U));
    FStar_UInt128_uint128 l_0 =
      FStar_UInt128_add(o110, FStar_UInt128_uint64_to_uint128(c00));
    uint64_t tmp10 = FStar_UInt128_uint128_to_uint64(l_0) & 0x7ffffffffffffULL;
    uint64_t c10 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_0, 51U));
    FStar_UInt128_uint128 l_1 =
      FStar_UInt128_add(o120, FStar_UInt128_uint64_to_uint128(c10));
    uint64_t tmp20 = FStar_UInt128_uint128_to_uint64(l_1) & 0x7ffffffffffffULL;
    uint64_t c20 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_1, 51U));
    FStar_UInt128_uint128 l_2 =
      FStar_UInt128_add(o130, FStar_UInt128_uint64_to_uint128(c20));
    uint64_t tmp30 = FStar_UInt128_uint128_to_uint64(l_2) & 0x7ffffffffffffULL;
    uint64_t c30 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_2, 51U));
    FStar_UInt128_uint128 l_3 =
      FStar_UInt128_add(o140, FStar_UInt128_uint64_to_uint128(c30));
    uint64_t tmp40 = FStar_UInt128_uint128_to_uint64(l_3) & 0x7ffffffffffffULL;
    uint64_t c40 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_3, 51U));
    uint64_t l_4 = tmp00 + c40 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c50 = l_4 >> 51U;
    uint64_t o101 = tmp0_;
    uint64_t o111 = tmp10 + c50;
    uint64_t o121 = tmp20;
    uint64_t o131 = tmp30;
    uint64_t o141 = tmp40;
    FStar_UInt128_uint128 l_5 =
      FStar_UInt128_add(o200, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp0 = FStar_UInt128_uint128_to_uint64(l_5) & 0x7ffffffffffffULL;
    uint64_t c0 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_5, 51U));
    FStar_UInt128_uint128 l_6 =
      FStar_UInt128_add(o210, FStar_UInt128_uint64_to_uint128(c0));
    uint64_t tmp1 = FStar_UInt128_uint128_to_uint64(l_6) & 0x7ffffffffffffULL;
    uint64_t c1 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_6, 51U));
    FStar_UInt128_uint128 l_7 =
      FStar_UInt128_add(o220, FStar_UInt128_uint64_to_uint128(c1));
    uint64_t tmp2 = FStar_UInt128_uint128_to_uint64(l_7) & 0x7ffffffffffffULL;
    uint64_t c2 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_7, 51U));
    FStar_UInt128_uint128 l_8 =
      FStar_UInt128_add(o230, FStar_UInt128_uint64_to_uint128(c2));
    uint64_t tmp3 = FStar_UInt128_uint128_to_uint64(l_8) & 0x7ffffffffffffULL;
    uint64_t c3 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_8, 51U));
    FStar_UInt128_uint128 l_9 =
      FStar_UInt128_add(o240, FStar_UInt128_uint64_to_uint128(c3));
    uint64_t tmp4 = FStar_UInt128_uint128_to_uint64(l_9) & 0x7ffffffffffffULL;
    uint64_t c4 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_9, 51U));
    uint64_t l_10 = tmp0 + c4 * 19ULL;
    uint64_t tmp0_0 = l_10 & 0x7ffffffffffffULL;
    uint64_t c5 = l_10 >> 51U;
    uint64_t o201 = tmp0_0;
    uint64_t o211 = tmp1 + c5;
    uint64_t o221 = tmp2;
    uint64_t o231 = tmp3;
    uint64_t o241 = tmp4;
    uint64_t o10 = o101;
    uint64_t o11 = o111;
    uint64_t o12 = o121;
    uint64_t o13 = o131;
    uint64_t o14 = o141;
    uint64_t o20 = o201;
    uint64_t o21 = o211;
    uint64_t o22 = o221;
    uint64_t o23 = o231;
    uint64_t o24 = o241;
    out[0U] = o10;
    out[1U] = o11;
    out[2U] = o12;
    out[3U] = o13;
    out[4U] = o14;
    out[5U] = o20;
    out[6U] = o21;
    out[7U] = o22;
    out[8U] = o23;
    out[9U] = o24;
}

void
Hacl_Impl_Curve25519_fsub(uint64_t out[static 5],
                          const uint64_t f1[static 5],
                          const uint64_t f2[static 5])
{
    uint64_t f10 = f1[0U];
    uint64_t f20 = f2[0U];
    uint64_t f11 = f1[1U];
    uint64_t f21 = f2[1U];
    uint64_t f12 = f1[2U];
    uint64_t f22 = f2[2U];
    uint64_t f13 = f1[3U];
    uint64_t f23 = f2[3U];
    uint64_t f14 = f1[4U];
    uint64_t f24 = f2[4U];
    out[0U] = f10 + 0x3fffffffffff68ULL - f20;
    out[1U] = f11 + 0x3ffffffffffff8ULL - f21;
    out[2U] = f12 + 0x3ffffffffffff8ULL - f22;
    out[3U] = f13 + 0x3ffffffffffff8ULL - f23;
    out[4U] = f14 + 0x3ffffffffffff8ULL - f24;
}

void
Hacl_Impl_Curve25519_fmul1(uint64_t out[static 5],
                           const uint64_t f1[static 5],
                           uint64_t f2)
{
    uint64_t f10 = f1[0U];
    uint64_t f11 = f1[1U];
    uint64_t f12 = f1[2U];
    uint64_t f13 = f1[3U];
    uint64_t f14 = f1[4U];
    FStar_UInt128_uint128 tmp_w0 = FStar_UInt128_mul_wide(f2, f10);
    FStar_UInt128_uint128 tmp_w1 = FStar_UInt128_mul_wide(f2, f11);
    FStar_UInt128_uint128 tmp_w2 = FStar_UInt128_mul_wide(f2, f12);
    FStar_UInt128_uint128 tmp_w3 = FStar_UInt128_mul_wide(f2, f13);
    FStar_UInt128_uint128 tmp_w4 = FStar_UInt128_mul_wide(f2, f14);
    FStar_UInt128_uint128 l_ =
      FStar_UInt128_add(tmp_w0, FStar_UInt128_uint64_to_uint128(0ULL));
    uint64_t tmp0 = FStar_UInt128_uint128_to_uint64(l_) & 0x7ffffffffffffULL;
    uint64_t c0 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_, 51U));
    FStar_UInt128_uint128 l_0 =
      FStar_UInt128_add(tmp_w1, FStar_UInt128_uint64_to_uint128(c0));
    uint64_t tmp1 = FStar_UInt128_uint128_to_uint64(l_0) & 0x7ffffffffffffULL;
    uint64_t c1 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_0, 51U));
    FStar_UInt128_uint128 l_1 =
      FStar_UInt128_add(tmp_w2, FStar_UInt128_uint64_to_uint128(c1));
    uint64_t tmp2 = FStar_UInt128_uint128_to_uint64(l_1) & 0x7ffffffffffffULL;
    uint64_t c2 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_1, 51U));
    FStar_UInt128_uint128 l_2 =
      FStar_UInt128_add(tmp_w3, FStar_UInt128_uint64_to_uint128(c2));
    uint64_t tmp3 = FStar_UInt128_uint128_to_uint64(l_2) & 0x7ffffffffffffULL;
    uint64_t c3 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_2, 51U));
    FStar_UInt128_uint128 l_3 =
      FStar_UInt128_add(tmp_w4, FStar_UInt128_uint64_to_uint128(c3));
    uint64_t tmp4 = FStar_UInt128_uint128_to_uint64(l_3) & 0x7ffffffffffffULL;
    uint64_t c4 =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(l_3, 51U));
    uint64_t l_4 = tmp0 + c4 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c5 = l_4 >> 51U;
    uint64_t o0 = tmp0_;
    uint64_t o1 = tmp1 + c5;
    uint64_t o2 = tmp2;
    uint64_t o3 = tmp3;
    uint64_t o4 = tmp4;
    out[0U] = o0;
    out[1U] = o1;
    out[2U] = o2;
    out[3U] = o3;
    out[4U] = o4;
}

void
Hacl_Impl_Curve25519_store_felem(uint64_t u64s[static 4],
                                 const uint64_t f[static 5])
{
    uint64_t f0 = f[0U];
    uint64_t f1 = f[1U];
    uint64_t f2 = f[2U];
    uint64_t f3 = f[3U];
    uint64_t f4 = f[4U];
    uint64_t l_ = f0 + 0ULL;
    uint64_t tmp0 = l_ & 0x7ffffffffffffULL;
    uint64_t c0 = l_ >> 51U;
    uint64_t l_0 = f1 + c0;
    uint64_t tmp1 = l_0 & 0x7ffffffffffffULL;
    uint64_t c1 = l_0 >> 51U;
    uint64_t l_1 = f2 + c1;
    uint64_t tmp2 = l_1 & 0x7ffffffffffffULL;
    uint64_t c2 = l_1 >> 51U;
    uint64_t l_2 = f3 + c2;
    uint64_t tmp3 = l_2 & 0x7ffffffffffffULL;
    uint64_t c3 = l_2 >> 51U;
    uint64_t l_3 = f4 + c3;
    uint64_t tmp4 = l_3 & 0x7ffffffffffffULL;
    uint64_t c4 = l_3 >> 51U;
    uint64_t l_4 = tmp0 + c4 * 19ULL;
    uint64_t tmp0_ = l_4 & 0x7ffffffffffffULL;
    uint64_t c5 = l_4 >> 51U;
    uint64_t f01 = tmp0_;
    uint64_t f11 = tmp1 + c5;
    uint64_t f21 = tmp2;
    uint64_t f31 = tmp3;
    uint64_t f41 = tmp4;
    uint64_t m0 = FStar_UInt64_gte_mask(f01, 0x7ffffffffffedULL);
    uint64_t m1 = FStar_UInt64_eq_mask(f11, 0x7ffffffffffffULL);
    uint64_t m2 = FStar_UInt64_eq_mask(f21, 0x7ffffffffffffULL);
    uint64_t m3 = FStar_UInt64_eq_mask(f31, 0x7ffffffffffffULL);
    uint64_t m4 = FStar_UInt64_eq_mask(f41, 0x7ffffffffffffULL);
    uint64_t mask = (((m0 & m1) & m2) & m3) & m4;
    uint64_t f0_ = f01 - (mask & 0x7ffffffffffedULL);
    uint64_t f1_ = f11 - (mask & 0x7ffffffffffffULL);
    uint64_t f2_ = f21 - (mask & 0x7ffffffffffffULL);
    uint64_t f3_ = f31 - (mask & 0x7ffffffffffffULL);
    uint64_t f4_ = f41 - (mask & 0x7ffffffffffffULL);
    uint64_t f02 = f0_;
    uint64_t f12 = f1_;
    uint64_t f22 = f2_;
    uint64_t f32 = f3_;
    uint64_t f42 = f4_;
    uint64_t o00 = f02 | f12 << 51U;
    uint64_t o10 = f12 >> 13U | f22 << 38U;
    uint64_t o20 = f22 >> 26U | f32 << 25U;
    uint64_t o30 = f32 >> 39U | f42 << 12U;
    uint64_t o0 = o00;
    uint64_t o1 = o10;
    uint64_t o2 = o20;
    uint64_t o3 = o30;
    u64s[0U] = o0;
    u64s[1U] = o1;
    u64s[2U] = o2;
    u64s[3U] = o3;
}

void
Hacl_Impl_Curve25519_cswap2(uint64_t bit, uint64_t* p1, uint64_t* p2)
{
    uint64_t mask = 0ULL - bit;
    KRML_MAYBE_FOR10(i, 0U, 10U, 1U, uint64_t dummy = mask & (p1[i] ^ p2[i]);
                     p1[i] = p1[i] ^ dummy;
                     p2[i] = p2[i] ^ dummy;);
}

static void
point_add_and_double(uint64_t* q,
                     uint64_t* p01_tmp1,
                     FStar_UInt128_uint128* tmp2)
{
    uint64_t* nq = p01_tmp1;
    uint64_t* nq_p1 = p01_tmp1 + 10U;
    uint64_t* tmp1 = p01_tmp1 + 20U;
    uint64_t* x1 = q;
    uint64_t* x2 = nq;
    uint64_t* z2 = nq + 5U;
    uint64_t* z3 = nq_p1 + 5U;
    uint64_t* a = tmp1;
    uint64_t* b = tmp1 + 5U;
    uint64_t* ab = tmp1;
    uint64_t* dc = tmp1 + 10U;
    Hacl_Impl_Curve25519_fadd(a, x2, z2);
    Hacl_Impl_Curve25519_fsub(b, x2, z2);
    uint64_t* x3 = nq_p1;
    uint64_t* z31 = nq_p1 + 5U;
    uint64_t* d0 = dc;
    uint64_t* c0 = dc + 5U;
    Hacl_Impl_Curve25519_fadd(c0, x3, z31);
    Hacl_Impl_Curve25519_fsub(d0, x3, z31);
    Hacl_Impl_Curve25519_fmul2(dc, dc, ab);
    Hacl_Impl_Curve25519_fadd(x3, d0, c0);
    Hacl_Impl_Curve25519_fsub(z31, d0, c0);
    uint64_t* a1 = tmp1;
    uint64_t* b1 = tmp1 + 5U;
    uint64_t* d = tmp1 + 10U;
    uint64_t* c = tmp1 + 15U;
    uint64_t* ab1 = tmp1;
    uint64_t* dc1 = tmp1 + 10U;
    Hacl_Impl_Curve25519_fsqr2(dc1, ab1);
    Hacl_Impl_Curve25519_fsqr2(nq_p1, nq_p1);
    a1[0U] = c[0U];
    a1[1U] = c[1U];
    a1[2U] = c[2U];
    a1[3U] = c[3U];
    a1[4U] = c[4U];
    Hacl_Impl_Curve25519_fsub(c, d, c);
    Hacl_Impl_Curve25519_fmul1(b1, c, 121665ULL);
    Hacl_Impl_Curve25519_fadd(b1, b1, d);
    Hacl_Impl_Curve25519_fmul2(nq, dc1, ab1);
    Hacl_Impl_Curve25519_fmul(z3, z3, x1);
}

static void
point_double(uint64_t* nq,
             uint64_t tmp1[static 20],
             FStar_UInt128_uint128* tmp2)
{
    uint64_t* x2 = nq;
    uint64_t* z2 = nq + 5U;
    uint64_t* a = tmp1;
    uint64_t* b = tmp1 + 5U;
    uint64_t* d = tmp1 + 10U;
    uint64_t* c = tmp1 + 15U;
    uint64_t* ab = tmp1;
    uint64_t* dc = tmp1 + 10U;
    Hacl_Impl_Curve25519_fadd(a, x2, z2);
    Hacl_Impl_Curve25519_fsub(b, x2, z2);
    Hacl_Impl_Curve25519_fsqr2(dc, ab);
    a[0U] = c[0U];
    a[1U] = c[1U];
    a[2U] = c[2U];
    a[3U] = c[3U];
    a[4U] = c[4U];
    Hacl_Impl_Curve25519_fsub(c, d, c);
    Hacl_Impl_Curve25519_fmul1(b, c, 121665ULL);
    Hacl_Impl_Curve25519_fadd(b, b, d);
    Hacl_Impl_Curve25519_fmul2(nq, dc, ab);
}

static void
montgomery_ladder(uint64_t* out,
                  const uint8_t key[static 32],
                  uint64_t init[static 10])
{
    FStar_UInt128_uint128 tmp2[10U];
    for (uint32_t _i = 0U; _i < 10U; ++_i)
        tmp2[_i] = FStar_UInt128_uint64_to_uint128(0ULL);
    uint64_t p01_tmp1_swap[41U] = { 0U };
    uint64_t* p0 = p01_tmp1_swap;
    uint64_t* p01 = p01_tmp1_swap;
    uint64_t* p03 = p01;
    uint64_t* p11 = p01 + 10U;
    memcpy(p11, init, 10U * sizeof(uint64_t));
    uint64_t* x0 = p03;
    uint64_t* z0 = p03 + 5U;
    x0[0U] = 1ULL;
    x0[1U] = 0ULL;
    x0[2U] = 0ULL;
    x0[3U] = 0ULL;
    x0[4U] = 0ULL;
    z0[0U] = 0ULL;
    z0[1U] = 0ULL;
    z0[2U] = 0ULL;
    z0[3U] = 0ULL;
    z0[4U] = 0ULL;
    uint64_t* p01_tmp1 = p01_tmp1_swap;
    uint64_t* p01_tmp11 = p01_tmp1_swap;
    uint64_t* nq1 = p01_tmp1_swap;
    uint64_t* nq_p11 = p01_tmp1_swap + 10U;
    uint64_t* swap = p01_tmp1_swap + 40U;
    Hacl_Impl_Curve25519_cswap2(1ULL, nq1, nq_p11);
    point_add_and_double(init, p01_tmp11, tmp2);
    swap[0U] = 1ULL;
    for (uint32_t i = 0U; i < 251U; i++) {
        uint64_t* p01_tmp12 = p01_tmp1_swap;
        uint64_t* swap1 = p01_tmp1_swap + 40U;
        uint64_t* nq2 = p01_tmp12;
        uint64_t* nq_p12 = p01_tmp12 + 10U;
        uint64_t bit =
          (uint64_t)((uint32_t)key[(253U - i) / 8U] >> (253U - i) % 8U & 1U);
        uint64_t sw = swap1[0U] ^ bit;
        Hacl_Impl_Curve25519_cswap2(sw, nq2, nq_p12);
        point_add_and_double(init, p01_tmp12, tmp2);
        swap1[0U] = bit;
    }
    uint64_t sw = swap[0U];
    Hacl_Impl_Curve25519_cswap2(sw, nq1, nq_p11);
    uint64_t* nq10 = p01_tmp1;
    uint64_t* tmp1 = p01_tmp1 + 20U;
    point_double(nq10, tmp1, tmp2);
    point_double(nq10, tmp1, tmp2);
    point_double(nq10, tmp1, tmp2);
    memcpy(out, p0, 10U * sizeof(uint64_t));
}

void
Hacl_Curve25519_fsquare_times(uint64_t o[static 5],
                              const uint64_t inp[static 5],
                              FStar_UInt128_uint128* tmp,
                              uint32_t n)
{
    Hacl_Impl_Curve25519_fsqr(o, inp);
    for (uint32_t i = 0U; i < n - 1U; i++) {
        Hacl_Impl_Curve25519_fsqr(o, o);
    }
}

void
Hacl_Curve25519_finv(uint64_t o[static 5],
                     const uint64_t i[static 5],
                     FStar_UInt128_uint128* tmp)
{
    uint64_t t1[20U] = { 0U };
    uint64_t* a1 = t1;
    uint64_t* b1 = t1 + 5U;
    uint64_t* t010 = t1 + 15U;
    FStar_UInt128_uint128* tmp10 = tmp;
    Hacl_Curve25519_fsquare_times(a1, i, tmp10, 1U);
    Hacl_Curve25519_fsquare_times(t010, a1, tmp10, 2U);
    Hacl_Impl_Curve25519_fmul(b1, t010, i);
    Hacl_Impl_Curve25519_fmul(a1, b1, a1);
    Hacl_Curve25519_fsquare_times(t010, a1, tmp10, 1U);
    Hacl_Impl_Curve25519_fmul(b1, t010, b1);
    Hacl_Curve25519_fsquare_times(t010, b1, tmp10, 5U);
    Hacl_Impl_Curve25519_fmul(b1, t010, b1);
    uint64_t* b10 = t1 + 5U;
    uint64_t* c10 = t1 + 10U;
    uint64_t* t011 = t1 + 15U;
    FStar_UInt128_uint128* tmp11 = tmp;
    Hacl_Curve25519_fsquare_times(t011, b10, tmp11, 10U);
    Hacl_Impl_Curve25519_fmul(c10, t011, b10);
    Hacl_Curve25519_fsquare_times(t011, c10, tmp11, 20U);
    Hacl_Impl_Curve25519_fmul(t011, t011, c10);
    Hacl_Curve25519_fsquare_times(t011, t011, tmp11, 10U);
    Hacl_Impl_Curve25519_fmul(b10, t011, b10);
    Hacl_Curve25519_fsquare_times(t011, b10, tmp11, 50U);
    Hacl_Impl_Curve25519_fmul(c10, t011, b10);
    uint64_t* b11 = t1 + 5U;
    uint64_t* c1 = t1 + 10U;
    uint64_t* t01 = t1 + 15U;
    FStar_UInt128_uint128* tmp1 = tmp;
    Hacl_Curve25519_fsquare_times(t01, c1, tmp1, 100U);
    Hacl_Impl_Curve25519_fmul(t01, t01, c1);
    Hacl_Curve25519_fsquare_times(t01, t01, tmp1, 50U);
    Hacl_Impl_Curve25519_fmul(t01, t01, b11);
    Hacl_Curve25519_fsquare_times(t01, t01, tmp1, 5U);
    uint64_t* a = t1;
    uint64_t* t0 = t1 + 15U;
    Hacl_Impl_Curve25519_fmul(o, t0, a);
}

static void
encode_point(uint8_t o[static 32], const uint64_t i[static 10])
{
    const uint64_t* x = i;
    const uint64_t* z = i + 5U;
    uint64_t tmp[5U] = { 0U };
    uint64_t u64s[4U] = { 0U };
    FStar_UInt128_uint128 tmp_w[10U];
    for (uint32_t _i = 0U; _i < 10U; ++_i)
        tmp_w[_i] = FStar_UInt128_uint64_to_uint128(0ULL);
    Hacl_Curve25519_finv(tmp, z, tmp_w);
    Hacl_Impl_Curve25519_fmul(tmp, tmp, x);
    Hacl_Impl_Curve25519_store_felem(u64s, tmp);
    KRML_MAYBE_FOR4(i0, 0U, 4U, 1U, WRITE64LE(o + i0 * 8U, u64s[i0]););
}

/**
  Compute the scalar multiple of a point.

  @param out Pointer to 32 bytes of memory, allocated by the caller,
      where the resulting point is written to.

  @param priv Pointer to 32 bytes of memory where the secret/private key
      is read from.

  @param pub Pointer to 32 bytes of memory where the public point is
      read from.
*/
void
Hacl_Curve25519_scalarmult(uint8_t out[32],
                           const uint8_t priv[32],
                           const uint8_t pub[32])
{
    uint64_t init[10U] = { 0U };
    uint64_t tmp[4U] = { 0U };
    KRML_MAYBE_FOR4(i, 0U, 4U, 1U, uint64_t* os = tmp;
                    const uint8_t* bj = pub + i * 8U;
                    uint64_t u = READ64LE(bj);
                    uint64_t r = u;
                    uint64_t x = r;
                    os[i] = x;);
    uint64_t tmp3 = tmp[3U];
    tmp[3U] = tmp3 & 0x7fffffffffffffffULL;
    uint64_t* x = init;
    uint64_t* z = init + 5U;
    z[0U] = 1ULL;
    z[1U] = 0ULL;
    z[2U] = 0ULL;
    z[3U] = 0ULL;
    z[4U] = 0ULL;
    uint64_t f0l = tmp[0U] & 0x7ffffffffffffULL;
    uint64_t f0h = tmp[0U] >> 51U;
    uint64_t f1l = (tmp[1U] & 0x3fffffffffULL) << 13U;
    uint64_t f1h = tmp[1U] >> 38U;
    uint64_t f2l = (tmp[2U] & 0x1ffffffULL) << 26U;
    uint64_t f2h = tmp[2U] >> 25U;
    uint64_t f3l = (tmp[3U] & 0xfffULL) << 39U;
    uint64_t f3h = tmp[3U] >> 12U;
    x[0U] = f0l;
    x[1U] = f0h | f1l;
    x[2U] = f1h | f2l;
    x[3U] = f2h | f3l;
    x[4U] = f3h;
    montgomery_ladder(init, priv, init);
    encode_point(out, init);
}

/**
  Calculate a public point from a secret/private key.

  This computes a scalar multiplication of the secret/private key with
  the curve's basepoint.

  @param pub Pointer to 32 bytes of memory, allocated by the caller,
      where the resulting point is written to.

  @param priv Pointer to 32 bytes of memory where the secret/private key
      is read from.
*/
void
Hacl_Curve25519_secret_to_public(uint8_t pub[32], const uint8_t priv[32])
{
    uint8_t basepoint[32U] = { 0U };
    for (uint32_t i = 0U; i < 32U; i++) {
        uint8_t* os = basepoint;
        uint8_t x = g25519[i];
        os[i] = x;
    }
    Hacl_Curve25519_scalarmult(pub, priv, basepoint);
}

/**
  Execute the diffie-hellmann key exchange.

  @param out Pointer to 32 bytes of memory, allocated by the caller,
      where the resulting point is written to.

  @param priv Pointer to 32 bytes of memory where **our** secret/private
      key is read from.

  @param pub Pointer to 32 bytes of memory where **their** public point
      is read from.
*/
bool
Hacl_Curve25519_ecdh(uint8_t out[32],
                     const uint8_t priv[32],
                     const uint8_t pub[32])
{
    uint8_t zeros[32U] = { 0U };
    Hacl_Curve25519_scalarmult(out, priv, pub);
    uint8_t res = 255U;
    for (uint32_t i = 0U; i < 32U; i++) {
        uint8_t uu____0 = FStar_UInt8_eq_mask(out[i], zeros[i]);
        res = (uint32_t)uu____0 & (uint32_t)res;
    }
    uint8_t z = res;
    bool r = z == 255U;
    return !r;
}
