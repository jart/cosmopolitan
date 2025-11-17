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
#include "third_party/haclstar/loop.h"
#include "third_party/haclstar/macros.h"
#include "third_party/haclstar/mask.h"
#include "third_party/haclstar/optional.h"
#include "third_party/haclstar/simd128.h"
__static_yoink("hacl_star_notice");

typedef struct Hacl_MAC_Poly1305_Simd128_state_t_s
{
    Lib_IntVector_Intrinsics_vec128* block_state;
    uint8_t* buf;
    uint64_t total_len;
    uint8_t* p_key;
} Hacl_MAC_Poly1305_Simd128_state_t;

void
Hacl_MAC_Poly1305_Simd128_load_acc2(Lib_IntVector_Intrinsics_vec128* acc,
                                    uint8_t* b)
{
    KRML_PRE_ALIGN(16)
    Lib_IntVector_Intrinsics_vec128 e[5U] KRML_POST_ALIGN(16) = { 0U };
    Lib_IntVector_Intrinsics_vec128 b1 =
      Lib_IntVector_Intrinsics_vec128_load64_le(b);
    Lib_IntVector_Intrinsics_vec128 b2 =
      Lib_IntVector_Intrinsics_vec128_load64_le(b + 16U);
    Lib_IntVector_Intrinsics_vec128 lo =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(b1, b2);
    Lib_IntVector_Intrinsics_vec128 hi =
      Lib_IntVector_Intrinsics_vec128_interleave_high64(b1, b2);
    Lib_IntVector_Intrinsics_vec128 f00 = Lib_IntVector_Intrinsics_vec128_and(
      lo, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f10 = Lib_IntVector_Intrinsics_vec128_and(
      Lib_IntVector_Intrinsics_vec128_shift_right64(lo, 26U),
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f20 = Lib_IntVector_Intrinsics_vec128_or(
      Lib_IntVector_Intrinsics_vec128_shift_right64(lo, 52U),
      Lib_IntVector_Intrinsics_vec128_shift_left64(
        Lib_IntVector_Intrinsics_vec128_and(
          hi, Lib_IntVector_Intrinsics_vec128_load64(0x3fffULL)),
        12U));
    Lib_IntVector_Intrinsics_vec128 f30 = Lib_IntVector_Intrinsics_vec128_and(
      Lib_IntVector_Intrinsics_vec128_shift_right64(hi, 14U),
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f40 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(hi, 40U);
    Lib_IntVector_Intrinsics_vec128 f02 = f00;
    Lib_IntVector_Intrinsics_vec128 f12 = f10;
    Lib_IntVector_Intrinsics_vec128 f22 = f20;
    Lib_IntVector_Intrinsics_vec128 f32 = f30;
    Lib_IntVector_Intrinsics_vec128 f42 = f40;
    e[0U] = f02;
    e[1U] = f12;
    e[2U] = f22;
    e[3U] = f32;
    e[4U] = f42;
    uint64_t b10 = 0x1000000ULL;
    Lib_IntVector_Intrinsics_vec128 mask =
      Lib_IntVector_Intrinsics_vec128_load64(b10);
    Lib_IntVector_Intrinsics_vec128 f43 = e[4U];
    e[4U] = Lib_IntVector_Intrinsics_vec128_or(f43, mask);
    Lib_IntVector_Intrinsics_vec128 acc0 = acc[0U];
    Lib_IntVector_Intrinsics_vec128 acc1 = acc[1U];
    Lib_IntVector_Intrinsics_vec128 acc2 = acc[2U];
    Lib_IntVector_Intrinsics_vec128 acc3 = acc[3U];
    Lib_IntVector_Intrinsics_vec128 acc4 = acc[4U];
    Lib_IntVector_Intrinsics_vec128 e0 = e[0U];
    Lib_IntVector_Intrinsics_vec128 e1 = e[1U];
    Lib_IntVector_Intrinsics_vec128 e2 = e[2U];
    Lib_IntVector_Intrinsics_vec128 e3 = e[3U];
    Lib_IntVector_Intrinsics_vec128 e4 = e[4U];
    Lib_IntVector_Intrinsics_vec128 f0 =
      Lib_IntVector_Intrinsics_vec128_insert64(acc0, 0ULL, 1U);
    Lib_IntVector_Intrinsics_vec128 f1 =
      Lib_IntVector_Intrinsics_vec128_insert64(acc1, 0ULL, 1U);
    Lib_IntVector_Intrinsics_vec128 f2 =
      Lib_IntVector_Intrinsics_vec128_insert64(acc2, 0ULL, 1U);
    Lib_IntVector_Intrinsics_vec128 f3 =
      Lib_IntVector_Intrinsics_vec128_insert64(acc3, 0ULL, 1U);
    Lib_IntVector_Intrinsics_vec128 f4 =
      Lib_IntVector_Intrinsics_vec128_insert64(acc4, 0ULL, 1U);
    Lib_IntVector_Intrinsics_vec128 f01 =
      Lib_IntVector_Intrinsics_vec128_add64(f0, e0);
    Lib_IntVector_Intrinsics_vec128 f11 =
      Lib_IntVector_Intrinsics_vec128_add64(f1, e1);
    Lib_IntVector_Intrinsics_vec128 f21 =
      Lib_IntVector_Intrinsics_vec128_add64(f2, e2);
    Lib_IntVector_Intrinsics_vec128 f31 =
      Lib_IntVector_Intrinsics_vec128_add64(f3, e3);
    Lib_IntVector_Intrinsics_vec128 f41 =
      Lib_IntVector_Intrinsics_vec128_add64(f4, e4);
    Lib_IntVector_Intrinsics_vec128 acc01 = f01;
    Lib_IntVector_Intrinsics_vec128 acc11 = f11;
    Lib_IntVector_Intrinsics_vec128 acc21 = f21;
    Lib_IntVector_Intrinsics_vec128 acc31 = f31;
    Lib_IntVector_Intrinsics_vec128 acc41 = f41;
    acc[0U] = acc01;
    acc[1U] = acc11;
    acc[2U] = acc21;
    acc[3U] = acc31;
    acc[4U] = acc41;
}

void
Hacl_MAC_Poly1305_Simd128_fmul_r2_normalize(
  Lib_IntVector_Intrinsics_vec128* out,
  Lib_IntVector_Intrinsics_vec128* p)
{
    Lib_IntVector_Intrinsics_vec128* r = p;
    Lib_IntVector_Intrinsics_vec128* r2 = p + 10U;
    Lib_IntVector_Intrinsics_vec128 a0 = out[0U];
    Lib_IntVector_Intrinsics_vec128 a1 = out[1U];
    Lib_IntVector_Intrinsics_vec128 a2 = out[2U];
    Lib_IntVector_Intrinsics_vec128 a3 = out[3U];
    Lib_IntVector_Intrinsics_vec128 a4 = out[4U];
    Lib_IntVector_Intrinsics_vec128 r10 = r[0U];
    Lib_IntVector_Intrinsics_vec128 r11 = r[1U];
    Lib_IntVector_Intrinsics_vec128 r12 = r[2U];
    Lib_IntVector_Intrinsics_vec128 r13 = r[3U];
    Lib_IntVector_Intrinsics_vec128 r14 = r[4U];
    Lib_IntVector_Intrinsics_vec128 r20 = r2[0U];
    Lib_IntVector_Intrinsics_vec128 r21 = r2[1U];
    Lib_IntVector_Intrinsics_vec128 r22 = r2[2U];
    Lib_IntVector_Intrinsics_vec128 r23 = r2[3U];
    Lib_IntVector_Intrinsics_vec128 r24 = r2[4U];
    Lib_IntVector_Intrinsics_vec128 r201 =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(r20, r10);
    Lib_IntVector_Intrinsics_vec128 r211 =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(r21, r11);
    Lib_IntVector_Intrinsics_vec128 r221 =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(r22, r12);
    Lib_IntVector_Intrinsics_vec128 r231 =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(r23, r13);
    Lib_IntVector_Intrinsics_vec128 r241 =
      Lib_IntVector_Intrinsics_vec128_interleave_low64(r24, r14);
    Lib_IntVector_Intrinsics_vec128 r251 =
      Lib_IntVector_Intrinsics_vec128_smul64(r211, 5ULL);
    Lib_IntVector_Intrinsics_vec128 r252 =
      Lib_IntVector_Intrinsics_vec128_smul64(r221, 5ULL);
    Lib_IntVector_Intrinsics_vec128 r253 =
      Lib_IntVector_Intrinsics_vec128_smul64(r231, 5ULL);
    Lib_IntVector_Intrinsics_vec128 r254 =
      Lib_IntVector_Intrinsics_vec128_smul64(r241, 5ULL);
    Lib_IntVector_Intrinsics_vec128 a01 =
      Lib_IntVector_Intrinsics_vec128_mul64(r201, a0);
    Lib_IntVector_Intrinsics_vec128 a11 =
      Lib_IntVector_Intrinsics_vec128_mul64(r211, a0);
    Lib_IntVector_Intrinsics_vec128 a21 =
      Lib_IntVector_Intrinsics_vec128_mul64(r221, a0);
    Lib_IntVector_Intrinsics_vec128 a31 =
      Lib_IntVector_Intrinsics_vec128_mul64(r231, a0);
    Lib_IntVector_Intrinsics_vec128 a41 =
      Lib_IntVector_Intrinsics_vec128_mul64(r241, a0);
    Lib_IntVector_Intrinsics_vec128 a02 = Lib_IntVector_Intrinsics_vec128_add64(
      a01, Lib_IntVector_Intrinsics_vec128_mul64(r254, a1));
    Lib_IntVector_Intrinsics_vec128 a12 = Lib_IntVector_Intrinsics_vec128_add64(
      a11, Lib_IntVector_Intrinsics_vec128_mul64(r201, a1));
    Lib_IntVector_Intrinsics_vec128 a22 = Lib_IntVector_Intrinsics_vec128_add64(
      a21, Lib_IntVector_Intrinsics_vec128_mul64(r211, a1));
    Lib_IntVector_Intrinsics_vec128 a32 = Lib_IntVector_Intrinsics_vec128_add64(
      a31, Lib_IntVector_Intrinsics_vec128_mul64(r221, a1));
    Lib_IntVector_Intrinsics_vec128 a42 = Lib_IntVector_Intrinsics_vec128_add64(
      a41, Lib_IntVector_Intrinsics_vec128_mul64(r231, a1));
    Lib_IntVector_Intrinsics_vec128 a03 = Lib_IntVector_Intrinsics_vec128_add64(
      a02, Lib_IntVector_Intrinsics_vec128_mul64(r253, a2));
    Lib_IntVector_Intrinsics_vec128 a13 = Lib_IntVector_Intrinsics_vec128_add64(
      a12, Lib_IntVector_Intrinsics_vec128_mul64(r254, a2));
    Lib_IntVector_Intrinsics_vec128 a23 = Lib_IntVector_Intrinsics_vec128_add64(
      a22, Lib_IntVector_Intrinsics_vec128_mul64(r201, a2));
    Lib_IntVector_Intrinsics_vec128 a33 = Lib_IntVector_Intrinsics_vec128_add64(
      a32, Lib_IntVector_Intrinsics_vec128_mul64(r211, a2));
    Lib_IntVector_Intrinsics_vec128 a43 = Lib_IntVector_Intrinsics_vec128_add64(
      a42, Lib_IntVector_Intrinsics_vec128_mul64(r221, a2));
    Lib_IntVector_Intrinsics_vec128 a04 = Lib_IntVector_Intrinsics_vec128_add64(
      a03, Lib_IntVector_Intrinsics_vec128_mul64(r252, a3));
    Lib_IntVector_Intrinsics_vec128 a14 = Lib_IntVector_Intrinsics_vec128_add64(
      a13, Lib_IntVector_Intrinsics_vec128_mul64(r253, a3));
    Lib_IntVector_Intrinsics_vec128 a24 = Lib_IntVector_Intrinsics_vec128_add64(
      a23, Lib_IntVector_Intrinsics_vec128_mul64(r254, a3));
    Lib_IntVector_Intrinsics_vec128 a34 = Lib_IntVector_Intrinsics_vec128_add64(
      a33, Lib_IntVector_Intrinsics_vec128_mul64(r201, a3));
    Lib_IntVector_Intrinsics_vec128 a44 = Lib_IntVector_Intrinsics_vec128_add64(
      a43, Lib_IntVector_Intrinsics_vec128_mul64(r211, a3));
    Lib_IntVector_Intrinsics_vec128 a05 = Lib_IntVector_Intrinsics_vec128_add64(
      a04, Lib_IntVector_Intrinsics_vec128_mul64(r251, a4));
    Lib_IntVector_Intrinsics_vec128 a15 = Lib_IntVector_Intrinsics_vec128_add64(
      a14, Lib_IntVector_Intrinsics_vec128_mul64(r252, a4));
    Lib_IntVector_Intrinsics_vec128 a25 = Lib_IntVector_Intrinsics_vec128_add64(
      a24, Lib_IntVector_Intrinsics_vec128_mul64(r253, a4));
    Lib_IntVector_Intrinsics_vec128 a35 = Lib_IntVector_Intrinsics_vec128_add64(
      a34, Lib_IntVector_Intrinsics_vec128_mul64(r254, a4));
    Lib_IntVector_Intrinsics_vec128 a45 = Lib_IntVector_Intrinsics_vec128_add64(
      a44, Lib_IntVector_Intrinsics_vec128_mul64(r201, a4));
    Lib_IntVector_Intrinsics_vec128 t0 = a05;
    Lib_IntVector_Intrinsics_vec128 t1 = a15;
    Lib_IntVector_Intrinsics_vec128 t2 = a25;
    Lib_IntVector_Intrinsics_vec128 t3 = a35;
    Lib_IntVector_Intrinsics_vec128 t4 = a45;
    Lib_IntVector_Intrinsics_vec128 mask26 =
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec128 z0 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(t0, 26U);
    Lib_IntVector_Intrinsics_vec128 z1 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(t3, 26U);
    Lib_IntVector_Intrinsics_vec128 x0 =
      Lib_IntVector_Intrinsics_vec128_and(t0, mask26);
    Lib_IntVector_Intrinsics_vec128 x3 =
      Lib_IntVector_Intrinsics_vec128_and(t3, mask26);
    Lib_IntVector_Intrinsics_vec128 x1 =
      Lib_IntVector_Intrinsics_vec128_add64(t1, z0);
    Lib_IntVector_Intrinsics_vec128 x4 =
      Lib_IntVector_Intrinsics_vec128_add64(t4, z1);
    Lib_IntVector_Intrinsics_vec128 z01 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x1, 26U);
    Lib_IntVector_Intrinsics_vec128 z11 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x4, 26U);
    Lib_IntVector_Intrinsics_vec128 t =
      Lib_IntVector_Intrinsics_vec128_shift_left64(z11, 2U);
    Lib_IntVector_Intrinsics_vec128 z12 =
      Lib_IntVector_Intrinsics_vec128_add64(z11, t);
    Lib_IntVector_Intrinsics_vec128 x11 =
      Lib_IntVector_Intrinsics_vec128_and(x1, mask26);
    Lib_IntVector_Intrinsics_vec128 x41 =
      Lib_IntVector_Intrinsics_vec128_and(x4, mask26);
    Lib_IntVector_Intrinsics_vec128 x2 =
      Lib_IntVector_Intrinsics_vec128_add64(t2, z01);
    Lib_IntVector_Intrinsics_vec128 x01 =
      Lib_IntVector_Intrinsics_vec128_add64(x0, z12);
    Lib_IntVector_Intrinsics_vec128 z02 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x2, 26U);
    Lib_IntVector_Intrinsics_vec128 z13 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x01, 26U);
    Lib_IntVector_Intrinsics_vec128 x21 =
      Lib_IntVector_Intrinsics_vec128_and(x2, mask26);
    Lib_IntVector_Intrinsics_vec128 x02 =
      Lib_IntVector_Intrinsics_vec128_and(x01, mask26);
    Lib_IntVector_Intrinsics_vec128 x31 =
      Lib_IntVector_Intrinsics_vec128_add64(x3, z02);
    Lib_IntVector_Intrinsics_vec128 x12 =
      Lib_IntVector_Intrinsics_vec128_add64(x11, z13);
    Lib_IntVector_Intrinsics_vec128 z03 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x31, 26U);
    Lib_IntVector_Intrinsics_vec128 x32 =
      Lib_IntVector_Intrinsics_vec128_and(x31, mask26);
    Lib_IntVector_Intrinsics_vec128 x42 =
      Lib_IntVector_Intrinsics_vec128_add64(x41, z03);
    Lib_IntVector_Intrinsics_vec128 o0 = x02;
    Lib_IntVector_Intrinsics_vec128 o10 = x12;
    Lib_IntVector_Intrinsics_vec128 o20 = x21;
    Lib_IntVector_Intrinsics_vec128 o30 = x32;
    Lib_IntVector_Intrinsics_vec128 o40 = x42;
    Lib_IntVector_Intrinsics_vec128 o01 = Lib_IntVector_Intrinsics_vec128_add64(
      o0, Lib_IntVector_Intrinsics_vec128_interleave_high64(o0, o0));
    Lib_IntVector_Intrinsics_vec128 o11 = Lib_IntVector_Intrinsics_vec128_add64(
      o10, Lib_IntVector_Intrinsics_vec128_interleave_high64(o10, o10));
    Lib_IntVector_Intrinsics_vec128 o21 = Lib_IntVector_Intrinsics_vec128_add64(
      o20, Lib_IntVector_Intrinsics_vec128_interleave_high64(o20, o20));
    Lib_IntVector_Intrinsics_vec128 o31 = Lib_IntVector_Intrinsics_vec128_add64(
      o30, Lib_IntVector_Intrinsics_vec128_interleave_high64(o30, o30));
    Lib_IntVector_Intrinsics_vec128 o41 = Lib_IntVector_Intrinsics_vec128_add64(
      o40, Lib_IntVector_Intrinsics_vec128_interleave_high64(o40, o40));
    Lib_IntVector_Intrinsics_vec128 l = Lib_IntVector_Intrinsics_vec128_add64(
      o01, Lib_IntVector_Intrinsics_vec128_zero);
    Lib_IntVector_Intrinsics_vec128 tmp0 = Lib_IntVector_Intrinsics_vec128_and(
      l, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c0 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l, 26U);
    Lib_IntVector_Intrinsics_vec128 l0 =
      Lib_IntVector_Intrinsics_vec128_add64(o11, c0);
    Lib_IntVector_Intrinsics_vec128 tmp1 = Lib_IntVector_Intrinsics_vec128_and(
      l0, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c1 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l0, 26U);
    Lib_IntVector_Intrinsics_vec128 l1 =
      Lib_IntVector_Intrinsics_vec128_add64(o21, c1);
    Lib_IntVector_Intrinsics_vec128 tmp2 = Lib_IntVector_Intrinsics_vec128_and(
      l1, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c2 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l1, 26U);
    Lib_IntVector_Intrinsics_vec128 l2 =
      Lib_IntVector_Intrinsics_vec128_add64(o31, c2);
    Lib_IntVector_Intrinsics_vec128 tmp3 = Lib_IntVector_Intrinsics_vec128_and(
      l2, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c3 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l2, 26U);
    Lib_IntVector_Intrinsics_vec128 l3 =
      Lib_IntVector_Intrinsics_vec128_add64(o41, c3);
    Lib_IntVector_Intrinsics_vec128 tmp4 = Lib_IntVector_Intrinsics_vec128_and(
      l3, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c4 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l3, 26U);
    Lib_IntVector_Intrinsics_vec128 o00 = Lib_IntVector_Intrinsics_vec128_add64(
      tmp0, Lib_IntVector_Intrinsics_vec128_smul64(c4, 5ULL));
    Lib_IntVector_Intrinsics_vec128 o1 = tmp1;
    Lib_IntVector_Intrinsics_vec128 o2 = tmp2;
    Lib_IntVector_Intrinsics_vec128 o3 = tmp3;
    Lib_IntVector_Intrinsics_vec128 o4 = tmp4;
    out[0U] = o00;
    out[1U] = o1;
    out[2U] = o2;
    out[3U] = o3;
    out[4U] = o4;
}

void
Hacl_MAC_Poly1305_Simd128_poly1305_init(Lib_IntVector_Intrinsics_vec128* ctx,
                                        uint8_t* key)
{
    Lib_IntVector_Intrinsics_vec128* acc = ctx;
    Lib_IntVector_Intrinsics_vec128* pre = ctx + 5U;
    uint8_t* kr = key;
    acc[0U] = Lib_IntVector_Intrinsics_vec128_zero;
    acc[1U] = Lib_IntVector_Intrinsics_vec128_zero;
    acc[2U] = Lib_IntVector_Intrinsics_vec128_zero;
    acc[3U] = Lib_IntVector_Intrinsics_vec128_zero;
    acc[4U] = Lib_IntVector_Intrinsics_vec128_zero;
    uint64_t u0 = READ64LE(kr);
    uint64_t lo = u0;
    uint64_t u = READ64LE(kr + 8U);
    uint64_t hi = u;
    uint64_t mask0 = 0x0ffffffc0fffffffULL;
    uint64_t mask1 = 0x0ffffffc0ffffffcULL;
    uint64_t lo1 = lo & mask0;
    uint64_t hi1 = hi & mask1;
    Lib_IntVector_Intrinsics_vec128* r = pre;
    Lib_IntVector_Intrinsics_vec128* r5 = pre + 5U;
    Lib_IntVector_Intrinsics_vec128* rn = pre + 10U;
    Lib_IntVector_Intrinsics_vec128* rn_5 = pre + 15U;
    Lib_IntVector_Intrinsics_vec128 r_vec0 =
      Lib_IntVector_Intrinsics_vec128_load64(lo1);
    Lib_IntVector_Intrinsics_vec128 r_vec1 =
      Lib_IntVector_Intrinsics_vec128_load64(hi1);
    Lib_IntVector_Intrinsics_vec128 f00 = Lib_IntVector_Intrinsics_vec128_and(
      r_vec0, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f15 = Lib_IntVector_Intrinsics_vec128_and(
      Lib_IntVector_Intrinsics_vec128_shift_right64(r_vec0, 26U),
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f20 = Lib_IntVector_Intrinsics_vec128_or(
      Lib_IntVector_Intrinsics_vec128_shift_right64(r_vec0, 52U),
      Lib_IntVector_Intrinsics_vec128_shift_left64(
        Lib_IntVector_Intrinsics_vec128_and(
          r_vec1, Lib_IntVector_Intrinsics_vec128_load64(0x3fffULL)),
        12U));
    Lib_IntVector_Intrinsics_vec128 f30 = Lib_IntVector_Intrinsics_vec128_and(
      Lib_IntVector_Intrinsics_vec128_shift_right64(r_vec1, 14U),
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 f40 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(r_vec1, 40U);
    Lib_IntVector_Intrinsics_vec128 f0 = f00;
    Lib_IntVector_Intrinsics_vec128 f1 = f15;
    Lib_IntVector_Intrinsics_vec128 f2 = f20;
    Lib_IntVector_Intrinsics_vec128 f3 = f30;
    Lib_IntVector_Intrinsics_vec128 f4 = f40;
    r[0U] = f0;
    r[1U] = f1;
    r[2U] = f2;
    r[3U] = f3;
    r[4U] = f4;
    Lib_IntVector_Intrinsics_vec128 f200 = r[0U];
    Lib_IntVector_Intrinsics_vec128 f210 = r[1U];
    Lib_IntVector_Intrinsics_vec128 f220 = r[2U];
    Lib_IntVector_Intrinsics_vec128 f230 = r[3U];
    Lib_IntVector_Intrinsics_vec128 f240 = r[4U];
    r5[0U] = Lib_IntVector_Intrinsics_vec128_smul64(f200, 5ULL);
    r5[1U] = Lib_IntVector_Intrinsics_vec128_smul64(f210, 5ULL);
    r5[2U] = Lib_IntVector_Intrinsics_vec128_smul64(f220, 5ULL);
    r5[3U] = Lib_IntVector_Intrinsics_vec128_smul64(f230, 5ULL);
    r5[4U] = Lib_IntVector_Intrinsics_vec128_smul64(f240, 5ULL);
    Lib_IntVector_Intrinsics_vec128 r0 = r[0U];
    Lib_IntVector_Intrinsics_vec128 r1 = r[1U];
    Lib_IntVector_Intrinsics_vec128 r2 = r[2U];
    Lib_IntVector_Intrinsics_vec128 r3 = r[3U];
    Lib_IntVector_Intrinsics_vec128 r4 = r[4U];
    Lib_IntVector_Intrinsics_vec128 r51 = r5[1U];
    Lib_IntVector_Intrinsics_vec128 r52 = r5[2U];
    Lib_IntVector_Intrinsics_vec128 r53 = r5[3U];
    Lib_IntVector_Intrinsics_vec128 r54 = r5[4U];
    Lib_IntVector_Intrinsics_vec128 f10 = r[0U];
    Lib_IntVector_Intrinsics_vec128 f11 = r[1U];
    Lib_IntVector_Intrinsics_vec128 f12 = r[2U];
    Lib_IntVector_Intrinsics_vec128 f13 = r[3U];
    Lib_IntVector_Intrinsics_vec128 f14 = r[4U];
    Lib_IntVector_Intrinsics_vec128 a0 =
      Lib_IntVector_Intrinsics_vec128_mul64(r0, f10);
    Lib_IntVector_Intrinsics_vec128 a1 =
      Lib_IntVector_Intrinsics_vec128_mul64(r1, f10);
    Lib_IntVector_Intrinsics_vec128 a2 =
      Lib_IntVector_Intrinsics_vec128_mul64(r2, f10);
    Lib_IntVector_Intrinsics_vec128 a3 =
      Lib_IntVector_Intrinsics_vec128_mul64(r3, f10);
    Lib_IntVector_Intrinsics_vec128 a4 =
      Lib_IntVector_Intrinsics_vec128_mul64(r4, f10);
    Lib_IntVector_Intrinsics_vec128 a01 = Lib_IntVector_Intrinsics_vec128_add64(
      a0, Lib_IntVector_Intrinsics_vec128_mul64(r54, f11));
    Lib_IntVector_Intrinsics_vec128 a11 = Lib_IntVector_Intrinsics_vec128_add64(
      a1, Lib_IntVector_Intrinsics_vec128_mul64(r0, f11));
    Lib_IntVector_Intrinsics_vec128 a21 = Lib_IntVector_Intrinsics_vec128_add64(
      a2, Lib_IntVector_Intrinsics_vec128_mul64(r1, f11));
    Lib_IntVector_Intrinsics_vec128 a31 = Lib_IntVector_Intrinsics_vec128_add64(
      a3, Lib_IntVector_Intrinsics_vec128_mul64(r2, f11));
    Lib_IntVector_Intrinsics_vec128 a41 = Lib_IntVector_Intrinsics_vec128_add64(
      a4, Lib_IntVector_Intrinsics_vec128_mul64(r3, f11));
    Lib_IntVector_Intrinsics_vec128 a02 = Lib_IntVector_Intrinsics_vec128_add64(
      a01, Lib_IntVector_Intrinsics_vec128_mul64(r53, f12));
    Lib_IntVector_Intrinsics_vec128 a12 = Lib_IntVector_Intrinsics_vec128_add64(
      a11, Lib_IntVector_Intrinsics_vec128_mul64(r54, f12));
    Lib_IntVector_Intrinsics_vec128 a22 = Lib_IntVector_Intrinsics_vec128_add64(
      a21, Lib_IntVector_Intrinsics_vec128_mul64(r0, f12));
    Lib_IntVector_Intrinsics_vec128 a32 = Lib_IntVector_Intrinsics_vec128_add64(
      a31, Lib_IntVector_Intrinsics_vec128_mul64(r1, f12));
    Lib_IntVector_Intrinsics_vec128 a42 = Lib_IntVector_Intrinsics_vec128_add64(
      a41, Lib_IntVector_Intrinsics_vec128_mul64(r2, f12));
    Lib_IntVector_Intrinsics_vec128 a03 = Lib_IntVector_Intrinsics_vec128_add64(
      a02, Lib_IntVector_Intrinsics_vec128_mul64(r52, f13));
    Lib_IntVector_Intrinsics_vec128 a13 = Lib_IntVector_Intrinsics_vec128_add64(
      a12, Lib_IntVector_Intrinsics_vec128_mul64(r53, f13));
    Lib_IntVector_Intrinsics_vec128 a23 = Lib_IntVector_Intrinsics_vec128_add64(
      a22, Lib_IntVector_Intrinsics_vec128_mul64(r54, f13));
    Lib_IntVector_Intrinsics_vec128 a33 = Lib_IntVector_Intrinsics_vec128_add64(
      a32, Lib_IntVector_Intrinsics_vec128_mul64(r0, f13));
    Lib_IntVector_Intrinsics_vec128 a43 = Lib_IntVector_Intrinsics_vec128_add64(
      a42, Lib_IntVector_Intrinsics_vec128_mul64(r1, f13));
    Lib_IntVector_Intrinsics_vec128 a04 = Lib_IntVector_Intrinsics_vec128_add64(
      a03, Lib_IntVector_Intrinsics_vec128_mul64(r51, f14));
    Lib_IntVector_Intrinsics_vec128 a14 = Lib_IntVector_Intrinsics_vec128_add64(
      a13, Lib_IntVector_Intrinsics_vec128_mul64(r52, f14));
    Lib_IntVector_Intrinsics_vec128 a24 = Lib_IntVector_Intrinsics_vec128_add64(
      a23, Lib_IntVector_Intrinsics_vec128_mul64(r53, f14));
    Lib_IntVector_Intrinsics_vec128 a34 = Lib_IntVector_Intrinsics_vec128_add64(
      a33, Lib_IntVector_Intrinsics_vec128_mul64(r54, f14));
    Lib_IntVector_Intrinsics_vec128 a44 = Lib_IntVector_Intrinsics_vec128_add64(
      a43, Lib_IntVector_Intrinsics_vec128_mul64(r0, f14));
    Lib_IntVector_Intrinsics_vec128 t0 = a04;
    Lib_IntVector_Intrinsics_vec128 t1 = a14;
    Lib_IntVector_Intrinsics_vec128 t2 = a24;
    Lib_IntVector_Intrinsics_vec128 t3 = a34;
    Lib_IntVector_Intrinsics_vec128 t4 = a44;
    Lib_IntVector_Intrinsics_vec128 mask26 =
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec128 z0 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(t0, 26U);
    Lib_IntVector_Intrinsics_vec128 z1 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(t3, 26U);
    Lib_IntVector_Intrinsics_vec128 x0 =
      Lib_IntVector_Intrinsics_vec128_and(t0, mask26);
    Lib_IntVector_Intrinsics_vec128 x3 =
      Lib_IntVector_Intrinsics_vec128_and(t3, mask26);
    Lib_IntVector_Intrinsics_vec128 x1 =
      Lib_IntVector_Intrinsics_vec128_add64(t1, z0);
    Lib_IntVector_Intrinsics_vec128 x4 =
      Lib_IntVector_Intrinsics_vec128_add64(t4, z1);
    Lib_IntVector_Intrinsics_vec128 z01 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x1, 26U);
    Lib_IntVector_Intrinsics_vec128 z11 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x4, 26U);
    Lib_IntVector_Intrinsics_vec128 t =
      Lib_IntVector_Intrinsics_vec128_shift_left64(z11, 2U);
    Lib_IntVector_Intrinsics_vec128 z12 =
      Lib_IntVector_Intrinsics_vec128_add64(z11, t);
    Lib_IntVector_Intrinsics_vec128 x11 =
      Lib_IntVector_Intrinsics_vec128_and(x1, mask26);
    Lib_IntVector_Intrinsics_vec128 x41 =
      Lib_IntVector_Intrinsics_vec128_and(x4, mask26);
    Lib_IntVector_Intrinsics_vec128 x2 =
      Lib_IntVector_Intrinsics_vec128_add64(t2, z01);
    Lib_IntVector_Intrinsics_vec128 x01 =
      Lib_IntVector_Intrinsics_vec128_add64(x0, z12);
    Lib_IntVector_Intrinsics_vec128 z02 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x2, 26U);
    Lib_IntVector_Intrinsics_vec128 z13 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x01, 26U);
    Lib_IntVector_Intrinsics_vec128 x21 =
      Lib_IntVector_Intrinsics_vec128_and(x2, mask26);
    Lib_IntVector_Intrinsics_vec128 x02 =
      Lib_IntVector_Intrinsics_vec128_and(x01, mask26);
    Lib_IntVector_Intrinsics_vec128 x31 =
      Lib_IntVector_Intrinsics_vec128_add64(x3, z02);
    Lib_IntVector_Intrinsics_vec128 x12 =
      Lib_IntVector_Intrinsics_vec128_add64(x11, z13);
    Lib_IntVector_Intrinsics_vec128 z03 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(x31, 26U);
    Lib_IntVector_Intrinsics_vec128 x32 =
      Lib_IntVector_Intrinsics_vec128_and(x31, mask26);
    Lib_IntVector_Intrinsics_vec128 x42 =
      Lib_IntVector_Intrinsics_vec128_add64(x41, z03);
    Lib_IntVector_Intrinsics_vec128 o0 = x02;
    Lib_IntVector_Intrinsics_vec128 o1 = x12;
    Lib_IntVector_Intrinsics_vec128 o2 = x21;
    Lib_IntVector_Intrinsics_vec128 o3 = x32;
    Lib_IntVector_Intrinsics_vec128 o4 = x42;
    rn[0U] = o0;
    rn[1U] = o1;
    rn[2U] = o2;
    rn[3U] = o3;
    rn[4U] = o4;
    Lib_IntVector_Intrinsics_vec128 f201 = rn[0U];
    Lib_IntVector_Intrinsics_vec128 f21 = rn[1U];
    Lib_IntVector_Intrinsics_vec128 f22 = rn[2U];
    Lib_IntVector_Intrinsics_vec128 f23 = rn[3U];
    Lib_IntVector_Intrinsics_vec128 f24 = rn[4U];
    rn_5[0U] = Lib_IntVector_Intrinsics_vec128_smul64(f201, 5ULL);
    rn_5[1U] = Lib_IntVector_Intrinsics_vec128_smul64(f21, 5ULL);
    rn_5[2U] = Lib_IntVector_Intrinsics_vec128_smul64(f22, 5ULL);
    rn_5[3U] = Lib_IntVector_Intrinsics_vec128_smul64(f23, 5ULL);
    rn_5[4U] = Lib_IntVector_Intrinsics_vec128_smul64(f24, 5ULL);
}

static void
poly1305_update(Lib_IntVector_Intrinsics_vec128* ctx,
                uint32_t len,
                uint8_t* text)
{
    Lib_IntVector_Intrinsics_vec128* pre = ctx + 5U;
    Lib_IntVector_Intrinsics_vec128* acc = ctx;
    uint32_t sz_block = 32U;
    uint32_t len0 = len / sz_block * sz_block;
    uint8_t* t0 = text;
    if (len0 > 0U) {
        uint32_t bs = 32U;
        uint8_t* text0 = t0;
        Hacl_MAC_Poly1305_Simd128_load_acc2(acc, text0);
        uint32_t len1 = len0 - bs;
        uint8_t* text1 = t0 + bs;
        uint32_t nb = len1 / bs;
        for (uint32_t i = 0U; i < nb; i++) {
            uint8_t* block = text1 + i * bs;
            KRML_PRE_ALIGN(16)
            Lib_IntVector_Intrinsics_vec128 e[5U] KRML_POST_ALIGN(16) = { 0U };
            Lib_IntVector_Intrinsics_vec128 b1 =
              Lib_IntVector_Intrinsics_vec128_load64_le(block);
            Lib_IntVector_Intrinsics_vec128 b2 =
              Lib_IntVector_Intrinsics_vec128_load64_le(block + 16U);
            Lib_IntVector_Intrinsics_vec128 lo =
              Lib_IntVector_Intrinsics_vec128_interleave_low64(b1, b2);
            Lib_IntVector_Intrinsics_vec128 hi =
              Lib_IntVector_Intrinsics_vec128_interleave_high64(b1, b2);
            Lib_IntVector_Intrinsics_vec128 f00 =
              Lib_IntVector_Intrinsics_vec128_and(
                lo, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
            Lib_IntVector_Intrinsics_vec128 f15 =
              Lib_IntVector_Intrinsics_vec128_and(
                Lib_IntVector_Intrinsics_vec128_shift_right64(lo, 26U),
                Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
            Lib_IntVector_Intrinsics_vec128 f25 =
              Lib_IntVector_Intrinsics_vec128_or(
                Lib_IntVector_Intrinsics_vec128_shift_right64(lo, 52U),
                Lib_IntVector_Intrinsics_vec128_shift_left64(
                  Lib_IntVector_Intrinsics_vec128_and(
                    hi, Lib_IntVector_Intrinsics_vec128_load64(0x3fffULL)),
                  12U));
            Lib_IntVector_Intrinsics_vec128 f30 =
              Lib_IntVector_Intrinsics_vec128_and(
                Lib_IntVector_Intrinsics_vec128_shift_right64(hi, 14U),
                Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
            Lib_IntVector_Intrinsics_vec128 f40 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(hi, 40U);
            Lib_IntVector_Intrinsics_vec128 f0 = f00;
            Lib_IntVector_Intrinsics_vec128 f1 = f15;
            Lib_IntVector_Intrinsics_vec128 f2 = f25;
            Lib_IntVector_Intrinsics_vec128 f3 = f30;
            Lib_IntVector_Intrinsics_vec128 f41 = f40;
            e[0U] = f0;
            e[1U] = f1;
            e[2U] = f2;
            e[3U] = f3;
            e[4U] = f41;
            uint64_t b = 0x1000000ULL;
            Lib_IntVector_Intrinsics_vec128 mask =
              Lib_IntVector_Intrinsics_vec128_load64(b);
            Lib_IntVector_Intrinsics_vec128 f4 = e[4U];
            e[4U] = Lib_IntVector_Intrinsics_vec128_or(f4, mask);
            Lib_IntVector_Intrinsics_vec128* rn = pre + 10U;
            Lib_IntVector_Intrinsics_vec128* rn5 = pre + 15U;
            Lib_IntVector_Intrinsics_vec128 r0 = rn[0U];
            Lib_IntVector_Intrinsics_vec128 r1 = rn[1U];
            Lib_IntVector_Intrinsics_vec128 r2 = rn[2U];
            Lib_IntVector_Intrinsics_vec128 r3 = rn[3U];
            Lib_IntVector_Intrinsics_vec128 r4 = rn[4U];
            Lib_IntVector_Intrinsics_vec128 r51 = rn5[1U];
            Lib_IntVector_Intrinsics_vec128 r52 = rn5[2U];
            Lib_IntVector_Intrinsics_vec128 r53 = rn5[3U];
            Lib_IntVector_Intrinsics_vec128 r54 = rn5[4U];
            Lib_IntVector_Intrinsics_vec128 f10 = acc[0U];
            Lib_IntVector_Intrinsics_vec128 f110 = acc[1U];
            Lib_IntVector_Intrinsics_vec128 f120 = acc[2U];
            Lib_IntVector_Intrinsics_vec128 f130 = acc[3U];
            Lib_IntVector_Intrinsics_vec128 f140 = acc[4U];
            Lib_IntVector_Intrinsics_vec128 a0 =
              Lib_IntVector_Intrinsics_vec128_mul64(r0, f10);
            Lib_IntVector_Intrinsics_vec128 a1 =
              Lib_IntVector_Intrinsics_vec128_mul64(r1, f10);
            Lib_IntVector_Intrinsics_vec128 a2 =
              Lib_IntVector_Intrinsics_vec128_mul64(r2, f10);
            Lib_IntVector_Intrinsics_vec128 a3 =
              Lib_IntVector_Intrinsics_vec128_mul64(r3, f10);
            Lib_IntVector_Intrinsics_vec128 a4 =
              Lib_IntVector_Intrinsics_vec128_mul64(r4, f10);
            Lib_IntVector_Intrinsics_vec128 a01 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a0, Lib_IntVector_Intrinsics_vec128_mul64(r54, f110));
            Lib_IntVector_Intrinsics_vec128 a11 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a1, Lib_IntVector_Intrinsics_vec128_mul64(r0, f110));
            Lib_IntVector_Intrinsics_vec128 a21 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a2, Lib_IntVector_Intrinsics_vec128_mul64(r1, f110));
            Lib_IntVector_Intrinsics_vec128 a31 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a3, Lib_IntVector_Intrinsics_vec128_mul64(r2, f110));
            Lib_IntVector_Intrinsics_vec128 a41 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a4, Lib_IntVector_Intrinsics_vec128_mul64(r3, f110));
            Lib_IntVector_Intrinsics_vec128 a02 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a01, Lib_IntVector_Intrinsics_vec128_mul64(r53, f120));
            Lib_IntVector_Intrinsics_vec128 a12 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a11, Lib_IntVector_Intrinsics_vec128_mul64(r54, f120));
            Lib_IntVector_Intrinsics_vec128 a22 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a21, Lib_IntVector_Intrinsics_vec128_mul64(r0, f120));
            Lib_IntVector_Intrinsics_vec128 a32 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a31, Lib_IntVector_Intrinsics_vec128_mul64(r1, f120));
            Lib_IntVector_Intrinsics_vec128 a42 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a41, Lib_IntVector_Intrinsics_vec128_mul64(r2, f120));
            Lib_IntVector_Intrinsics_vec128 a03 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a02, Lib_IntVector_Intrinsics_vec128_mul64(r52, f130));
            Lib_IntVector_Intrinsics_vec128 a13 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a12, Lib_IntVector_Intrinsics_vec128_mul64(r53, f130));
            Lib_IntVector_Intrinsics_vec128 a23 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a22, Lib_IntVector_Intrinsics_vec128_mul64(r54, f130));
            Lib_IntVector_Intrinsics_vec128 a33 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a32, Lib_IntVector_Intrinsics_vec128_mul64(r0, f130));
            Lib_IntVector_Intrinsics_vec128 a43 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a42, Lib_IntVector_Intrinsics_vec128_mul64(r1, f130));
            Lib_IntVector_Intrinsics_vec128 a04 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a03, Lib_IntVector_Intrinsics_vec128_mul64(r51, f140));
            Lib_IntVector_Intrinsics_vec128 a14 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a13, Lib_IntVector_Intrinsics_vec128_mul64(r52, f140));
            Lib_IntVector_Intrinsics_vec128 a24 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a23, Lib_IntVector_Intrinsics_vec128_mul64(r53, f140));
            Lib_IntVector_Intrinsics_vec128 a34 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a33, Lib_IntVector_Intrinsics_vec128_mul64(r54, f140));
            Lib_IntVector_Intrinsics_vec128 a44 =
              Lib_IntVector_Intrinsics_vec128_add64(
                a43, Lib_IntVector_Intrinsics_vec128_mul64(r0, f140));
            Lib_IntVector_Intrinsics_vec128 t01 = a04;
            Lib_IntVector_Intrinsics_vec128 t1 = a14;
            Lib_IntVector_Intrinsics_vec128 t2 = a24;
            Lib_IntVector_Intrinsics_vec128 t3 = a34;
            Lib_IntVector_Intrinsics_vec128 t4 = a44;
            Lib_IntVector_Intrinsics_vec128 mask26 =
              Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
            Lib_IntVector_Intrinsics_vec128 z0 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(t01, 26U);
            Lib_IntVector_Intrinsics_vec128 z1 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(t3, 26U);
            Lib_IntVector_Intrinsics_vec128 x0 =
              Lib_IntVector_Intrinsics_vec128_and(t01, mask26);
            Lib_IntVector_Intrinsics_vec128 x3 =
              Lib_IntVector_Intrinsics_vec128_and(t3, mask26);
            Lib_IntVector_Intrinsics_vec128 x1 =
              Lib_IntVector_Intrinsics_vec128_add64(t1, z0);
            Lib_IntVector_Intrinsics_vec128 x4 =
              Lib_IntVector_Intrinsics_vec128_add64(t4, z1);
            Lib_IntVector_Intrinsics_vec128 z01 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(x1, 26U);
            Lib_IntVector_Intrinsics_vec128 z11 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(x4, 26U);
            Lib_IntVector_Intrinsics_vec128 t =
              Lib_IntVector_Intrinsics_vec128_shift_left64(z11, 2U);
            Lib_IntVector_Intrinsics_vec128 z12 =
              Lib_IntVector_Intrinsics_vec128_add64(z11, t);
            Lib_IntVector_Intrinsics_vec128 x11 =
              Lib_IntVector_Intrinsics_vec128_and(x1, mask26);
            Lib_IntVector_Intrinsics_vec128 x41 =
              Lib_IntVector_Intrinsics_vec128_and(x4, mask26);
            Lib_IntVector_Intrinsics_vec128 x2 =
              Lib_IntVector_Intrinsics_vec128_add64(t2, z01);
            Lib_IntVector_Intrinsics_vec128 x01 =
              Lib_IntVector_Intrinsics_vec128_add64(x0, z12);
            Lib_IntVector_Intrinsics_vec128 z02 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(x2, 26U);
            Lib_IntVector_Intrinsics_vec128 z13 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(x01, 26U);
            Lib_IntVector_Intrinsics_vec128 x21 =
              Lib_IntVector_Intrinsics_vec128_and(x2, mask26);
            Lib_IntVector_Intrinsics_vec128 x02 =
              Lib_IntVector_Intrinsics_vec128_and(x01, mask26);
            Lib_IntVector_Intrinsics_vec128 x31 =
              Lib_IntVector_Intrinsics_vec128_add64(x3, z02);
            Lib_IntVector_Intrinsics_vec128 x12 =
              Lib_IntVector_Intrinsics_vec128_add64(x11, z13);
            Lib_IntVector_Intrinsics_vec128 z03 =
              Lib_IntVector_Intrinsics_vec128_shift_right64(x31, 26U);
            Lib_IntVector_Intrinsics_vec128 x32 =
              Lib_IntVector_Intrinsics_vec128_and(x31, mask26);
            Lib_IntVector_Intrinsics_vec128 x42 =
              Lib_IntVector_Intrinsics_vec128_add64(x41, z03);
            Lib_IntVector_Intrinsics_vec128 o00 = x02;
            Lib_IntVector_Intrinsics_vec128 o10 = x12;
            Lib_IntVector_Intrinsics_vec128 o20 = x21;
            Lib_IntVector_Intrinsics_vec128 o30 = x32;
            Lib_IntVector_Intrinsics_vec128 o40 = x42;
            acc[0U] = o00;
            acc[1U] = o10;
            acc[2U] = o20;
            acc[3U] = o30;
            acc[4U] = o40;
            Lib_IntVector_Intrinsics_vec128 f100 = acc[0U];
            Lib_IntVector_Intrinsics_vec128 f11 = acc[1U];
            Lib_IntVector_Intrinsics_vec128 f12 = acc[2U];
            Lib_IntVector_Intrinsics_vec128 f13 = acc[3U];
            Lib_IntVector_Intrinsics_vec128 f14 = acc[4U];
            Lib_IntVector_Intrinsics_vec128 f20 = e[0U];
            Lib_IntVector_Intrinsics_vec128 f21 = e[1U];
            Lib_IntVector_Intrinsics_vec128 f22 = e[2U];
            Lib_IntVector_Intrinsics_vec128 f23 = e[3U];
            Lib_IntVector_Intrinsics_vec128 f24 = e[4U];
            Lib_IntVector_Intrinsics_vec128 o0 =
              Lib_IntVector_Intrinsics_vec128_add64(f100, f20);
            Lib_IntVector_Intrinsics_vec128 o1 =
              Lib_IntVector_Intrinsics_vec128_add64(f11, f21);
            Lib_IntVector_Intrinsics_vec128 o2 =
              Lib_IntVector_Intrinsics_vec128_add64(f12, f22);
            Lib_IntVector_Intrinsics_vec128 o3 =
              Lib_IntVector_Intrinsics_vec128_add64(f13, f23);
            Lib_IntVector_Intrinsics_vec128 o4 =
              Lib_IntVector_Intrinsics_vec128_add64(f14, f24);
            acc[0U] = o0;
            acc[1U] = o1;
            acc[2U] = o2;
            acc[3U] = o3;
            acc[4U] = o4;
        }
        Hacl_MAC_Poly1305_Simd128_fmul_r2_normalize(acc, pre);
    }
    uint32_t len1 = len - len0;
    uint8_t* t1 = text + len0;
    uint32_t nb = len1 / 16U;
    uint32_t rem = len1 % 16U;
    for (uint32_t i = 0U; i < nb; i++) {
        uint8_t* block = t1 + i * 16U;
        KRML_PRE_ALIGN(16)
        Lib_IntVector_Intrinsics_vec128 e[5U] KRML_POST_ALIGN(16) = { 0U };
        uint64_t u0 = READ64LE(block);
        uint64_t lo = u0;
        uint64_t u = READ64LE(block + 8U);
        uint64_t hi = u;
        Lib_IntVector_Intrinsics_vec128 f0 =
          Lib_IntVector_Intrinsics_vec128_load64(lo);
        Lib_IntVector_Intrinsics_vec128 f1 =
          Lib_IntVector_Intrinsics_vec128_load64(hi);
        Lib_IntVector_Intrinsics_vec128 f010 =
          Lib_IntVector_Intrinsics_vec128_and(
            f0, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f110 =
          Lib_IntVector_Intrinsics_vec128_and(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f0, 26U),
            Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f20 =
          Lib_IntVector_Intrinsics_vec128_or(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f0, 52U),
            Lib_IntVector_Intrinsics_vec128_shift_left64(
              Lib_IntVector_Intrinsics_vec128_and(
                f1, Lib_IntVector_Intrinsics_vec128_load64(0x3fffULL)),
              12U));
        Lib_IntVector_Intrinsics_vec128 f30 =
          Lib_IntVector_Intrinsics_vec128_and(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f1, 14U),
            Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f40 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(f1, 40U);
        Lib_IntVector_Intrinsics_vec128 f01 = f010;
        Lib_IntVector_Intrinsics_vec128 f111 = f110;
        Lib_IntVector_Intrinsics_vec128 f2 = f20;
        Lib_IntVector_Intrinsics_vec128 f3 = f30;
        Lib_IntVector_Intrinsics_vec128 f41 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f41;
        uint64_t b = 0x1000000ULL;
        Lib_IntVector_Intrinsics_vec128 mask =
          Lib_IntVector_Intrinsics_vec128_load64(b);
        Lib_IntVector_Intrinsics_vec128 f4 = e[4U];
        e[4U] = Lib_IntVector_Intrinsics_vec128_or(f4, mask);
        Lib_IntVector_Intrinsics_vec128* r = pre;
        Lib_IntVector_Intrinsics_vec128* r5 = pre + 5U;
        Lib_IntVector_Intrinsics_vec128 r0 = r[0U];
        Lib_IntVector_Intrinsics_vec128 r1 = r[1U];
        Lib_IntVector_Intrinsics_vec128 r2 = r[2U];
        Lib_IntVector_Intrinsics_vec128 r3 = r[3U];
        Lib_IntVector_Intrinsics_vec128 r4 = r[4U];
        Lib_IntVector_Intrinsics_vec128 r51 = r5[1U];
        Lib_IntVector_Intrinsics_vec128 r52 = r5[2U];
        Lib_IntVector_Intrinsics_vec128 r53 = r5[3U];
        Lib_IntVector_Intrinsics_vec128 r54 = r5[4U];
        Lib_IntVector_Intrinsics_vec128 f10 = e[0U];
        Lib_IntVector_Intrinsics_vec128 f11 = e[1U];
        Lib_IntVector_Intrinsics_vec128 f12 = e[2U];
        Lib_IntVector_Intrinsics_vec128 f13 = e[3U];
        Lib_IntVector_Intrinsics_vec128 f14 = e[4U];
        Lib_IntVector_Intrinsics_vec128 a0 = acc[0U];
        Lib_IntVector_Intrinsics_vec128 a1 = acc[1U];
        Lib_IntVector_Intrinsics_vec128 a2 = acc[2U];
        Lib_IntVector_Intrinsics_vec128 a3 = acc[3U];
        Lib_IntVector_Intrinsics_vec128 a4 = acc[4U];
        Lib_IntVector_Intrinsics_vec128 a01 =
          Lib_IntVector_Intrinsics_vec128_add64(a0, f10);
        Lib_IntVector_Intrinsics_vec128 a11 =
          Lib_IntVector_Intrinsics_vec128_add64(a1, f11);
        Lib_IntVector_Intrinsics_vec128 a21 =
          Lib_IntVector_Intrinsics_vec128_add64(a2, f12);
        Lib_IntVector_Intrinsics_vec128 a31 =
          Lib_IntVector_Intrinsics_vec128_add64(a3, f13);
        Lib_IntVector_Intrinsics_vec128 a41 =
          Lib_IntVector_Intrinsics_vec128_add64(a4, f14);
        Lib_IntVector_Intrinsics_vec128 a02 =
          Lib_IntVector_Intrinsics_vec128_mul64(r0, a01);
        Lib_IntVector_Intrinsics_vec128 a12 =
          Lib_IntVector_Intrinsics_vec128_mul64(r1, a01);
        Lib_IntVector_Intrinsics_vec128 a22 =
          Lib_IntVector_Intrinsics_vec128_mul64(r2, a01);
        Lib_IntVector_Intrinsics_vec128 a32 =
          Lib_IntVector_Intrinsics_vec128_mul64(r3, a01);
        Lib_IntVector_Intrinsics_vec128 a42 =
          Lib_IntVector_Intrinsics_vec128_mul64(r4, a01);
        Lib_IntVector_Intrinsics_vec128 a03 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a02, Lib_IntVector_Intrinsics_vec128_mul64(r54, a11));
        Lib_IntVector_Intrinsics_vec128 a13 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a12, Lib_IntVector_Intrinsics_vec128_mul64(r0, a11));
        Lib_IntVector_Intrinsics_vec128 a23 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a22, Lib_IntVector_Intrinsics_vec128_mul64(r1, a11));
        Lib_IntVector_Intrinsics_vec128 a33 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a32, Lib_IntVector_Intrinsics_vec128_mul64(r2, a11));
        Lib_IntVector_Intrinsics_vec128 a43 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a42, Lib_IntVector_Intrinsics_vec128_mul64(r3, a11));
        Lib_IntVector_Intrinsics_vec128 a04 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a03, Lib_IntVector_Intrinsics_vec128_mul64(r53, a21));
        Lib_IntVector_Intrinsics_vec128 a14 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a13, Lib_IntVector_Intrinsics_vec128_mul64(r54, a21));
        Lib_IntVector_Intrinsics_vec128 a24 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a23, Lib_IntVector_Intrinsics_vec128_mul64(r0, a21));
        Lib_IntVector_Intrinsics_vec128 a34 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a33, Lib_IntVector_Intrinsics_vec128_mul64(r1, a21));
        Lib_IntVector_Intrinsics_vec128 a44 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a43, Lib_IntVector_Intrinsics_vec128_mul64(r2, a21));
        Lib_IntVector_Intrinsics_vec128 a05 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a04, Lib_IntVector_Intrinsics_vec128_mul64(r52, a31));
        Lib_IntVector_Intrinsics_vec128 a15 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a14, Lib_IntVector_Intrinsics_vec128_mul64(r53, a31));
        Lib_IntVector_Intrinsics_vec128 a25 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a24, Lib_IntVector_Intrinsics_vec128_mul64(r54, a31));
        Lib_IntVector_Intrinsics_vec128 a35 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a34, Lib_IntVector_Intrinsics_vec128_mul64(r0, a31));
        Lib_IntVector_Intrinsics_vec128 a45 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a44, Lib_IntVector_Intrinsics_vec128_mul64(r1, a31));
        Lib_IntVector_Intrinsics_vec128 a06 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a05, Lib_IntVector_Intrinsics_vec128_mul64(r51, a41));
        Lib_IntVector_Intrinsics_vec128 a16 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a15, Lib_IntVector_Intrinsics_vec128_mul64(r52, a41));
        Lib_IntVector_Intrinsics_vec128 a26 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a25, Lib_IntVector_Intrinsics_vec128_mul64(r53, a41));
        Lib_IntVector_Intrinsics_vec128 a36 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a35, Lib_IntVector_Intrinsics_vec128_mul64(r54, a41));
        Lib_IntVector_Intrinsics_vec128 a46 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a45, Lib_IntVector_Intrinsics_vec128_mul64(r0, a41));
        Lib_IntVector_Intrinsics_vec128 t01 = a06;
        Lib_IntVector_Intrinsics_vec128 t11 = a16;
        Lib_IntVector_Intrinsics_vec128 t2 = a26;
        Lib_IntVector_Intrinsics_vec128 t3 = a36;
        Lib_IntVector_Intrinsics_vec128 t4 = a46;
        Lib_IntVector_Intrinsics_vec128 mask26 =
          Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
        Lib_IntVector_Intrinsics_vec128 z0 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(t01, 26U);
        Lib_IntVector_Intrinsics_vec128 z1 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(t3, 26U);
        Lib_IntVector_Intrinsics_vec128 x0 =
          Lib_IntVector_Intrinsics_vec128_and(t01, mask26);
        Lib_IntVector_Intrinsics_vec128 x3 =
          Lib_IntVector_Intrinsics_vec128_and(t3, mask26);
        Lib_IntVector_Intrinsics_vec128 x1 =
          Lib_IntVector_Intrinsics_vec128_add64(t11, z0);
        Lib_IntVector_Intrinsics_vec128 x4 =
          Lib_IntVector_Intrinsics_vec128_add64(t4, z1);
        Lib_IntVector_Intrinsics_vec128 z01 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x1, 26U);
        Lib_IntVector_Intrinsics_vec128 z11 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x4, 26U);
        Lib_IntVector_Intrinsics_vec128 t =
          Lib_IntVector_Intrinsics_vec128_shift_left64(z11, 2U);
        Lib_IntVector_Intrinsics_vec128 z12 =
          Lib_IntVector_Intrinsics_vec128_add64(z11, t);
        Lib_IntVector_Intrinsics_vec128 x11 =
          Lib_IntVector_Intrinsics_vec128_and(x1, mask26);
        Lib_IntVector_Intrinsics_vec128 x41 =
          Lib_IntVector_Intrinsics_vec128_and(x4, mask26);
        Lib_IntVector_Intrinsics_vec128 x2 =
          Lib_IntVector_Intrinsics_vec128_add64(t2, z01);
        Lib_IntVector_Intrinsics_vec128 x01 =
          Lib_IntVector_Intrinsics_vec128_add64(x0, z12);
        Lib_IntVector_Intrinsics_vec128 z02 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x2, 26U);
        Lib_IntVector_Intrinsics_vec128 z13 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x01, 26U);
        Lib_IntVector_Intrinsics_vec128 x21 =
          Lib_IntVector_Intrinsics_vec128_and(x2, mask26);
        Lib_IntVector_Intrinsics_vec128 x02 =
          Lib_IntVector_Intrinsics_vec128_and(x01, mask26);
        Lib_IntVector_Intrinsics_vec128 x31 =
          Lib_IntVector_Intrinsics_vec128_add64(x3, z02);
        Lib_IntVector_Intrinsics_vec128 x12 =
          Lib_IntVector_Intrinsics_vec128_add64(x11, z13);
        Lib_IntVector_Intrinsics_vec128 z03 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x31, 26U);
        Lib_IntVector_Intrinsics_vec128 x32 =
          Lib_IntVector_Intrinsics_vec128_and(x31, mask26);
        Lib_IntVector_Intrinsics_vec128 x42 =
          Lib_IntVector_Intrinsics_vec128_add64(x41, z03);
        Lib_IntVector_Intrinsics_vec128 o0 = x02;
        Lib_IntVector_Intrinsics_vec128 o1 = x12;
        Lib_IntVector_Intrinsics_vec128 o2 = x21;
        Lib_IntVector_Intrinsics_vec128 o3 = x32;
        Lib_IntVector_Intrinsics_vec128 o4 = x42;
        acc[0U] = o0;
        acc[1U] = o1;
        acc[2U] = o2;
        acc[3U] = o3;
        acc[4U] = o4;
    }
    if (rem > 0U) {
        uint8_t* last = t1 + nb * 16U;
        KRML_PRE_ALIGN(16)
        Lib_IntVector_Intrinsics_vec128 e[5U] KRML_POST_ALIGN(16) = { 0U };
        uint8_t tmp[16U] = { 0U };
        memcpy(tmp, last, rem * sizeof(uint8_t));
        uint64_t u0 = READ64LE(tmp);
        uint64_t lo = u0;
        uint64_t u = READ64LE(tmp + 8U);
        uint64_t hi = u;
        Lib_IntVector_Intrinsics_vec128 f0 =
          Lib_IntVector_Intrinsics_vec128_load64(lo);
        Lib_IntVector_Intrinsics_vec128 f1 =
          Lib_IntVector_Intrinsics_vec128_load64(hi);
        Lib_IntVector_Intrinsics_vec128 f010 =
          Lib_IntVector_Intrinsics_vec128_and(
            f0, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f110 =
          Lib_IntVector_Intrinsics_vec128_and(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f0, 26U),
            Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f20 =
          Lib_IntVector_Intrinsics_vec128_or(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f0, 52U),
            Lib_IntVector_Intrinsics_vec128_shift_left64(
              Lib_IntVector_Intrinsics_vec128_and(
                f1, Lib_IntVector_Intrinsics_vec128_load64(0x3fffULL)),
              12U));
        Lib_IntVector_Intrinsics_vec128 f30 =
          Lib_IntVector_Intrinsics_vec128_and(
            Lib_IntVector_Intrinsics_vec128_shift_right64(f1, 14U),
            Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec128 f40 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(f1, 40U);
        Lib_IntVector_Intrinsics_vec128 f01 = f010;
        Lib_IntVector_Intrinsics_vec128 f111 = f110;
        Lib_IntVector_Intrinsics_vec128 f2 = f20;
        Lib_IntVector_Intrinsics_vec128 f3 = f30;
        Lib_IntVector_Intrinsics_vec128 f4 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f4;
        uint64_t b = 1ULL << rem * 8U % 26U;
        Lib_IntVector_Intrinsics_vec128 mask =
          Lib_IntVector_Intrinsics_vec128_load64(b);
        Lib_IntVector_Intrinsics_vec128 fi = e[rem * 8U / 26U];
        e[rem * 8U / 26U] = Lib_IntVector_Intrinsics_vec128_or(fi, mask);
        Lib_IntVector_Intrinsics_vec128* r = pre;
        Lib_IntVector_Intrinsics_vec128* r5 = pre + 5U;
        Lib_IntVector_Intrinsics_vec128 r0 = r[0U];
        Lib_IntVector_Intrinsics_vec128 r1 = r[1U];
        Lib_IntVector_Intrinsics_vec128 r2 = r[2U];
        Lib_IntVector_Intrinsics_vec128 r3 = r[3U];
        Lib_IntVector_Intrinsics_vec128 r4 = r[4U];
        Lib_IntVector_Intrinsics_vec128 r51 = r5[1U];
        Lib_IntVector_Intrinsics_vec128 r52 = r5[2U];
        Lib_IntVector_Intrinsics_vec128 r53 = r5[3U];
        Lib_IntVector_Intrinsics_vec128 r54 = r5[4U];
        Lib_IntVector_Intrinsics_vec128 f10 = e[0U];
        Lib_IntVector_Intrinsics_vec128 f11 = e[1U];
        Lib_IntVector_Intrinsics_vec128 f12 = e[2U];
        Lib_IntVector_Intrinsics_vec128 f13 = e[3U];
        Lib_IntVector_Intrinsics_vec128 f14 = e[4U];
        Lib_IntVector_Intrinsics_vec128 a0 = acc[0U];
        Lib_IntVector_Intrinsics_vec128 a1 = acc[1U];
        Lib_IntVector_Intrinsics_vec128 a2 = acc[2U];
        Lib_IntVector_Intrinsics_vec128 a3 = acc[3U];
        Lib_IntVector_Intrinsics_vec128 a4 = acc[4U];
        Lib_IntVector_Intrinsics_vec128 a01 =
          Lib_IntVector_Intrinsics_vec128_add64(a0, f10);
        Lib_IntVector_Intrinsics_vec128 a11 =
          Lib_IntVector_Intrinsics_vec128_add64(a1, f11);
        Lib_IntVector_Intrinsics_vec128 a21 =
          Lib_IntVector_Intrinsics_vec128_add64(a2, f12);
        Lib_IntVector_Intrinsics_vec128 a31 =
          Lib_IntVector_Intrinsics_vec128_add64(a3, f13);
        Lib_IntVector_Intrinsics_vec128 a41 =
          Lib_IntVector_Intrinsics_vec128_add64(a4, f14);
        Lib_IntVector_Intrinsics_vec128 a02 =
          Lib_IntVector_Intrinsics_vec128_mul64(r0, a01);
        Lib_IntVector_Intrinsics_vec128 a12 =
          Lib_IntVector_Intrinsics_vec128_mul64(r1, a01);
        Lib_IntVector_Intrinsics_vec128 a22 =
          Lib_IntVector_Intrinsics_vec128_mul64(r2, a01);
        Lib_IntVector_Intrinsics_vec128 a32 =
          Lib_IntVector_Intrinsics_vec128_mul64(r3, a01);
        Lib_IntVector_Intrinsics_vec128 a42 =
          Lib_IntVector_Intrinsics_vec128_mul64(r4, a01);
        Lib_IntVector_Intrinsics_vec128 a03 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a02, Lib_IntVector_Intrinsics_vec128_mul64(r54, a11));
        Lib_IntVector_Intrinsics_vec128 a13 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a12, Lib_IntVector_Intrinsics_vec128_mul64(r0, a11));
        Lib_IntVector_Intrinsics_vec128 a23 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a22, Lib_IntVector_Intrinsics_vec128_mul64(r1, a11));
        Lib_IntVector_Intrinsics_vec128 a33 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a32, Lib_IntVector_Intrinsics_vec128_mul64(r2, a11));
        Lib_IntVector_Intrinsics_vec128 a43 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a42, Lib_IntVector_Intrinsics_vec128_mul64(r3, a11));
        Lib_IntVector_Intrinsics_vec128 a04 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a03, Lib_IntVector_Intrinsics_vec128_mul64(r53, a21));
        Lib_IntVector_Intrinsics_vec128 a14 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a13, Lib_IntVector_Intrinsics_vec128_mul64(r54, a21));
        Lib_IntVector_Intrinsics_vec128 a24 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a23, Lib_IntVector_Intrinsics_vec128_mul64(r0, a21));
        Lib_IntVector_Intrinsics_vec128 a34 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a33, Lib_IntVector_Intrinsics_vec128_mul64(r1, a21));
        Lib_IntVector_Intrinsics_vec128 a44 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a43, Lib_IntVector_Intrinsics_vec128_mul64(r2, a21));
        Lib_IntVector_Intrinsics_vec128 a05 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a04, Lib_IntVector_Intrinsics_vec128_mul64(r52, a31));
        Lib_IntVector_Intrinsics_vec128 a15 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a14, Lib_IntVector_Intrinsics_vec128_mul64(r53, a31));
        Lib_IntVector_Intrinsics_vec128 a25 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a24, Lib_IntVector_Intrinsics_vec128_mul64(r54, a31));
        Lib_IntVector_Intrinsics_vec128 a35 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a34, Lib_IntVector_Intrinsics_vec128_mul64(r0, a31));
        Lib_IntVector_Intrinsics_vec128 a45 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a44, Lib_IntVector_Intrinsics_vec128_mul64(r1, a31));
        Lib_IntVector_Intrinsics_vec128 a06 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a05, Lib_IntVector_Intrinsics_vec128_mul64(r51, a41));
        Lib_IntVector_Intrinsics_vec128 a16 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a15, Lib_IntVector_Intrinsics_vec128_mul64(r52, a41));
        Lib_IntVector_Intrinsics_vec128 a26 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a25, Lib_IntVector_Intrinsics_vec128_mul64(r53, a41));
        Lib_IntVector_Intrinsics_vec128 a36 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a35, Lib_IntVector_Intrinsics_vec128_mul64(r54, a41));
        Lib_IntVector_Intrinsics_vec128 a46 =
          Lib_IntVector_Intrinsics_vec128_add64(
            a45, Lib_IntVector_Intrinsics_vec128_mul64(r0, a41));
        Lib_IntVector_Intrinsics_vec128 t01 = a06;
        Lib_IntVector_Intrinsics_vec128 t11 = a16;
        Lib_IntVector_Intrinsics_vec128 t2 = a26;
        Lib_IntVector_Intrinsics_vec128 t3 = a36;
        Lib_IntVector_Intrinsics_vec128 t4 = a46;
        Lib_IntVector_Intrinsics_vec128 mask26 =
          Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
        Lib_IntVector_Intrinsics_vec128 z0 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(t01, 26U);
        Lib_IntVector_Intrinsics_vec128 z1 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(t3, 26U);
        Lib_IntVector_Intrinsics_vec128 x0 =
          Lib_IntVector_Intrinsics_vec128_and(t01, mask26);
        Lib_IntVector_Intrinsics_vec128 x3 =
          Lib_IntVector_Intrinsics_vec128_and(t3, mask26);
        Lib_IntVector_Intrinsics_vec128 x1 =
          Lib_IntVector_Intrinsics_vec128_add64(t11, z0);
        Lib_IntVector_Intrinsics_vec128 x4 =
          Lib_IntVector_Intrinsics_vec128_add64(t4, z1);
        Lib_IntVector_Intrinsics_vec128 z01 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x1, 26U);
        Lib_IntVector_Intrinsics_vec128 z11 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x4, 26U);
        Lib_IntVector_Intrinsics_vec128 t =
          Lib_IntVector_Intrinsics_vec128_shift_left64(z11, 2U);
        Lib_IntVector_Intrinsics_vec128 z12 =
          Lib_IntVector_Intrinsics_vec128_add64(z11, t);
        Lib_IntVector_Intrinsics_vec128 x11 =
          Lib_IntVector_Intrinsics_vec128_and(x1, mask26);
        Lib_IntVector_Intrinsics_vec128 x41 =
          Lib_IntVector_Intrinsics_vec128_and(x4, mask26);
        Lib_IntVector_Intrinsics_vec128 x2 =
          Lib_IntVector_Intrinsics_vec128_add64(t2, z01);
        Lib_IntVector_Intrinsics_vec128 x01 =
          Lib_IntVector_Intrinsics_vec128_add64(x0, z12);
        Lib_IntVector_Intrinsics_vec128 z02 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x2, 26U);
        Lib_IntVector_Intrinsics_vec128 z13 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x01, 26U);
        Lib_IntVector_Intrinsics_vec128 x21 =
          Lib_IntVector_Intrinsics_vec128_and(x2, mask26);
        Lib_IntVector_Intrinsics_vec128 x02 =
          Lib_IntVector_Intrinsics_vec128_and(x01, mask26);
        Lib_IntVector_Intrinsics_vec128 x31 =
          Lib_IntVector_Intrinsics_vec128_add64(x3, z02);
        Lib_IntVector_Intrinsics_vec128 x12 =
          Lib_IntVector_Intrinsics_vec128_add64(x11, z13);
        Lib_IntVector_Intrinsics_vec128 z03 =
          Lib_IntVector_Intrinsics_vec128_shift_right64(x31, 26U);
        Lib_IntVector_Intrinsics_vec128 x32 =
          Lib_IntVector_Intrinsics_vec128_and(x31, mask26);
        Lib_IntVector_Intrinsics_vec128 x42 =
          Lib_IntVector_Intrinsics_vec128_add64(x41, z03);
        Lib_IntVector_Intrinsics_vec128 o0 = x02;
        Lib_IntVector_Intrinsics_vec128 o1 = x12;
        Lib_IntVector_Intrinsics_vec128 o2 = x21;
        Lib_IntVector_Intrinsics_vec128 o3 = x32;
        Lib_IntVector_Intrinsics_vec128 o4 = x42;
        acc[0U] = o0;
        acc[1U] = o1;
        acc[2U] = o2;
        acc[3U] = o3;
        acc[4U] = o4;
        return;
    }
}

void
Hacl_MAC_Poly1305_Simd128_poly1305_finish(uint8_t* tag,
                                          uint8_t* key,
                                          Lib_IntVector_Intrinsics_vec128* ctx)
{
    Lib_IntVector_Intrinsics_vec128* acc = ctx;
    uint8_t* ks = key + 16U;
    Lib_IntVector_Intrinsics_vec128 f0 = acc[0U];
    Lib_IntVector_Intrinsics_vec128 f13 = acc[1U];
    Lib_IntVector_Intrinsics_vec128 f23 = acc[2U];
    Lib_IntVector_Intrinsics_vec128 f33 = acc[3U];
    Lib_IntVector_Intrinsics_vec128 f40 = acc[4U];
    Lib_IntVector_Intrinsics_vec128 l0 = Lib_IntVector_Intrinsics_vec128_add64(
      f0, Lib_IntVector_Intrinsics_vec128_zero);
    Lib_IntVector_Intrinsics_vec128 tmp00 = Lib_IntVector_Intrinsics_vec128_and(
      l0, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c00 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l0, 26U);
    Lib_IntVector_Intrinsics_vec128 l1 =
      Lib_IntVector_Intrinsics_vec128_add64(f13, c00);
    Lib_IntVector_Intrinsics_vec128 tmp10 = Lib_IntVector_Intrinsics_vec128_and(
      l1, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c10 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l1, 26U);
    Lib_IntVector_Intrinsics_vec128 l2 =
      Lib_IntVector_Intrinsics_vec128_add64(f23, c10);
    Lib_IntVector_Intrinsics_vec128 tmp20 = Lib_IntVector_Intrinsics_vec128_and(
      l2, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c20 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l2, 26U);
    Lib_IntVector_Intrinsics_vec128 l3 =
      Lib_IntVector_Intrinsics_vec128_add64(f33, c20);
    Lib_IntVector_Intrinsics_vec128 tmp30 = Lib_IntVector_Intrinsics_vec128_and(
      l3, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c30 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l3, 26U);
    Lib_IntVector_Intrinsics_vec128 l4 =
      Lib_IntVector_Intrinsics_vec128_add64(f40, c30);
    Lib_IntVector_Intrinsics_vec128 tmp40 = Lib_IntVector_Intrinsics_vec128_and(
      l4, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c40 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l4, 26U);
    Lib_IntVector_Intrinsics_vec128 f010 =
      Lib_IntVector_Intrinsics_vec128_add64(
        tmp00, Lib_IntVector_Intrinsics_vec128_smul64(c40, 5ULL));
    Lib_IntVector_Intrinsics_vec128 f110 = tmp10;
    Lib_IntVector_Intrinsics_vec128 f210 = tmp20;
    Lib_IntVector_Intrinsics_vec128 f310 = tmp30;
    Lib_IntVector_Intrinsics_vec128 f410 = tmp40;
    Lib_IntVector_Intrinsics_vec128 l = Lib_IntVector_Intrinsics_vec128_add64(
      f010, Lib_IntVector_Intrinsics_vec128_zero);
    Lib_IntVector_Intrinsics_vec128 tmp0 = Lib_IntVector_Intrinsics_vec128_and(
      l, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c0 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l, 26U);
    Lib_IntVector_Intrinsics_vec128 l5 =
      Lib_IntVector_Intrinsics_vec128_add64(f110, c0);
    Lib_IntVector_Intrinsics_vec128 tmp1 = Lib_IntVector_Intrinsics_vec128_and(
      l5, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c1 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l5, 26U);
    Lib_IntVector_Intrinsics_vec128 l6 =
      Lib_IntVector_Intrinsics_vec128_add64(f210, c1);
    Lib_IntVector_Intrinsics_vec128 tmp2 = Lib_IntVector_Intrinsics_vec128_and(
      l6, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c2 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l6, 26U);
    Lib_IntVector_Intrinsics_vec128 l7 =
      Lib_IntVector_Intrinsics_vec128_add64(f310, c2);
    Lib_IntVector_Intrinsics_vec128 tmp3 = Lib_IntVector_Intrinsics_vec128_and(
      l7, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c3 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l7, 26U);
    Lib_IntVector_Intrinsics_vec128 l8 =
      Lib_IntVector_Intrinsics_vec128_add64(f410, c3);
    Lib_IntVector_Intrinsics_vec128 tmp4 = Lib_IntVector_Intrinsics_vec128_and(
      l8, Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec128 c4 =
      Lib_IntVector_Intrinsics_vec128_shift_right64(l8, 26U);
    Lib_IntVector_Intrinsics_vec128 f02 = Lib_IntVector_Intrinsics_vec128_add64(
      tmp0, Lib_IntVector_Intrinsics_vec128_smul64(c4, 5ULL));
    Lib_IntVector_Intrinsics_vec128 f12 = tmp1;
    Lib_IntVector_Intrinsics_vec128 f22 = tmp2;
    Lib_IntVector_Intrinsics_vec128 f32 = tmp3;
    Lib_IntVector_Intrinsics_vec128 f42 = tmp4;
    Lib_IntVector_Intrinsics_vec128 mh =
      Lib_IntVector_Intrinsics_vec128_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec128 ml =
      Lib_IntVector_Intrinsics_vec128_load64(0x3fffffbULL);
    Lib_IntVector_Intrinsics_vec128 mask =
      Lib_IntVector_Intrinsics_vec128_eq64(f42, mh);
    Lib_IntVector_Intrinsics_vec128 mask1 = Lib_IntVector_Intrinsics_vec128_and(
      mask, Lib_IntVector_Intrinsics_vec128_eq64(f32, mh));
    Lib_IntVector_Intrinsics_vec128 mask2 = Lib_IntVector_Intrinsics_vec128_and(
      mask1, Lib_IntVector_Intrinsics_vec128_eq64(f22, mh));
    Lib_IntVector_Intrinsics_vec128 mask3 = Lib_IntVector_Intrinsics_vec128_and(
      mask2, Lib_IntVector_Intrinsics_vec128_eq64(f12, mh));
    Lib_IntVector_Intrinsics_vec128 mask4 = Lib_IntVector_Intrinsics_vec128_and(
      mask3,
      Lib_IntVector_Intrinsics_vec128_lognot(
        Lib_IntVector_Intrinsics_vec128_gt64(ml, f02)));
    Lib_IntVector_Intrinsics_vec128 ph =
      Lib_IntVector_Intrinsics_vec128_and(mask4, mh);
    Lib_IntVector_Intrinsics_vec128 pl =
      Lib_IntVector_Intrinsics_vec128_and(mask4, ml);
    Lib_IntVector_Intrinsics_vec128 o0 =
      Lib_IntVector_Intrinsics_vec128_sub64(f02, pl);
    Lib_IntVector_Intrinsics_vec128 o1 =
      Lib_IntVector_Intrinsics_vec128_sub64(f12, ph);
    Lib_IntVector_Intrinsics_vec128 o2 =
      Lib_IntVector_Intrinsics_vec128_sub64(f22, ph);
    Lib_IntVector_Intrinsics_vec128 o3 =
      Lib_IntVector_Intrinsics_vec128_sub64(f32, ph);
    Lib_IntVector_Intrinsics_vec128 o4 =
      Lib_IntVector_Intrinsics_vec128_sub64(f42, ph);
    Lib_IntVector_Intrinsics_vec128 f011 = o0;
    Lib_IntVector_Intrinsics_vec128 f111 = o1;
    Lib_IntVector_Intrinsics_vec128 f211 = o2;
    Lib_IntVector_Intrinsics_vec128 f311 = o3;
    Lib_IntVector_Intrinsics_vec128 f411 = o4;
    acc[0U] = f011;
    acc[1U] = f111;
    acc[2U] = f211;
    acc[3U] = f311;
    acc[4U] = f411;
    Lib_IntVector_Intrinsics_vec128 f00 = acc[0U];
    Lib_IntVector_Intrinsics_vec128 f1 = acc[1U];
    Lib_IntVector_Intrinsics_vec128 f2 = acc[2U];
    Lib_IntVector_Intrinsics_vec128 f3 = acc[3U];
    Lib_IntVector_Intrinsics_vec128 f4 = acc[4U];
    uint64_t f01 = Lib_IntVector_Intrinsics_vec128_extract64(f00, 0U);
    uint64_t f112 = Lib_IntVector_Intrinsics_vec128_extract64(f1, 0U);
    uint64_t f212 = Lib_IntVector_Intrinsics_vec128_extract64(f2, 0U);
    uint64_t f312 = Lib_IntVector_Intrinsics_vec128_extract64(f3, 0U);
    uint64_t f41 = Lib_IntVector_Intrinsics_vec128_extract64(f4, 0U);
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

typedef struct option___Lib_IntVector_Intrinsics_vec128__s
{
    Hacl_Streaming_Types_optional tag;
    Lib_IntVector_Intrinsics_vec128* v;
} option___Lib_IntVector_Intrinsics_vec128_;

Hacl_MAC_Poly1305_Simd128_state_t*
Hacl_MAC_Poly1305_Simd128_malloc(uint8_t* key)
{
    uint8_t* buf = (uint8_t*)calloc(32U, sizeof(uint8_t));
    if (buf == NULL) {
        return NULL;
    }
    uint8_t* buf1 = buf;
    Lib_IntVector_Intrinsics_vec128* r1 =
      (Lib_IntVector_Intrinsics_vec128*)memalign(
        16, sizeof(Lib_IntVector_Intrinsics_vec128) * 25U);
    if (r1 != NULL) {
        memset(r1, 0U, 25U * sizeof(Lib_IntVector_Intrinsics_vec128));
    }
    option___Lib_IntVector_Intrinsics_vec128_ block_state;
    if (r1 == NULL) {
        block_state = ((option___Lib_IntVector_Intrinsics_vec128_){
          .tag = Hacl_Streaming_Types_None });
    } else {
        block_state = ((option___Lib_IntVector_Intrinsics_vec128_){
          .tag = Hacl_Streaming_Types_Some, .v = r1 });
    }
    if (block_state.tag == Hacl_Streaming_Types_None) {
        free(buf1);
        return NULL;
    }
    if (block_state.tag == Hacl_Streaming_Types_Some) {
        Lib_IntVector_Intrinsics_vec128* block_state1 = block_state.v;
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
            Hacl_MAC_Poly1305_Simd128_state_t s = { .block_state = block_state1,
                                                    .buf = buf1,
                                                    .total_len = (uint64_t)0U,
                                                    .p_key = k_1 };
            Hacl_MAC_Poly1305_Simd128_state_t* p =
              (Hacl_MAC_Poly1305_Simd128_state_t*)malloc(
                sizeof(Hacl_MAC_Poly1305_Simd128_state_t));
            if (p != NULL) {
                p[0U] = s;
            }
            if (p == NULL) {
                free(k_1);
                free(block_state1);
                free(buf1);
                return NULL;
            }
            Hacl_MAC_Poly1305_Simd128_poly1305_init(block_state1, key);
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
Hacl_MAC_Poly1305_Simd128_reset(Hacl_MAC_Poly1305_Simd128_state_t* state,
                                uint8_t* key)
{
    Hacl_MAC_Poly1305_Simd128_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    Lib_IntVector_Intrinsics_vec128* block_state = scrut.block_state;
    Hacl_MAC_Poly1305_Simd128_poly1305_init(block_state, key);
    memcpy(k_, key, 32U * sizeof(uint8_t));
    uint8_t* k_1 = k_;
    Hacl_MAC_Poly1305_Simd128_state_t tmp = { .block_state = block_state,
                                              .buf = buf,
                                              .total_len = (uint64_t)0U,
                                              .p_key = k_1 };
    state[0U] = tmp;
}

/**
0 = success, 1 = max length exceeded
*/
Hacl_Streaming_Types_error_code
Hacl_MAC_Poly1305_Simd128_update(Hacl_MAC_Poly1305_Simd128_state_t* state,
                                 uint8_t* chunk,
                                 uint32_t chunk_len)
{
    Hacl_MAC_Poly1305_Simd128_state_t s = *state;
    uint64_t total_len = s.total_len;
    if ((uint64_t)chunk_len > 0xffffffffULL - total_len) {
        return Hacl_Streaming_Types_MaximumLengthExceeded;
    }
    uint32_t sz;
    if (total_len % (uint64_t)32U == 0ULL && total_len > 0ULL) {
        sz = 32U;
    } else {
        sz = (uint32_t)(total_len % (uint64_t)32U);
    }
    if (chunk_len <= 32U - sz) {
        Hacl_MAC_Poly1305_Simd128_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec128* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)32U == 0ULL && total_len1 > 0ULL) {
            sz1 = 32U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)32U);
        }
        uint8_t* buf2 = buf + sz1;
        memcpy(buf2, chunk, chunk_len * sizeof(uint8_t));
        uint64_t total_len2 = total_len1 + (uint64_t)chunk_len;
        *state =
          ((Hacl_MAC_Poly1305_Simd128_state_t){ .block_state = block_state1,
                                                .buf = buf,
                                                .total_len = total_len2,
                                                .p_key = k_1 });
    } else if (sz == 0U) {
        Hacl_MAC_Poly1305_Simd128_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec128* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)32U == 0ULL && total_len1 > 0ULL) {
            sz1 = 32U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)32U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 32U, buf);
        }
        uint32_t ite;
        if ((uint64_t)chunk_len % (uint64_t)32U == 0ULL &&
            (uint64_t)chunk_len > 0ULL) {
            ite = 32U;
        } else {
            ite = (uint32_t)((uint64_t)chunk_len % (uint64_t)32U);
        }
        uint32_t n_blocks = (chunk_len - ite) / 32U;
        uint32_t data1_len = n_blocks * 32U;
        uint32_t data2_len = chunk_len - data1_len;
        uint8_t* data1 = chunk;
        uint8_t* data2 = chunk + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_Simd128_state_t){
          .block_state = block_state1,
          .buf = buf,
          .total_len = total_len1 + (uint64_t)chunk_len,
          .p_key = k_1 });
    } else {
        uint32_t diff = 32U - sz;
        uint8_t* chunk1 = chunk;
        uint8_t* chunk2 = chunk + diff;
        Hacl_MAC_Poly1305_Simd128_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec128* block_state10 = s1.block_state;
        uint8_t* buf0 = s1.buf;
        uint64_t total_len10 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz10;
        if (total_len10 % (uint64_t)32U == 0ULL && total_len10 > 0ULL) {
            sz10 = 32U;
        } else {
            sz10 = (uint32_t)(total_len10 % (uint64_t)32U);
        }
        uint8_t* buf2 = buf0 + sz10;
        memcpy(buf2, chunk1, diff * sizeof(uint8_t));
        uint64_t total_len2 = total_len10 + (uint64_t)diff;
        *state =
          ((Hacl_MAC_Poly1305_Simd128_state_t){ .block_state = block_state10,
                                                .buf = buf0,
                                                .total_len = total_len2,
                                                .p_key = k_1 });
        Hacl_MAC_Poly1305_Simd128_state_t s10 = *state;
        Lib_IntVector_Intrinsics_vec128* block_state1 = s10.block_state;
        uint8_t* buf = s10.buf;
        uint64_t total_len1 = s10.total_len;
        uint8_t* k_10 = s10.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)32U == 0ULL && total_len1 > 0ULL) {
            sz1 = 32U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)32U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 32U, buf);
        }
        uint32_t ite;
        if ((uint64_t)(chunk_len - diff) % (uint64_t)32U == 0ULL &&
            (uint64_t)(chunk_len - diff) > 0ULL) {
            ite = 32U;
        } else {
            ite = (uint32_t)((uint64_t)(chunk_len - diff) % (uint64_t)32U);
        }
        uint32_t n_blocks = (chunk_len - diff - ite) / 32U;
        uint32_t data1_len = n_blocks * 32U;
        uint32_t data2_len = chunk_len - diff - data1_len;
        uint8_t* data1 = chunk2;
        uint8_t* data2 = chunk2 + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_Simd128_state_t){
          .block_state = block_state1,
          .buf = buf,
          .total_len = total_len1 + (uint64_t)(chunk_len - diff),
          .p_key = k_10 });
    }
    return Hacl_Streaming_Types_Success;
}

void
Hacl_MAC_Poly1305_Simd128_digest(Hacl_MAC_Poly1305_Simd128_state_t* state,
                                 uint8_t* output)
{
    Hacl_MAC_Poly1305_Simd128_state_t scrut = *state;
    Lib_IntVector_Intrinsics_vec128* block_state = scrut.block_state;
    uint8_t* buf_ = scrut.buf;
    uint64_t total_len = scrut.total_len;
    uint8_t* k_ = scrut.p_key;
    uint32_t r;
    if (total_len % (uint64_t)32U == 0ULL && total_len > 0ULL) {
        r = 32U;
    } else {
        r = (uint32_t)(total_len % (uint64_t)32U);
    }
    uint8_t* buf_1 = buf_;
    KRML_PRE_ALIGN(16)
    Lib_IntVector_Intrinsics_vec128 r1[25U] KRML_POST_ALIGN(16) = { 0U };
    Lib_IntVector_Intrinsics_vec128* tmp_block_state = r1;
    memcpy(tmp_block_state,
           block_state,
           25U * sizeof(Lib_IntVector_Intrinsics_vec128));
    uint32_t ite0;
    if (r % 16U == 0U && r > 0U) {
        ite0 = 16U;
    } else {
        ite0 = r % 16U;
    }
    uint8_t* buf_last = buf_1 + r - ite0;
    uint8_t* buf_multi = buf_1;
    uint32_t ite;
    if (r % 16U == 0U && r > 0U) {
        ite = 16U;
    } else {
        ite = r % 16U;
    }
    poly1305_update(tmp_block_state, r - ite, buf_multi);
    uint32_t ite1;
    if (r % 16U == 0U && r > 0U) {
        ite1 = 16U;
    } else {
        ite1 = r % 16U;
    }
    poly1305_update(tmp_block_state, ite1, buf_last);
    KRML_PRE_ALIGN(16)
    Lib_IntVector_Intrinsics_vec128 tmp[25U] KRML_POST_ALIGN(16) = { 0U };
    memcpy(tmp, tmp_block_state, 25U * sizeof(Lib_IntVector_Intrinsics_vec128));
    Hacl_MAC_Poly1305_Simd128_poly1305_finish(output, k_, tmp);
}

void
Hacl_MAC_Poly1305_Simd128_free(Hacl_MAC_Poly1305_Simd128_state_t* state)
{
    Hacl_MAC_Poly1305_Simd128_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    Lib_IntVector_Intrinsics_vec128* block_state = scrut.block_state;
    free(k_);
    free(block_state);
    free(buf);
    free(state);
}

void
Hacl_MAC_Poly1305_Simd128_mac(uint8_t* output,
                              uint8_t* input,
                              uint32_t input_len,
                              uint8_t* key)
{
    KRML_PRE_ALIGN(16)
    Lib_IntVector_Intrinsics_vec128 ctx[25U] KRML_POST_ALIGN(16) = { 0U };
    Hacl_MAC_Poly1305_Simd128_poly1305_init(ctx, key);
    poly1305_update(ctx, input_len, input);
    Hacl_MAC_Poly1305_Simd128_poly1305_finish(output, key, ctx);
}
