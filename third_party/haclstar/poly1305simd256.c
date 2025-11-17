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
#include "third_party/haclstar/simd256.h"
__static_yoink("hacl_star_notice");

#ifdef __x86_64__

typedef struct Hacl_MAC_Poly1305_Simd256_state_t_s
{
    Lib_IntVector_Intrinsics_vec256* block_state;
    uint8_t* buf;
    uint64_t total_len;
    uint8_t* p_key;
} Hacl_MAC_Poly1305_Simd256_state_t;

void
Hacl_MAC_Poly1305_Simd256_load_acc4(Lib_IntVector_Intrinsics_vec256* acc,
                                    uint8_t* b)
{
    KRML_PRE_ALIGN(32)
    Lib_IntVector_Intrinsics_vec256 e[5U] KRML_POST_ALIGN(32) = { 0U };
    Lib_IntVector_Intrinsics_vec256 lo =
      Lib_IntVector_Intrinsics_vec256_load64_le(b);
    Lib_IntVector_Intrinsics_vec256 hi =
      Lib_IntVector_Intrinsics_vec256_load64_le(b + 32U);
    Lib_IntVector_Intrinsics_vec256 mask26 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 m0 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(lo, hi);
    Lib_IntVector_Intrinsics_vec256 m1 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(lo, hi);
    Lib_IntVector_Intrinsics_vec256 m2 =
      Lib_IntVector_Intrinsics_vec256_shift_right(m0, 48U);
    Lib_IntVector_Intrinsics_vec256 m3 =
      Lib_IntVector_Intrinsics_vec256_shift_right(m1, 48U);
    Lib_IntVector_Intrinsics_vec256 m4 =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(m0, m1);
    Lib_IntVector_Intrinsics_vec256 t0 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(m0, m1);
    Lib_IntVector_Intrinsics_vec256 t3 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(m2, m3);
    Lib_IntVector_Intrinsics_vec256 t2 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 4U);
    Lib_IntVector_Intrinsics_vec256 o20 =
      Lib_IntVector_Intrinsics_vec256_and(t2, mask26);
    Lib_IntVector_Intrinsics_vec256 t1 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t0, 26U);
    Lib_IntVector_Intrinsics_vec256 o10 =
      Lib_IntVector_Intrinsics_vec256_and(t1, mask26);
    Lib_IntVector_Intrinsics_vec256 o5 =
      Lib_IntVector_Intrinsics_vec256_and(t0, mask26);
    Lib_IntVector_Intrinsics_vec256 t31 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 30U);
    Lib_IntVector_Intrinsics_vec256 o30 =
      Lib_IntVector_Intrinsics_vec256_and(t31, mask26);
    Lib_IntVector_Intrinsics_vec256 o40 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(m4, 40U);
    Lib_IntVector_Intrinsics_vec256 o0 = o5;
    Lib_IntVector_Intrinsics_vec256 o1 = o10;
    Lib_IntVector_Intrinsics_vec256 o2 = o20;
    Lib_IntVector_Intrinsics_vec256 o3 = o30;
    Lib_IntVector_Intrinsics_vec256 o4 = o40;
    e[0U] = o0;
    e[1U] = o1;
    e[2U] = o2;
    e[3U] = o3;
    e[4U] = o4;
    uint64_t b1 = 0x1000000ULL;
    Lib_IntVector_Intrinsics_vec256 mask =
      Lib_IntVector_Intrinsics_vec256_load64(b1);
    Lib_IntVector_Intrinsics_vec256 f40 = e[4U];
    e[4U] = Lib_IntVector_Intrinsics_vec256_or(f40, mask);
    Lib_IntVector_Intrinsics_vec256 acc0 = acc[0U];
    Lib_IntVector_Intrinsics_vec256 acc1 = acc[1U];
    Lib_IntVector_Intrinsics_vec256 acc2 = acc[2U];
    Lib_IntVector_Intrinsics_vec256 acc3 = acc[3U];
    Lib_IntVector_Intrinsics_vec256 acc4 = acc[4U];
    Lib_IntVector_Intrinsics_vec256 e0 = e[0U];
    Lib_IntVector_Intrinsics_vec256 e1 = e[1U];
    Lib_IntVector_Intrinsics_vec256 e2 = e[2U];
    Lib_IntVector_Intrinsics_vec256 e3 = e[3U];
    Lib_IntVector_Intrinsics_vec256 e4 = e[4U];
    Lib_IntVector_Intrinsics_vec256 r0 = Lib_IntVector_Intrinsics_vec256_zero;
    Lib_IntVector_Intrinsics_vec256 r1 = Lib_IntVector_Intrinsics_vec256_zero;
    Lib_IntVector_Intrinsics_vec256 r2 = Lib_IntVector_Intrinsics_vec256_zero;
    Lib_IntVector_Intrinsics_vec256 r3 = Lib_IntVector_Intrinsics_vec256_zero;
    Lib_IntVector_Intrinsics_vec256 r4 = Lib_IntVector_Intrinsics_vec256_zero;
    Lib_IntVector_Intrinsics_vec256 r01 =
      Lib_IntVector_Intrinsics_vec256_insert64(
        r0, Lib_IntVector_Intrinsics_vec256_extract64(acc0, 0U), 0U);
    Lib_IntVector_Intrinsics_vec256 r11 =
      Lib_IntVector_Intrinsics_vec256_insert64(
        r1, Lib_IntVector_Intrinsics_vec256_extract64(acc1, 0U), 0U);
    Lib_IntVector_Intrinsics_vec256 r21 =
      Lib_IntVector_Intrinsics_vec256_insert64(
        r2, Lib_IntVector_Intrinsics_vec256_extract64(acc2, 0U), 0U);
    Lib_IntVector_Intrinsics_vec256 r31 =
      Lib_IntVector_Intrinsics_vec256_insert64(
        r3, Lib_IntVector_Intrinsics_vec256_extract64(acc3, 0U), 0U);
    Lib_IntVector_Intrinsics_vec256 r41 =
      Lib_IntVector_Intrinsics_vec256_insert64(
        r4, Lib_IntVector_Intrinsics_vec256_extract64(acc4, 0U), 0U);
    Lib_IntVector_Intrinsics_vec256 f0 =
      Lib_IntVector_Intrinsics_vec256_add64(r01, e0);
    Lib_IntVector_Intrinsics_vec256 f1 =
      Lib_IntVector_Intrinsics_vec256_add64(r11, e1);
    Lib_IntVector_Intrinsics_vec256 f2 =
      Lib_IntVector_Intrinsics_vec256_add64(r21, e2);
    Lib_IntVector_Intrinsics_vec256 f3 =
      Lib_IntVector_Intrinsics_vec256_add64(r31, e3);
    Lib_IntVector_Intrinsics_vec256 f4 =
      Lib_IntVector_Intrinsics_vec256_add64(r41, e4);
    Lib_IntVector_Intrinsics_vec256 acc01 = f0;
    Lib_IntVector_Intrinsics_vec256 acc11 = f1;
    Lib_IntVector_Intrinsics_vec256 acc21 = f2;
    Lib_IntVector_Intrinsics_vec256 acc31 = f3;
    Lib_IntVector_Intrinsics_vec256 acc41 = f4;
    acc[0U] = acc01;
    acc[1U] = acc11;
    acc[2U] = acc21;
    acc[3U] = acc31;
    acc[4U] = acc41;
}

void
Hacl_MAC_Poly1305_Simd256_fmul_r4_normalize(
  Lib_IntVector_Intrinsics_vec256* out,
  Lib_IntVector_Intrinsics_vec256* p)
{
    Lib_IntVector_Intrinsics_vec256* r = p;
    Lib_IntVector_Intrinsics_vec256* r_5 = p + 5U;
    Lib_IntVector_Intrinsics_vec256* r4 = p + 10U;
    Lib_IntVector_Intrinsics_vec256 a0 = out[0U];
    Lib_IntVector_Intrinsics_vec256 a1 = out[1U];
    Lib_IntVector_Intrinsics_vec256 a2 = out[2U];
    Lib_IntVector_Intrinsics_vec256 a3 = out[3U];
    Lib_IntVector_Intrinsics_vec256 a4 = out[4U];
    Lib_IntVector_Intrinsics_vec256 r10 = r[0U];
    Lib_IntVector_Intrinsics_vec256 r11 = r[1U];
    Lib_IntVector_Intrinsics_vec256 r12 = r[2U];
    Lib_IntVector_Intrinsics_vec256 r13 = r[3U];
    Lib_IntVector_Intrinsics_vec256 r14 = r[4U];
    Lib_IntVector_Intrinsics_vec256 r151 = r_5[1U];
    Lib_IntVector_Intrinsics_vec256 r152 = r_5[2U];
    Lib_IntVector_Intrinsics_vec256 r153 = r_5[3U];
    Lib_IntVector_Intrinsics_vec256 r154 = r_5[4U];
    Lib_IntVector_Intrinsics_vec256 r40 = r4[0U];
    Lib_IntVector_Intrinsics_vec256 r41 = r4[1U];
    Lib_IntVector_Intrinsics_vec256 r42 = r4[2U];
    Lib_IntVector_Intrinsics_vec256 r43 = r4[3U];
    Lib_IntVector_Intrinsics_vec256 r44 = r4[4U];
    Lib_IntVector_Intrinsics_vec256 a010 =
      Lib_IntVector_Intrinsics_vec256_mul64(r10, r10);
    Lib_IntVector_Intrinsics_vec256 a110 =
      Lib_IntVector_Intrinsics_vec256_mul64(r11, r10);
    Lib_IntVector_Intrinsics_vec256 a210 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12, r10);
    Lib_IntVector_Intrinsics_vec256 a310 =
      Lib_IntVector_Intrinsics_vec256_mul64(r13, r10);
    Lib_IntVector_Intrinsics_vec256 a410 =
      Lib_IntVector_Intrinsics_vec256_mul64(r14, r10);
    Lib_IntVector_Intrinsics_vec256 a020 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a010, Lib_IntVector_Intrinsics_vec256_mul64(r154, r11));
    Lib_IntVector_Intrinsics_vec256 a120 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a110, Lib_IntVector_Intrinsics_vec256_mul64(r10, r11));
    Lib_IntVector_Intrinsics_vec256 a220 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a210, Lib_IntVector_Intrinsics_vec256_mul64(r11, r11));
    Lib_IntVector_Intrinsics_vec256 a320 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a310, Lib_IntVector_Intrinsics_vec256_mul64(r12, r11));
    Lib_IntVector_Intrinsics_vec256 a420 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a410, Lib_IntVector_Intrinsics_vec256_mul64(r13, r11));
    Lib_IntVector_Intrinsics_vec256 a030 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a020, Lib_IntVector_Intrinsics_vec256_mul64(r153, r12));
    Lib_IntVector_Intrinsics_vec256 a130 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a120, Lib_IntVector_Intrinsics_vec256_mul64(r154, r12));
    Lib_IntVector_Intrinsics_vec256 a230 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a220, Lib_IntVector_Intrinsics_vec256_mul64(r10, r12));
    Lib_IntVector_Intrinsics_vec256 a330 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a320, Lib_IntVector_Intrinsics_vec256_mul64(r11, r12));
    Lib_IntVector_Intrinsics_vec256 a430 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a420, Lib_IntVector_Intrinsics_vec256_mul64(r12, r12));
    Lib_IntVector_Intrinsics_vec256 a040 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a030, Lib_IntVector_Intrinsics_vec256_mul64(r152, r13));
    Lib_IntVector_Intrinsics_vec256 a140 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a130, Lib_IntVector_Intrinsics_vec256_mul64(r153, r13));
    Lib_IntVector_Intrinsics_vec256 a240 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a230, Lib_IntVector_Intrinsics_vec256_mul64(r154, r13));
    Lib_IntVector_Intrinsics_vec256 a340 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a330, Lib_IntVector_Intrinsics_vec256_mul64(r10, r13));
    Lib_IntVector_Intrinsics_vec256 a440 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a430, Lib_IntVector_Intrinsics_vec256_mul64(r11, r13));
    Lib_IntVector_Intrinsics_vec256 a050 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a040, Lib_IntVector_Intrinsics_vec256_mul64(r151, r14));
    Lib_IntVector_Intrinsics_vec256 a150 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a140, Lib_IntVector_Intrinsics_vec256_mul64(r152, r14));
    Lib_IntVector_Intrinsics_vec256 a250 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a240, Lib_IntVector_Intrinsics_vec256_mul64(r153, r14));
    Lib_IntVector_Intrinsics_vec256 a350 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a340, Lib_IntVector_Intrinsics_vec256_mul64(r154, r14));
    Lib_IntVector_Intrinsics_vec256 a450 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a440, Lib_IntVector_Intrinsics_vec256_mul64(r10, r14));
    Lib_IntVector_Intrinsics_vec256 t00 = a050;
    Lib_IntVector_Intrinsics_vec256 t10 = a150;
    Lib_IntVector_Intrinsics_vec256 t20 = a250;
    Lib_IntVector_Intrinsics_vec256 t30 = a350;
    Lib_IntVector_Intrinsics_vec256 t40 = a450;
    Lib_IntVector_Intrinsics_vec256 mask260 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 z00 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t00, 26U);
    Lib_IntVector_Intrinsics_vec256 z10 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t30, 26U);
    Lib_IntVector_Intrinsics_vec256 x00 =
      Lib_IntVector_Intrinsics_vec256_and(t00, mask260);
    Lib_IntVector_Intrinsics_vec256 x30 =
      Lib_IntVector_Intrinsics_vec256_and(t30, mask260);
    Lib_IntVector_Intrinsics_vec256 x10 =
      Lib_IntVector_Intrinsics_vec256_add64(t10, z00);
    Lib_IntVector_Intrinsics_vec256 x40 =
      Lib_IntVector_Intrinsics_vec256_add64(t40, z10);
    Lib_IntVector_Intrinsics_vec256 z010 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x10, 26U);
    Lib_IntVector_Intrinsics_vec256 z110 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x40, 26U);
    Lib_IntVector_Intrinsics_vec256 t5 =
      Lib_IntVector_Intrinsics_vec256_shift_left64(z110, 2U);
    Lib_IntVector_Intrinsics_vec256 z12 =
      Lib_IntVector_Intrinsics_vec256_add64(z110, t5);
    Lib_IntVector_Intrinsics_vec256 x110 =
      Lib_IntVector_Intrinsics_vec256_and(x10, mask260);
    Lib_IntVector_Intrinsics_vec256 x410 =
      Lib_IntVector_Intrinsics_vec256_and(x40, mask260);
    Lib_IntVector_Intrinsics_vec256 x20 =
      Lib_IntVector_Intrinsics_vec256_add64(t20, z010);
    Lib_IntVector_Intrinsics_vec256 x010 =
      Lib_IntVector_Intrinsics_vec256_add64(x00, z12);
    Lib_IntVector_Intrinsics_vec256 z020 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x20, 26U);
    Lib_IntVector_Intrinsics_vec256 z130 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x010, 26U);
    Lib_IntVector_Intrinsics_vec256 x210 =
      Lib_IntVector_Intrinsics_vec256_and(x20, mask260);
    Lib_IntVector_Intrinsics_vec256 x020 =
      Lib_IntVector_Intrinsics_vec256_and(x010, mask260);
    Lib_IntVector_Intrinsics_vec256 x310 =
      Lib_IntVector_Intrinsics_vec256_add64(x30, z020);
    Lib_IntVector_Intrinsics_vec256 x120 =
      Lib_IntVector_Intrinsics_vec256_add64(x110, z130);
    Lib_IntVector_Intrinsics_vec256 z030 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x310, 26U);
    Lib_IntVector_Intrinsics_vec256 x320 =
      Lib_IntVector_Intrinsics_vec256_and(x310, mask260);
    Lib_IntVector_Intrinsics_vec256 x420 =
      Lib_IntVector_Intrinsics_vec256_add64(x410, z030);
    Lib_IntVector_Intrinsics_vec256 r20 = x020;
    Lib_IntVector_Intrinsics_vec256 r21 = x120;
    Lib_IntVector_Intrinsics_vec256 r22 = x210;
    Lib_IntVector_Intrinsics_vec256 r23 = x320;
    Lib_IntVector_Intrinsics_vec256 r24 = x420;
    Lib_IntVector_Intrinsics_vec256 a011 =
      Lib_IntVector_Intrinsics_vec256_mul64(r10, r20);
    Lib_IntVector_Intrinsics_vec256 a111 =
      Lib_IntVector_Intrinsics_vec256_mul64(r11, r20);
    Lib_IntVector_Intrinsics_vec256 a211 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12, r20);
    Lib_IntVector_Intrinsics_vec256 a311 =
      Lib_IntVector_Intrinsics_vec256_mul64(r13, r20);
    Lib_IntVector_Intrinsics_vec256 a411 =
      Lib_IntVector_Intrinsics_vec256_mul64(r14, r20);
    Lib_IntVector_Intrinsics_vec256 a021 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a011, Lib_IntVector_Intrinsics_vec256_mul64(r154, r21));
    Lib_IntVector_Intrinsics_vec256 a121 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a111, Lib_IntVector_Intrinsics_vec256_mul64(r10, r21));
    Lib_IntVector_Intrinsics_vec256 a221 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a211, Lib_IntVector_Intrinsics_vec256_mul64(r11, r21));
    Lib_IntVector_Intrinsics_vec256 a321 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a311, Lib_IntVector_Intrinsics_vec256_mul64(r12, r21));
    Lib_IntVector_Intrinsics_vec256 a421 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a411, Lib_IntVector_Intrinsics_vec256_mul64(r13, r21));
    Lib_IntVector_Intrinsics_vec256 a031 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a021, Lib_IntVector_Intrinsics_vec256_mul64(r153, r22));
    Lib_IntVector_Intrinsics_vec256 a131 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a121, Lib_IntVector_Intrinsics_vec256_mul64(r154, r22));
    Lib_IntVector_Intrinsics_vec256 a231 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a221, Lib_IntVector_Intrinsics_vec256_mul64(r10, r22));
    Lib_IntVector_Intrinsics_vec256 a331 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a321, Lib_IntVector_Intrinsics_vec256_mul64(r11, r22));
    Lib_IntVector_Intrinsics_vec256 a431 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a421, Lib_IntVector_Intrinsics_vec256_mul64(r12, r22));
    Lib_IntVector_Intrinsics_vec256 a041 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a031, Lib_IntVector_Intrinsics_vec256_mul64(r152, r23));
    Lib_IntVector_Intrinsics_vec256 a141 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a131, Lib_IntVector_Intrinsics_vec256_mul64(r153, r23));
    Lib_IntVector_Intrinsics_vec256 a241 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a231, Lib_IntVector_Intrinsics_vec256_mul64(r154, r23));
    Lib_IntVector_Intrinsics_vec256 a341 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a331, Lib_IntVector_Intrinsics_vec256_mul64(r10, r23));
    Lib_IntVector_Intrinsics_vec256 a441 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a431, Lib_IntVector_Intrinsics_vec256_mul64(r11, r23));
    Lib_IntVector_Intrinsics_vec256 a051 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a041, Lib_IntVector_Intrinsics_vec256_mul64(r151, r24));
    Lib_IntVector_Intrinsics_vec256 a151 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a141, Lib_IntVector_Intrinsics_vec256_mul64(r152, r24));
    Lib_IntVector_Intrinsics_vec256 a251 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a241, Lib_IntVector_Intrinsics_vec256_mul64(r153, r24));
    Lib_IntVector_Intrinsics_vec256 a351 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a341, Lib_IntVector_Intrinsics_vec256_mul64(r154, r24));
    Lib_IntVector_Intrinsics_vec256 a451 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a441, Lib_IntVector_Intrinsics_vec256_mul64(r10, r24));
    Lib_IntVector_Intrinsics_vec256 t01 = a051;
    Lib_IntVector_Intrinsics_vec256 t11 = a151;
    Lib_IntVector_Intrinsics_vec256 t21 = a251;
    Lib_IntVector_Intrinsics_vec256 t31 = a351;
    Lib_IntVector_Intrinsics_vec256 t41 = a451;
    Lib_IntVector_Intrinsics_vec256 mask261 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 z04 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t01, 26U);
    Lib_IntVector_Intrinsics_vec256 z14 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t31, 26U);
    Lib_IntVector_Intrinsics_vec256 x03 =
      Lib_IntVector_Intrinsics_vec256_and(t01, mask261);
    Lib_IntVector_Intrinsics_vec256 x33 =
      Lib_IntVector_Intrinsics_vec256_and(t31, mask261);
    Lib_IntVector_Intrinsics_vec256 x13 =
      Lib_IntVector_Intrinsics_vec256_add64(t11, z04);
    Lib_IntVector_Intrinsics_vec256 x43 =
      Lib_IntVector_Intrinsics_vec256_add64(t41, z14);
    Lib_IntVector_Intrinsics_vec256 z011 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x13, 26U);
    Lib_IntVector_Intrinsics_vec256 z111 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x43, 26U);
    Lib_IntVector_Intrinsics_vec256 t6 =
      Lib_IntVector_Intrinsics_vec256_shift_left64(z111, 2U);
    Lib_IntVector_Intrinsics_vec256 z120 =
      Lib_IntVector_Intrinsics_vec256_add64(z111, t6);
    Lib_IntVector_Intrinsics_vec256 x111 =
      Lib_IntVector_Intrinsics_vec256_and(x13, mask261);
    Lib_IntVector_Intrinsics_vec256 x411 =
      Lib_IntVector_Intrinsics_vec256_and(x43, mask261);
    Lib_IntVector_Intrinsics_vec256 x22 =
      Lib_IntVector_Intrinsics_vec256_add64(t21, z011);
    Lib_IntVector_Intrinsics_vec256 x011 =
      Lib_IntVector_Intrinsics_vec256_add64(x03, z120);
    Lib_IntVector_Intrinsics_vec256 z021 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x22, 26U);
    Lib_IntVector_Intrinsics_vec256 z131 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x011, 26U);
    Lib_IntVector_Intrinsics_vec256 x211 =
      Lib_IntVector_Intrinsics_vec256_and(x22, mask261);
    Lib_IntVector_Intrinsics_vec256 x021 =
      Lib_IntVector_Intrinsics_vec256_and(x011, mask261);
    Lib_IntVector_Intrinsics_vec256 x311 =
      Lib_IntVector_Intrinsics_vec256_add64(x33, z021);
    Lib_IntVector_Intrinsics_vec256 x121 =
      Lib_IntVector_Intrinsics_vec256_add64(x111, z131);
    Lib_IntVector_Intrinsics_vec256 z031 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x311, 26U);
    Lib_IntVector_Intrinsics_vec256 x321 =
      Lib_IntVector_Intrinsics_vec256_and(x311, mask261);
    Lib_IntVector_Intrinsics_vec256 x421 =
      Lib_IntVector_Intrinsics_vec256_add64(x411, z031);
    Lib_IntVector_Intrinsics_vec256 r30 = x021;
    Lib_IntVector_Intrinsics_vec256 r31 = x121;
    Lib_IntVector_Intrinsics_vec256 r32 = x211;
    Lib_IntVector_Intrinsics_vec256 r33 = x321;
    Lib_IntVector_Intrinsics_vec256 r34 = x421;
    Lib_IntVector_Intrinsics_vec256 v12120 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r20, r10);
    Lib_IntVector_Intrinsics_vec256 v34340 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r40, r30);
    Lib_IntVector_Intrinsics_vec256 r12340 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(v34340, v12120);
    Lib_IntVector_Intrinsics_vec256 v12121 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r21, r11);
    Lib_IntVector_Intrinsics_vec256 v34341 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r41, r31);
    Lib_IntVector_Intrinsics_vec256 r12341 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(v34341, v12121);
    Lib_IntVector_Intrinsics_vec256 v12122 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r22, r12);
    Lib_IntVector_Intrinsics_vec256 v34342 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r42, r32);
    Lib_IntVector_Intrinsics_vec256 r12342 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(v34342, v12122);
    Lib_IntVector_Intrinsics_vec256 v12123 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r23, r13);
    Lib_IntVector_Intrinsics_vec256 v34343 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r43, r33);
    Lib_IntVector_Intrinsics_vec256 r12343 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(v34343, v12123);
    Lib_IntVector_Intrinsics_vec256 v12124 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r24, r14);
    Lib_IntVector_Intrinsics_vec256 v34344 =
      Lib_IntVector_Intrinsics_vec256_interleave_low64(r44, r34);
    Lib_IntVector_Intrinsics_vec256 r12344 =
      Lib_IntVector_Intrinsics_vec256_interleave_low128(v34344, v12124);
    Lib_IntVector_Intrinsics_vec256 r123451 =
      Lib_IntVector_Intrinsics_vec256_smul64(r12341, 5ULL);
    Lib_IntVector_Intrinsics_vec256 r123452 =
      Lib_IntVector_Intrinsics_vec256_smul64(r12342, 5ULL);
    Lib_IntVector_Intrinsics_vec256 r123453 =
      Lib_IntVector_Intrinsics_vec256_smul64(r12343, 5ULL);
    Lib_IntVector_Intrinsics_vec256 r123454 =
      Lib_IntVector_Intrinsics_vec256_smul64(r12344, 5ULL);
    Lib_IntVector_Intrinsics_vec256 a01 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12340, a0);
    Lib_IntVector_Intrinsics_vec256 a11 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12341, a0);
    Lib_IntVector_Intrinsics_vec256 a21 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12342, a0);
    Lib_IntVector_Intrinsics_vec256 a31 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12343, a0);
    Lib_IntVector_Intrinsics_vec256 a41 =
      Lib_IntVector_Intrinsics_vec256_mul64(r12344, a0);
    Lib_IntVector_Intrinsics_vec256 a02 = Lib_IntVector_Intrinsics_vec256_add64(
      a01, Lib_IntVector_Intrinsics_vec256_mul64(r123454, a1));
    Lib_IntVector_Intrinsics_vec256 a12 = Lib_IntVector_Intrinsics_vec256_add64(
      a11, Lib_IntVector_Intrinsics_vec256_mul64(r12340, a1));
    Lib_IntVector_Intrinsics_vec256 a22 = Lib_IntVector_Intrinsics_vec256_add64(
      a21, Lib_IntVector_Intrinsics_vec256_mul64(r12341, a1));
    Lib_IntVector_Intrinsics_vec256 a32 = Lib_IntVector_Intrinsics_vec256_add64(
      a31, Lib_IntVector_Intrinsics_vec256_mul64(r12342, a1));
    Lib_IntVector_Intrinsics_vec256 a42 = Lib_IntVector_Intrinsics_vec256_add64(
      a41, Lib_IntVector_Intrinsics_vec256_mul64(r12343, a1));
    Lib_IntVector_Intrinsics_vec256 a03 = Lib_IntVector_Intrinsics_vec256_add64(
      a02, Lib_IntVector_Intrinsics_vec256_mul64(r123453, a2));
    Lib_IntVector_Intrinsics_vec256 a13 = Lib_IntVector_Intrinsics_vec256_add64(
      a12, Lib_IntVector_Intrinsics_vec256_mul64(r123454, a2));
    Lib_IntVector_Intrinsics_vec256 a23 = Lib_IntVector_Intrinsics_vec256_add64(
      a22, Lib_IntVector_Intrinsics_vec256_mul64(r12340, a2));
    Lib_IntVector_Intrinsics_vec256 a33 = Lib_IntVector_Intrinsics_vec256_add64(
      a32, Lib_IntVector_Intrinsics_vec256_mul64(r12341, a2));
    Lib_IntVector_Intrinsics_vec256 a43 = Lib_IntVector_Intrinsics_vec256_add64(
      a42, Lib_IntVector_Intrinsics_vec256_mul64(r12342, a2));
    Lib_IntVector_Intrinsics_vec256 a04 = Lib_IntVector_Intrinsics_vec256_add64(
      a03, Lib_IntVector_Intrinsics_vec256_mul64(r123452, a3));
    Lib_IntVector_Intrinsics_vec256 a14 = Lib_IntVector_Intrinsics_vec256_add64(
      a13, Lib_IntVector_Intrinsics_vec256_mul64(r123453, a3));
    Lib_IntVector_Intrinsics_vec256 a24 = Lib_IntVector_Intrinsics_vec256_add64(
      a23, Lib_IntVector_Intrinsics_vec256_mul64(r123454, a3));
    Lib_IntVector_Intrinsics_vec256 a34 = Lib_IntVector_Intrinsics_vec256_add64(
      a33, Lib_IntVector_Intrinsics_vec256_mul64(r12340, a3));
    Lib_IntVector_Intrinsics_vec256 a44 = Lib_IntVector_Intrinsics_vec256_add64(
      a43, Lib_IntVector_Intrinsics_vec256_mul64(r12341, a3));
    Lib_IntVector_Intrinsics_vec256 a05 = Lib_IntVector_Intrinsics_vec256_add64(
      a04, Lib_IntVector_Intrinsics_vec256_mul64(r123451, a4));
    Lib_IntVector_Intrinsics_vec256 a15 = Lib_IntVector_Intrinsics_vec256_add64(
      a14, Lib_IntVector_Intrinsics_vec256_mul64(r123452, a4));
    Lib_IntVector_Intrinsics_vec256 a25 = Lib_IntVector_Intrinsics_vec256_add64(
      a24, Lib_IntVector_Intrinsics_vec256_mul64(r123453, a4));
    Lib_IntVector_Intrinsics_vec256 a35 = Lib_IntVector_Intrinsics_vec256_add64(
      a34, Lib_IntVector_Intrinsics_vec256_mul64(r123454, a4));
    Lib_IntVector_Intrinsics_vec256 a45 = Lib_IntVector_Intrinsics_vec256_add64(
      a44, Lib_IntVector_Intrinsics_vec256_mul64(r12340, a4));
    Lib_IntVector_Intrinsics_vec256 t0 = a05;
    Lib_IntVector_Intrinsics_vec256 t1 = a15;
    Lib_IntVector_Intrinsics_vec256 t2 = a25;
    Lib_IntVector_Intrinsics_vec256 t3 = a35;
    Lib_IntVector_Intrinsics_vec256 t4 = a45;
    Lib_IntVector_Intrinsics_vec256 mask26 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 z0 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t0, 26U);
    Lib_IntVector_Intrinsics_vec256 z1 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 26U);
    Lib_IntVector_Intrinsics_vec256 x0 =
      Lib_IntVector_Intrinsics_vec256_and(t0, mask26);
    Lib_IntVector_Intrinsics_vec256 x3 =
      Lib_IntVector_Intrinsics_vec256_and(t3, mask26);
    Lib_IntVector_Intrinsics_vec256 x1 =
      Lib_IntVector_Intrinsics_vec256_add64(t1, z0);
    Lib_IntVector_Intrinsics_vec256 x4 =
      Lib_IntVector_Intrinsics_vec256_add64(t4, z1);
    Lib_IntVector_Intrinsics_vec256 z01 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x1, 26U);
    Lib_IntVector_Intrinsics_vec256 z11 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x4, 26U);
    Lib_IntVector_Intrinsics_vec256 t =
      Lib_IntVector_Intrinsics_vec256_shift_left64(z11, 2U);
    Lib_IntVector_Intrinsics_vec256 z121 =
      Lib_IntVector_Intrinsics_vec256_add64(z11, t);
    Lib_IntVector_Intrinsics_vec256 x11 =
      Lib_IntVector_Intrinsics_vec256_and(x1, mask26);
    Lib_IntVector_Intrinsics_vec256 x41 =
      Lib_IntVector_Intrinsics_vec256_and(x4, mask26);
    Lib_IntVector_Intrinsics_vec256 x2 =
      Lib_IntVector_Intrinsics_vec256_add64(t2, z01);
    Lib_IntVector_Intrinsics_vec256 x01 =
      Lib_IntVector_Intrinsics_vec256_add64(x0, z121);
    Lib_IntVector_Intrinsics_vec256 z02 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x2, 26U);
    Lib_IntVector_Intrinsics_vec256 z13 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x01, 26U);
    Lib_IntVector_Intrinsics_vec256 x21 =
      Lib_IntVector_Intrinsics_vec256_and(x2, mask26);
    Lib_IntVector_Intrinsics_vec256 x02 =
      Lib_IntVector_Intrinsics_vec256_and(x01, mask26);
    Lib_IntVector_Intrinsics_vec256 x31 =
      Lib_IntVector_Intrinsics_vec256_add64(x3, z02);
    Lib_IntVector_Intrinsics_vec256 x12 =
      Lib_IntVector_Intrinsics_vec256_add64(x11, z13);
    Lib_IntVector_Intrinsics_vec256 z03 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x31, 26U);
    Lib_IntVector_Intrinsics_vec256 x32 =
      Lib_IntVector_Intrinsics_vec256_and(x31, mask26);
    Lib_IntVector_Intrinsics_vec256 x42 =
      Lib_IntVector_Intrinsics_vec256_add64(x41, z03);
    Lib_IntVector_Intrinsics_vec256 o0 = x02;
    Lib_IntVector_Intrinsics_vec256 o10 = x12;
    Lib_IntVector_Intrinsics_vec256 o20 = x21;
    Lib_IntVector_Intrinsics_vec256 o30 = x32;
    Lib_IntVector_Intrinsics_vec256 o40 = x42;
    Lib_IntVector_Intrinsics_vec256 v00 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(o0, o0);
    Lib_IntVector_Intrinsics_vec256 v10 =
      Lib_IntVector_Intrinsics_vec256_add64(o0, v00);
    Lib_IntVector_Intrinsics_vec256 v10h =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(v10, v10);
    Lib_IntVector_Intrinsics_vec256 v20 =
      Lib_IntVector_Intrinsics_vec256_add64(v10, v10h);
    Lib_IntVector_Intrinsics_vec256 v01 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(o10, o10);
    Lib_IntVector_Intrinsics_vec256 v11 =
      Lib_IntVector_Intrinsics_vec256_add64(o10, v01);
    Lib_IntVector_Intrinsics_vec256 v11h =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(v11, v11);
    Lib_IntVector_Intrinsics_vec256 v21 =
      Lib_IntVector_Intrinsics_vec256_add64(v11, v11h);
    Lib_IntVector_Intrinsics_vec256 v02 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(o20, o20);
    Lib_IntVector_Intrinsics_vec256 v12 =
      Lib_IntVector_Intrinsics_vec256_add64(o20, v02);
    Lib_IntVector_Intrinsics_vec256 v12h =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(v12, v12);
    Lib_IntVector_Intrinsics_vec256 v22 =
      Lib_IntVector_Intrinsics_vec256_add64(v12, v12h);
    Lib_IntVector_Intrinsics_vec256 v03 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(o30, o30);
    Lib_IntVector_Intrinsics_vec256 v13 =
      Lib_IntVector_Intrinsics_vec256_add64(o30, v03);
    Lib_IntVector_Intrinsics_vec256 v13h =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(v13, v13);
    Lib_IntVector_Intrinsics_vec256 v23 =
      Lib_IntVector_Intrinsics_vec256_add64(v13, v13h);
    Lib_IntVector_Intrinsics_vec256 v04 =
      Lib_IntVector_Intrinsics_vec256_interleave_high128(o40, o40);
    Lib_IntVector_Intrinsics_vec256 v14 =
      Lib_IntVector_Intrinsics_vec256_add64(o40, v04);
    Lib_IntVector_Intrinsics_vec256 v14h =
      Lib_IntVector_Intrinsics_vec256_interleave_high64(v14, v14);
    Lib_IntVector_Intrinsics_vec256 v24 =
      Lib_IntVector_Intrinsics_vec256_add64(v14, v14h);
    Lib_IntVector_Intrinsics_vec256 l = Lib_IntVector_Intrinsics_vec256_add64(
      v20, Lib_IntVector_Intrinsics_vec256_zero);
    Lib_IntVector_Intrinsics_vec256 tmp0 = Lib_IntVector_Intrinsics_vec256_and(
      l, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c0 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l, 26U);
    Lib_IntVector_Intrinsics_vec256 l0 =
      Lib_IntVector_Intrinsics_vec256_add64(v21, c0);
    Lib_IntVector_Intrinsics_vec256 tmp1 = Lib_IntVector_Intrinsics_vec256_and(
      l0, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c1 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l0, 26U);
    Lib_IntVector_Intrinsics_vec256 l1 =
      Lib_IntVector_Intrinsics_vec256_add64(v22, c1);
    Lib_IntVector_Intrinsics_vec256 tmp2 = Lib_IntVector_Intrinsics_vec256_and(
      l1, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c2 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l1, 26U);
    Lib_IntVector_Intrinsics_vec256 l2 =
      Lib_IntVector_Intrinsics_vec256_add64(v23, c2);
    Lib_IntVector_Intrinsics_vec256 tmp3 = Lib_IntVector_Intrinsics_vec256_and(
      l2, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c3 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l2, 26U);
    Lib_IntVector_Intrinsics_vec256 l3 =
      Lib_IntVector_Intrinsics_vec256_add64(v24, c3);
    Lib_IntVector_Intrinsics_vec256 tmp4 = Lib_IntVector_Intrinsics_vec256_and(
      l3, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c4 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l3, 26U);
    Lib_IntVector_Intrinsics_vec256 o00 = Lib_IntVector_Intrinsics_vec256_add64(
      tmp0, Lib_IntVector_Intrinsics_vec256_smul64(c4, 5ULL));
    Lib_IntVector_Intrinsics_vec256 o1 = tmp1;
    Lib_IntVector_Intrinsics_vec256 o2 = tmp2;
    Lib_IntVector_Intrinsics_vec256 o3 = tmp3;
    Lib_IntVector_Intrinsics_vec256 o4 = tmp4;
    out[0U] = o00;
    out[1U] = o1;
    out[2U] = o2;
    out[3U] = o3;
    out[4U] = o4;
}

void
Hacl_MAC_Poly1305_Simd256_poly1305_init(Lib_IntVector_Intrinsics_vec256* ctx,
                                        uint8_t* key)
{
    Lib_IntVector_Intrinsics_vec256* acc = ctx;
    Lib_IntVector_Intrinsics_vec256* pre = ctx + 5U;
    uint8_t* kr = key;
    acc[0U] = Lib_IntVector_Intrinsics_vec256_zero;
    acc[1U] = Lib_IntVector_Intrinsics_vec256_zero;
    acc[2U] = Lib_IntVector_Intrinsics_vec256_zero;
    acc[3U] = Lib_IntVector_Intrinsics_vec256_zero;
    acc[4U] = Lib_IntVector_Intrinsics_vec256_zero;
    uint64_t u0 = READ64LE(kr);
    uint64_t lo = u0;
    uint64_t u = READ64LE(kr + 8U);
    uint64_t hi = u;
    uint64_t mask0 = 0x0ffffffc0fffffffULL;
    uint64_t mask1 = 0x0ffffffc0ffffffcULL;
    uint64_t lo1 = lo & mask0;
    uint64_t hi1 = hi & mask1;
    Lib_IntVector_Intrinsics_vec256* r = pre;
    Lib_IntVector_Intrinsics_vec256* r5 = pre + 5U;
    Lib_IntVector_Intrinsics_vec256* rn = pre + 10U;
    Lib_IntVector_Intrinsics_vec256* rn_5 = pre + 15U;
    Lib_IntVector_Intrinsics_vec256 r_vec0 =
      Lib_IntVector_Intrinsics_vec256_load64(lo1);
    Lib_IntVector_Intrinsics_vec256 r_vec1 =
      Lib_IntVector_Intrinsics_vec256_load64(hi1);
    Lib_IntVector_Intrinsics_vec256 f00 = Lib_IntVector_Intrinsics_vec256_and(
      r_vec0, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 f15 = Lib_IntVector_Intrinsics_vec256_and(
      Lib_IntVector_Intrinsics_vec256_shift_right64(r_vec0, 26U),
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 f20 = Lib_IntVector_Intrinsics_vec256_or(
      Lib_IntVector_Intrinsics_vec256_shift_right64(r_vec0, 52U),
      Lib_IntVector_Intrinsics_vec256_shift_left64(
        Lib_IntVector_Intrinsics_vec256_and(
          r_vec1, Lib_IntVector_Intrinsics_vec256_load64(0x3fffULL)),
        12U));
    Lib_IntVector_Intrinsics_vec256 f30 = Lib_IntVector_Intrinsics_vec256_and(
      Lib_IntVector_Intrinsics_vec256_shift_right64(r_vec1, 14U),
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 f40 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(r_vec1, 40U);
    Lib_IntVector_Intrinsics_vec256 f0 = f00;
    Lib_IntVector_Intrinsics_vec256 f1 = f15;
    Lib_IntVector_Intrinsics_vec256 f2 = f20;
    Lib_IntVector_Intrinsics_vec256 f3 = f30;
    Lib_IntVector_Intrinsics_vec256 f4 = f40;
    r[0U] = f0;
    r[1U] = f1;
    r[2U] = f2;
    r[3U] = f3;
    r[4U] = f4;
    Lib_IntVector_Intrinsics_vec256 f200 = r[0U];
    Lib_IntVector_Intrinsics_vec256 f210 = r[1U];
    Lib_IntVector_Intrinsics_vec256 f220 = r[2U];
    Lib_IntVector_Intrinsics_vec256 f230 = r[3U];
    Lib_IntVector_Intrinsics_vec256 f240 = r[4U];
    r5[0U] = Lib_IntVector_Intrinsics_vec256_smul64(f200, 5ULL);
    r5[1U] = Lib_IntVector_Intrinsics_vec256_smul64(f210, 5ULL);
    r5[2U] = Lib_IntVector_Intrinsics_vec256_smul64(f220, 5ULL);
    r5[3U] = Lib_IntVector_Intrinsics_vec256_smul64(f230, 5ULL);
    r5[4U] = Lib_IntVector_Intrinsics_vec256_smul64(f240, 5ULL);
    Lib_IntVector_Intrinsics_vec256 r0 = r[0U];
    Lib_IntVector_Intrinsics_vec256 r10 = r[1U];
    Lib_IntVector_Intrinsics_vec256 r20 = r[2U];
    Lib_IntVector_Intrinsics_vec256 r30 = r[3U];
    Lib_IntVector_Intrinsics_vec256 r40 = r[4U];
    Lib_IntVector_Intrinsics_vec256 r510 = r5[1U];
    Lib_IntVector_Intrinsics_vec256 r520 = r5[2U];
    Lib_IntVector_Intrinsics_vec256 r530 = r5[3U];
    Lib_IntVector_Intrinsics_vec256 r540 = r5[4U];
    Lib_IntVector_Intrinsics_vec256 f100 = r[0U];
    Lib_IntVector_Intrinsics_vec256 f110 = r[1U];
    Lib_IntVector_Intrinsics_vec256 f120 = r[2U];
    Lib_IntVector_Intrinsics_vec256 f130 = r[3U];
    Lib_IntVector_Intrinsics_vec256 f140 = r[4U];
    Lib_IntVector_Intrinsics_vec256 a00 =
      Lib_IntVector_Intrinsics_vec256_mul64(r0, f100);
    Lib_IntVector_Intrinsics_vec256 a10 =
      Lib_IntVector_Intrinsics_vec256_mul64(r10, f100);
    Lib_IntVector_Intrinsics_vec256 a20 =
      Lib_IntVector_Intrinsics_vec256_mul64(r20, f100);
    Lib_IntVector_Intrinsics_vec256 a30 =
      Lib_IntVector_Intrinsics_vec256_mul64(r30, f100);
    Lib_IntVector_Intrinsics_vec256 a40 =
      Lib_IntVector_Intrinsics_vec256_mul64(r40, f100);
    Lib_IntVector_Intrinsics_vec256 a010 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a00, Lib_IntVector_Intrinsics_vec256_mul64(r540, f110));
    Lib_IntVector_Intrinsics_vec256 a110 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a10, Lib_IntVector_Intrinsics_vec256_mul64(r0, f110));
    Lib_IntVector_Intrinsics_vec256 a210 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a20, Lib_IntVector_Intrinsics_vec256_mul64(r10, f110));
    Lib_IntVector_Intrinsics_vec256 a310 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a30, Lib_IntVector_Intrinsics_vec256_mul64(r20, f110));
    Lib_IntVector_Intrinsics_vec256 a410 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a40, Lib_IntVector_Intrinsics_vec256_mul64(r30, f110));
    Lib_IntVector_Intrinsics_vec256 a020 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a010, Lib_IntVector_Intrinsics_vec256_mul64(r530, f120));
    Lib_IntVector_Intrinsics_vec256 a120 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a110, Lib_IntVector_Intrinsics_vec256_mul64(r540, f120));
    Lib_IntVector_Intrinsics_vec256 a220 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a210, Lib_IntVector_Intrinsics_vec256_mul64(r0, f120));
    Lib_IntVector_Intrinsics_vec256 a320 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a310, Lib_IntVector_Intrinsics_vec256_mul64(r10, f120));
    Lib_IntVector_Intrinsics_vec256 a420 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a410, Lib_IntVector_Intrinsics_vec256_mul64(r20, f120));
    Lib_IntVector_Intrinsics_vec256 a030 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a020, Lib_IntVector_Intrinsics_vec256_mul64(r520, f130));
    Lib_IntVector_Intrinsics_vec256 a130 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a120, Lib_IntVector_Intrinsics_vec256_mul64(r530, f130));
    Lib_IntVector_Intrinsics_vec256 a230 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a220, Lib_IntVector_Intrinsics_vec256_mul64(r540, f130));
    Lib_IntVector_Intrinsics_vec256 a330 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a320, Lib_IntVector_Intrinsics_vec256_mul64(r0, f130));
    Lib_IntVector_Intrinsics_vec256 a430 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a420, Lib_IntVector_Intrinsics_vec256_mul64(r10, f130));
    Lib_IntVector_Intrinsics_vec256 a040 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a030, Lib_IntVector_Intrinsics_vec256_mul64(r510, f140));
    Lib_IntVector_Intrinsics_vec256 a140 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a130, Lib_IntVector_Intrinsics_vec256_mul64(r520, f140));
    Lib_IntVector_Intrinsics_vec256 a240 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a230, Lib_IntVector_Intrinsics_vec256_mul64(r530, f140));
    Lib_IntVector_Intrinsics_vec256 a340 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a330, Lib_IntVector_Intrinsics_vec256_mul64(r540, f140));
    Lib_IntVector_Intrinsics_vec256 a440 =
      Lib_IntVector_Intrinsics_vec256_add64(
        a430, Lib_IntVector_Intrinsics_vec256_mul64(r0, f140));
    Lib_IntVector_Intrinsics_vec256 t00 = a040;
    Lib_IntVector_Intrinsics_vec256 t10 = a140;
    Lib_IntVector_Intrinsics_vec256 t20 = a240;
    Lib_IntVector_Intrinsics_vec256 t30 = a340;
    Lib_IntVector_Intrinsics_vec256 t40 = a440;
    Lib_IntVector_Intrinsics_vec256 mask260 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 z00 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t00, 26U);
    Lib_IntVector_Intrinsics_vec256 z10 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t30, 26U);
    Lib_IntVector_Intrinsics_vec256 x00 =
      Lib_IntVector_Intrinsics_vec256_and(t00, mask260);
    Lib_IntVector_Intrinsics_vec256 x30 =
      Lib_IntVector_Intrinsics_vec256_and(t30, mask260);
    Lib_IntVector_Intrinsics_vec256 x10 =
      Lib_IntVector_Intrinsics_vec256_add64(t10, z00);
    Lib_IntVector_Intrinsics_vec256 x40 =
      Lib_IntVector_Intrinsics_vec256_add64(t40, z10);
    Lib_IntVector_Intrinsics_vec256 z010 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x10, 26U);
    Lib_IntVector_Intrinsics_vec256 z110 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x40, 26U);
    Lib_IntVector_Intrinsics_vec256 t5 =
      Lib_IntVector_Intrinsics_vec256_shift_left64(z110, 2U);
    Lib_IntVector_Intrinsics_vec256 z12 =
      Lib_IntVector_Intrinsics_vec256_add64(z110, t5);
    Lib_IntVector_Intrinsics_vec256 x110 =
      Lib_IntVector_Intrinsics_vec256_and(x10, mask260);
    Lib_IntVector_Intrinsics_vec256 x410 =
      Lib_IntVector_Intrinsics_vec256_and(x40, mask260);
    Lib_IntVector_Intrinsics_vec256 x20 =
      Lib_IntVector_Intrinsics_vec256_add64(t20, z010);
    Lib_IntVector_Intrinsics_vec256 x010 =
      Lib_IntVector_Intrinsics_vec256_add64(x00, z12);
    Lib_IntVector_Intrinsics_vec256 z020 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x20, 26U);
    Lib_IntVector_Intrinsics_vec256 z130 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x010, 26U);
    Lib_IntVector_Intrinsics_vec256 x210 =
      Lib_IntVector_Intrinsics_vec256_and(x20, mask260);
    Lib_IntVector_Intrinsics_vec256 x020 =
      Lib_IntVector_Intrinsics_vec256_and(x010, mask260);
    Lib_IntVector_Intrinsics_vec256 x310 =
      Lib_IntVector_Intrinsics_vec256_add64(x30, z020);
    Lib_IntVector_Intrinsics_vec256 x120 =
      Lib_IntVector_Intrinsics_vec256_add64(x110, z130);
    Lib_IntVector_Intrinsics_vec256 z030 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x310, 26U);
    Lib_IntVector_Intrinsics_vec256 x320 =
      Lib_IntVector_Intrinsics_vec256_and(x310, mask260);
    Lib_IntVector_Intrinsics_vec256 x420 =
      Lib_IntVector_Intrinsics_vec256_add64(x410, z030);
    Lib_IntVector_Intrinsics_vec256 o00 = x020;
    Lib_IntVector_Intrinsics_vec256 o10 = x120;
    Lib_IntVector_Intrinsics_vec256 o20 = x210;
    Lib_IntVector_Intrinsics_vec256 o30 = x320;
    Lib_IntVector_Intrinsics_vec256 o40 = x420;
    rn[0U] = o00;
    rn[1U] = o10;
    rn[2U] = o20;
    rn[3U] = o30;
    rn[4U] = o40;
    Lib_IntVector_Intrinsics_vec256 f201 = rn[0U];
    Lib_IntVector_Intrinsics_vec256 f211 = rn[1U];
    Lib_IntVector_Intrinsics_vec256 f221 = rn[2U];
    Lib_IntVector_Intrinsics_vec256 f231 = rn[3U];
    Lib_IntVector_Intrinsics_vec256 f241 = rn[4U];
    rn_5[0U] = Lib_IntVector_Intrinsics_vec256_smul64(f201, 5ULL);
    rn_5[1U] = Lib_IntVector_Intrinsics_vec256_smul64(f211, 5ULL);
    rn_5[2U] = Lib_IntVector_Intrinsics_vec256_smul64(f221, 5ULL);
    rn_5[3U] = Lib_IntVector_Intrinsics_vec256_smul64(f231, 5ULL);
    rn_5[4U] = Lib_IntVector_Intrinsics_vec256_smul64(f241, 5ULL);
    Lib_IntVector_Intrinsics_vec256 r00 = rn[0U];
    Lib_IntVector_Intrinsics_vec256 r1 = rn[1U];
    Lib_IntVector_Intrinsics_vec256 r2 = rn[2U];
    Lib_IntVector_Intrinsics_vec256 r3 = rn[3U];
    Lib_IntVector_Intrinsics_vec256 r4 = rn[4U];
    Lib_IntVector_Intrinsics_vec256 r51 = rn_5[1U];
    Lib_IntVector_Intrinsics_vec256 r52 = rn_5[2U];
    Lib_IntVector_Intrinsics_vec256 r53 = rn_5[3U];
    Lib_IntVector_Intrinsics_vec256 r54 = rn_5[4U];
    Lib_IntVector_Intrinsics_vec256 f10 = rn[0U];
    Lib_IntVector_Intrinsics_vec256 f11 = rn[1U];
    Lib_IntVector_Intrinsics_vec256 f12 = rn[2U];
    Lib_IntVector_Intrinsics_vec256 f13 = rn[3U];
    Lib_IntVector_Intrinsics_vec256 f14 = rn[4U];
    Lib_IntVector_Intrinsics_vec256 a0 =
      Lib_IntVector_Intrinsics_vec256_mul64(r00, f10);
    Lib_IntVector_Intrinsics_vec256 a1 =
      Lib_IntVector_Intrinsics_vec256_mul64(r1, f10);
    Lib_IntVector_Intrinsics_vec256 a2 =
      Lib_IntVector_Intrinsics_vec256_mul64(r2, f10);
    Lib_IntVector_Intrinsics_vec256 a3 =
      Lib_IntVector_Intrinsics_vec256_mul64(r3, f10);
    Lib_IntVector_Intrinsics_vec256 a4 =
      Lib_IntVector_Intrinsics_vec256_mul64(r4, f10);
    Lib_IntVector_Intrinsics_vec256 a01 = Lib_IntVector_Intrinsics_vec256_add64(
      a0, Lib_IntVector_Intrinsics_vec256_mul64(r54, f11));
    Lib_IntVector_Intrinsics_vec256 a11 = Lib_IntVector_Intrinsics_vec256_add64(
      a1, Lib_IntVector_Intrinsics_vec256_mul64(r00, f11));
    Lib_IntVector_Intrinsics_vec256 a21 = Lib_IntVector_Intrinsics_vec256_add64(
      a2, Lib_IntVector_Intrinsics_vec256_mul64(r1, f11));
    Lib_IntVector_Intrinsics_vec256 a31 = Lib_IntVector_Intrinsics_vec256_add64(
      a3, Lib_IntVector_Intrinsics_vec256_mul64(r2, f11));
    Lib_IntVector_Intrinsics_vec256 a41 = Lib_IntVector_Intrinsics_vec256_add64(
      a4, Lib_IntVector_Intrinsics_vec256_mul64(r3, f11));
    Lib_IntVector_Intrinsics_vec256 a02 = Lib_IntVector_Intrinsics_vec256_add64(
      a01, Lib_IntVector_Intrinsics_vec256_mul64(r53, f12));
    Lib_IntVector_Intrinsics_vec256 a12 = Lib_IntVector_Intrinsics_vec256_add64(
      a11, Lib_IntVector_Intrinsics_vec256_mul64(r54, f12));
    Lib_IntVector_Intrinsics_vec256 a22 = Lib_IntVector_Intrinsics_vec256_add64(
      a21, Lib_IntVector_Intrinsics_vec256_mul64(r00, f12));
    Lib_IntVector_Intrinsics_vec256 a32 = Lib_IntVector_Intrinsics_vec256_add64(
      a31, Lib_IntVector_Intrinsics_vec256_mul64(r1, f12));
    Lib_IntVector_Intrinsics_vec256 a42 = Lib_IntVector_Intrinsics_vec256_add64(
      a41, Lib_IntVector_Intrinsics_vec256_mul64(r2, f12));
    Lib_IntVector_Intrinsics_vec256 a03 = Lib_IntVector_Intrinsics_vec256_add64(
      a02, Lib_IntVector_Intrinsics_vec256_mul64(r52, f13));
    Lib_IntVector_Intrinsics_vec256 a13 = Lib_IntVector_Intrinsics_vec256_add64(
      a12, Lib_IntVector_Intrinsics_vec256_mul64(r53, f13));
    Lib_IntVector_Intrinsics_vec256 a23 = Lib_IntVector_Intrinsics_vec256_add64(
      a22, Lib_IntVector_Intrinsics_vec256_mul64(r54, f13));
    Lib_IntVector_Intrinsics_vec256 a33 = Lib_IntVector_Intrinsics_vec256_add64(
      a32, Lib_IntVector_Intrinsics_vec256_mul64(r00, f13));
    Lib_IntVector_Intrinsics_vec256 a43 = Lib_IntVector_Intrinsics_vec256_add64(
      a42, Lib_IntVector_Intrinsics_vec256_mul64(r1, f13));
    Lib_IntVector_Intrinsics_vec256 a04 = Lib_IntVector_Intrinsics_vec256_add64(
      a03, Lib_IntVector_Intrinsics_vec256_mul64(r51, f14));
    Lib_IntVector_Intrinsics_vec256 a14 = Lib_IntVector_Intrinsics_vec256_add64(
      a13, Lib_IntVector_Intrinsics_vec256_mul64(r52, f14));
    Lib_IntVector_Intrinsics_vec256 a24 = Lib_IntVector_Intrinsics_vec256_add64(
      a23, Lib_IntVector_Intrinsics_vec256_mul64(r53, f14));
    Lib_IntVector_Intrinsics_vec256 a34 = Lib_IntVector_Intrinsics_vec256_add64(
      a33, Lib_IntVector_Intrinsics_vec256_mul64(r54, f14));
    Lib_IntVector_Intrinsics_vec256 a44 = Lib_IntVector_Intrinsics_vec256_add64(
      a43, Lib_IntVector_Intrinsics_vec256_mul64(r00, f14));
    Lib_IntVector_Intrinsics_vec256 t0 = a04;
    Lib_IntVector_Intrinsics_vec256 t1 = a14;
    Lib_IntVector_Intrinsics_vec256 t2 = a24;
    Lib_IntVector_Intrinsics_vec256 t3 = a34;
    Lib_IntVector_Intrinsics_vec256 t4 = a44;
    Lib_IntVector_Intrinsics_vec256 mask26 =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 z0 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t0, 26U);
    Lib_IntVector_Intrinsics_vec256 z1 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 26U);
    Lib_IntVector_Intrinsics_vec256 x0 =
      Lib_IntVector_Intrinsics_vec256_and(t0, mask26);
    Lib_IntVector_Intrinsics_vec256 x3 =
      Lib_IntVector_Intrinsics_vec256_and(t3, mask26);
    Lib_IntVector_Intrinsics_vec256 x1 =
      Lib_IntVector_Intrinsics_vec256_add64(t1, z0);
    Lib_IntVector_Intrinsics_vec256 x4 =
      Lib_IntVector_Intrinsics_vec256_add64(t4, z1);
    Lib_IntVector_Intrinsics_vec256 z01 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x1, 26U);
    Lib_IntVector_Intrinsics_vec256 z11 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x4, 26U);
    Lib_IntVector_Intrinsics_vec256 t =
      Lib_IntVector_Intrinsics_vec256_shift_left64(z11, 2U);
    Lib_IntVector_Intrinsics_vec256 z120 =
      Lib_IntVector_Intrinsics_vec256_add64(z11, t);
    Lib_IntVector_Intrinsics_vec256 x11 =
      Lib_IntVector_Intrinsics_vec256_and(x1, mask26);
    Lib_IntVector_Intrinsics_vec256 x41 =
      Lib_IntVector_Intrinsics_vec256_and(x4, mask26);
    Lib_IntVector_Intrinsics_vec256 x2 =
      Lib_IntVector_Intrinsics_vec256_add64(t2, z01);
    Lib_IntVector_Intrinsics_vec256 x01 =
      Lib_IntVector_Intrinsics_vec256_add64(x0, z120);
    Lib_IntVector_Intrinsics_vec256 z02 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x2, 26U);
    Lib_IntVector_Intrinsics_vec256 z13 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x01, 26U);
    Lib_IntVector_Intrinsics_vec256 x21 =
      Lib_IntVector_Intrinsics_vec256_and(x2, mask26);
    Lib_IntVector_Intrinsics_vec256 x02 =
      Lib_IntVector_Intrinsics_vec256_and(x01, mask26);
    Lib_IntVector_Intrinsics_vec256 x31 =
      Lib_IntVector_Intrinsics_vec256_add64(x3, z02);
    Lib_IntVector_Intrinsics_vec256 x12 =
      Lib_IntVector_Intrinsics_vec256_add64(x11, z13);
    Lib_IntVector_Intrinsics_vec256 z03 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(x31, 26U);
    Lib_IntVector_Intrinsics_vec256 x32 =
      Lib_IntVector_Intrinsics_vec256_and(x31, mask26);
    Lib_IntVector_Intrinsics_vec256 x42 =
      Lib_IntVector_Intrinsics_vec256_add64(x41, z03);
    Lib_IntVector_Intrinsics_vec256 o0 = x02;
    Lib_IntVector_Intrinsics_vec256 o1 = x12;
    Lib_IntVector_Intrinsics_vec256 o2 = x21;
    Lib_IntVector_Intrinsics_vec256 o3 = x32;
    Lib_IntVector_Intrinsics_vec256 o4 = x42;
    rn[0U] = o0;
    rn[1U] = o1;
    rn[2U] = o2;
    rn[3U] = o3;
    rn[4U] = o4;
    Lib_IntVector_Intrinsics_vec256 f202 = rn[0U];
    Lib_IntVector_Intrinsics_vec256 f21 = rn[1U];
    Lib_IntVector_Intrinsics_vec256 f22 = rn[2U];
    Lib_IntVector_Intrinsics_vec256 f23 = rn[3U];
    Lib_IntVector_Intrinsics_vec256 f24 = rn[4U];
    rn_5[0U] = Lib_IntVector_Intrinsics_vec256_smul64(f202, 5ULL);
    rn_5[1U] = Lib_IntVector_Intrinsics_vec256_smul64(f21, 5ULL);
    rn_5[2U] = Lib_IntVector_Intrinsics_vec256_smul64(f22, 5ULL);
    rn_5[3U] = Lib_IntVector_Intrinsics_vec256_smul64(f23, 5ULL);
    rn_5[4U] = Lib_IntVector_Intrinsics_vec256_smul64(f24, 5ULL);
}

static void
poly1305_update(Lib_IntVector_Intrinsics_vec256* ctx,
                uint32_t len,
                uint8_t* text)
{
    Lib_IntVector_Intrinsics_vec256* pre = ctx + 5U;
    Lib_IntVector_Intrinsics_vec256* acc = ctx;
    uint32_t sz_block = 64U;
    uint32_t len0 = len / sz_block * sz_block;
    uint8_t* t0 = text;
    if (len0 > 0U) {
        uint32_t bs = 64U;
        uint8_t* text0 = t0;
        Hacl_MAC_Poly1305_Simd256_load_acc4(acc, text0);
        uint32_t len1 = len0 - bs;
        uint8_t* text1 = t0 + bs;
        uint32_t nb = len1 / bs;
        for (uint32_t i = 0U; i < nb; i++) {
            uint8_t* block = text1 + i * bs;
            KRML_PRE_ALIGN(32)
            Lib_IntVector_Intrinsics_vec256 e[5U] KRML_POST_ALIGN(32) = { 0U };
            Lib_IntVector_Intrinsics_vec256 lo =
              Lib_IntVector_Intrinsics_vec256_load64_le(block);
            Lib_IntVector_Intrinsics_vec256 hi =
              Lib_IntVector_Intrinsics_vec256_load64_le(block + 32U);
            Lib_IntVector_Intrinsics_vec256 mask260 =
              Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
            Lib_IntVector_Intrinsics_vec256 m0 =
              Lib_IntVector_Intrinsics_vec256_interleave_low128(lo, hi);
            Lib_IntVector_Intrinsics_vec256 m1 =
              Lib_IntVector_Intrinsics_vec256_interleave_high128(lo, hi);
            Lib_IntVector_Intrinsics_vec256 m2 =
              Lib_IntVector_Intrinsics_vec256_shift_right(m0, 48U);
            Lib_IntVector_Intrinsics_vec256 m3 =
              Lib_IntVector_Intrinsics_vec256_shift_right(m1, 48U);
            Lib_IntVector_Intrinsics_vec256 m4 =
              Lib_IntVector_Intrinsics_vec256_interleave_high64(m0, m1);
            Lib_IntVector_Intrinsics_vec256 t010 =
              Lib_IntVector_Intrinsics_vec256_interleave_low64(m0, m1);
            Lib_IntVector_Intrinsics_vec256 t30 =
              Lib_IntVector_Intrinsics_vec256_interleave_low64(m2, m3);
            Lib_IntVector_Intrinsics_vec256 t20 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(t30, 4U);
            Lib_IntVector_Intrinsics_vec256 o20 =
              Lib_IntVector_Intrinsics_vec256_and(t20, mask260);
            Lib_IntVector_Intrinsics_vec256 t10 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(t010, 26U);
            Lib_IntVector_Intrinsics_vec256 o10 =
              Lib_IntVector_Intrinsics_vec256_and(t10, mask260);
            Lib_IntVector_Intrinsics_vec256 o5 =
              Lib_IntVector_Intrinsics_vec256_and(t010, mask260);
            Lib_IntVector_Intrinsics_vec256 t31 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(t30, 30U);
            Lib_IntVector_Intrinsics_vec256 o30 =
              Lib_IntVector_Intrinsics_vec256_and(t31, mask260);
            Lib_IntVector_Intrinsics_vec256 o40 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(m4, 40U);
            Lib_IntVector_Intrinsics_vec256 o00 = o5;
            Lib_IntVector_Intrinsics_vec256 o11 = o10;
            Lib_IntVector_Intrinsics_vec256 o21 = o20;
            Lib_IntVector_Intrinsics_vec256 o31 = o30;
            Lib_IntVector_Intrinsics_vec256 o41 = o40;
            e[0U] = o00;
            e[1U] = o11;
            e[2U] = o21;
            e[3U] = o31;
            e[4U] = o41;
            uint64_t b = 0x1000000ULL;
            Lib_IntVector_Intrinsics_vec256 mask =
              Lib_IntVector_Intrinsics_vec256_load64(b);
            Lib_IntVector_Intrinsics_vec256 f4 = e[4U];
            e[4U] = Lib_IntVector_Intrinsics_vec256_or(f4, mask);
            Lib_IntVector_Intrinsics_vec256* rn = pre + 10U;
            Lib_IntVector_Intrinsics_vec256* rn5 = pre + 15U;
            Lib_IntVector_Intrinsics_vec256 r0 = rn[0U];
            Lib_IntVector_Intrinsics_vec256 r1 = rn[1U];
            Lib_IntVector_Intrinsics_vec256 r2 = rn[2U];
            Lib_IntVector_Intrinsics_vec256 r3 = rn[3U];
            Lib_IntVector_Intrinsics_vec256 r4 = rn[4U];
            Lib_IntVector_Intrinsics_vec256 r51 = rn5[1U];
            Lib_IntVector_Intrinsics_vec256 r52 = rn5[2U];
            Lib_IntVector_Intrinsics_vec256 r53 = rn5[3U];
            Lib_IntVector_Intrinsics_vec256 r54 = rn5[4U];
            Lib_IntVector_Intrinsics_vec256 f10 = acc[0U];
            Lib_IntVector_Intrinsics_vec256 f110 = acc[1U];
            Lib_IntVector_Intrinsics_vec256 f120 = acc[2U];
            Lib_IntVector_Intrinsics_vec256 f130 = acc[3U];
            Lib_IntVector_Intrinsics_vec256 f140 = acc[4U];
            Lib_IntVector_Intrinsics_vec256 a0 =
              Lib_IntVector_Intrinsics_vec256_mul64(r0, f10);
            Lib_IntVector_Intrinsics_vec256 a1 =
              Lib_IntVector_Intrinsics_vec256_mul64(r1, f10);
            Lib_IntVector_Intrinsics_vec256 a2 =
              Lib_IntVector_Intrinsics_vec256_mul64(r2, f10);
            Lib_IntVector_Intrinsics_vec256 a3 =
              Lib_IntVector_Intrinsics_vec256_mul64(r3, f10);
            Lib_IntVector_Intrinsics_vec256 a4 =
              Lib_IntVector_Intrinsics_vec256_mul64(r4, f10);
            Lib_IntVector_Intrinsics_vec256 a01 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a0, Lib_IntVector_Intrinsics_vec256_mul64(r54, f110));
            Lib_IntVector_Intrinsics_vec256 a11 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a1, Lib_IntVector_Intrinsics_vec256_mul64(r0, f110));
            Lib_IntVector_Intrinsics_vec256 a21 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a2, Lib_IntVector_Intrinsics_vec256_mul64(r1, f110));
            Lib_IntVector_Intrinsics_vec256 a31 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a3, Lib_IntVector_Intrinsics_vec256_mul64(r2, f110));
            Lib_IntVector_Intrinsics_vec256 a41 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a4, Lib_IntVector_Intrinsics_vec256_mul64(r3, f110));
            Lib_IntVector_Intrinsics_vec256 a02 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a01, Lib_IntVector_Intrinsics_vec256_mul64(r53, f120));
            Lib_IntVector_Intrinsics_vec256 a12 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a11, Lib_IntVector_Intrinsics_vec256_mul64(r54, f120));
            Lib_IntVector_Intrinsics_vec256 a22 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a21, Lib_IntVector_Intrinsics_vec256_mul64(r0, f120));
            Lib_IntVector_Intrinsics_vec256 a32 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a31, Lib_IntVector_Intrinsics_vec256_mul64(r1, f120));
            Lib_IntVector_Intrinsics_vec256 a42 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a41, Lib_IntVector_Intrinsics_vec256_mul64(r2, f120));
            Lib_IntVector_Intrinsics_vec256 a03 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a02, Lib_IntVector_Intrinsics_vec256_mul64(r52, f130));
            Lib_IntVector_Intrinsics_vec256 a13 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a12, Lib_IntVector_Intrinsics_vec256_mul64(r53, f130));
            Lib_IntVector_Intrinsics_vec256 a23 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a22, Lib_IntVector_Intrinsics_vec256_mul64(r54, f130));
            Lib_IntVector_Intrinsics_vec256 a33 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a32, Lib_IntVector_Intrinsics_vec256_mul64(r0, f130));
            Lib_IntVector_Intrinsics_vec256 a43 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a42, Lib_IntVector_Intrinsics_vec256_mul64(r1, f130));
            Lib_IntVector_Intrinsics_vec256 a04 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a03, Lib_IntVector_Intrinsics_vec256_mul64(r51, f140));
            Lib_IntVector_Intrinsics_vec256 a14 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a13, Lib_IntVector_Intrinsics_vec256_mul64(r52, f140));
            Lib_IntVector_Intrinsics_vec256 a24 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a23, Lib_IntVector_Intrinsics_vec256_mul64(r53, f140));
            Lib_IntVector_Intrinsics_vec256 a34 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a33, Lib_IntVector_Intrinsics_vec256_mul64(r54, f140));
            Lib_IntVector_Intrinsics_vec256 a44 =
              Lib_IntVector_Intrinsics_vec256_add64(
                a43, Lib_IntVector_Intrinsics_vec256_mul64(r0, f140));
            Lib_IntVector_Intrinsics_vec256 t01 = a04;
            Lib_IntVector_Intrinsics_vec256 t1 = a14;
            Lib_IntVector_Intrinsics_vec256 t2 = a24;
            Lib_IntVector_Intrinsics_vec256 t3 = a34;
            Lib_IntVector_Intrinsics_vec256 t4 = a44;
            Lib_IntVector_Intrinsics_vec256 mask26 =
              Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
            Lib_IntVector_Intrinsics_vec256 z0 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(t01, 26U);
            Lib_IntVector_Intrinsics_vec256 z1 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 26U);
            Lib_IntVector_Intrinsics_vec256 x0 =
              Lib_IntVector_Intrinsics_vec256_and(t01, mask26);
            Lib_IntVector_Intrinsics_vec256 x3 =
              Lib_IntVector_Intrinsics_vec256_and(t3, mask26);
            Lib_IntVector_Intrinsics_vec256 x1 =
              Lib_IntVector_Intrinsics_vec256_add64(t1, z0);
            Lib_IntVector_Intrinsics_vec256 x4 =
              Lib_IntVector_Intrinsics_vec256_add64(t4, z1);
            Lib_IntVector_Intrinsics_vec256 z01 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(x1, 26U);
            Lib_IntVector_Intrinsics_vec256 z11 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(x4, 26U);
            Lib_IntVector_Intrinsics_vec256 t =
              Lib_IntVector_Intrinsics_vec256_shift_left64(z11, 2U);
            Lib_IntVector_Intrinsics_vec256 z12 =
              Lib_IntVector_Intrinsics_vec256_add64(z11, t);
            Lib_IntVector_Intrinsics_vec256 x11 =
              Lib_IntVector_Intrinsics_vec256_and(x1, mask26);
            Lib_IntVector_Intrinsics_vec256 x41 =
              Lib_IntVector_Intrinsics_vec256_and(x4, mask26);
            Lib_IntVector_Intrinsics_vec256 x2 =
              Lib_IntVector_Intrinsics_vec256_add64(t2, z01);
            Lib_IntVector_Intrinsics_vec256 x01 =
              Lib_IntVector_Intrinsics_vec256_add64(x0, z12);
            Lib_IntVector_Intrinsics_vec256 z02 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(x2, 26U);
            Lib_IntVector_Intrinsics_vec256 z13 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(x01, 26U);
            Lib_IntVector_Intrinsics_vec256 x21 =
              Lib_IntVector_Intrinsics_vec256_and(x2, mask26);
            Lib_IntVector_Intrinsics_vec256 x02 =
              Lib_IntVector_Intrinsics_vec256_and(x01, mask26);
            Lib_IntVector_Intrinsics_vec256 x31 =
              Lib_IntVector_Intrinsics_vec256_add64(x3, z02);
            Lib_IntVector_Intrinsics_vec256 x12 =
              Lib_IntVector_Intrinsics_vec256_add64(x11, z13);
            Lib_IntVector_Intrinsics_vec256 z03 =
              Lib_IntVector_Intrinsics_vec256_shift_right64(x31, 26U);
            Lib_IntVector_Intrinsics_vec256 x32 =
              Lib_IntVector_Intrinsics_vec256_and(x31, mask26);
            Lib_IntVector_Intrinsics_vec256 x42 =
              Lib_IntVector_Intrinsics_vec256_add64(x41, z03);
            Lib_IntVector_Intrinsics_vec256 o01 = x02;
            Lib_IntVector_Intrinsics_vec256 o12 = x12;
            Lib_IntVector_Intrinsics_vec256 o22 = x21;
            Lib_IntVector_Intrinsics_vec256 o32 = x32;
            Lib_IntVector_Intrinsics_vec256 o42 = x42;
            acc[0U] = o01;
            acc[1U] = o12;
            acc[2U] = o22;
            acc[3U] = o32;
            acc[4U] = o42;
            Lib_IntVector_Intrinsics_vec256 f100 = acc[0U];
            Lib_IntVector_Intrinsics_vec256 f11 = acc[1U];
            Lib_IntVector_Intrinsics_vec256 f12 = acc[2U];
            Lib_IntVector_Intrinsics_vec256 f13 = acc[3U];
            Lib_IntVector_Intrinsics_vec256 f14 = acc[4U];
            Lib_IntVector_Intrinsics_vec256 f20 = e[0U];
            Lib_IntVector_Intrinsics_vec256 f21 = e[1U];
            Lib_IntVector_Intrinsics_vec256 f22 = e[2U];
            Lib_IntVector_Intrinsics_vec256 f23 = e[3U];
            Lib_IntVector_Intrinsics_vec256 f24 = e[4U];
            Lib_IntVector_Intrinsics_vec256 o0 =
              Lib_IntVector_Intrinsics_vec256_add64(f100, f20);
            Lib_IntVector_Intrinsics_vec256 o1 =
              Lib_IntVector_Intrinsics_vec256_add64(f11, f21);
            Lib_IntVector_Intrinsics_vec256 o2 =
              Lib_IntVector_Intrinsics_vec256_add64(f12, f22);
            Lib_IntVector_Intrinsics_vec256 o3 =
              Lib_IntVector_Intrinsics_vec256_add64(f13, f23);
            Lib_IntVector_Intrinsics_vec256 o4 =
              Lib_IntVector_Intrinsics_vec256_add64(f14, f24);
            acc[0U] = o0;
            acc[1U] = o1;
            acc[2U] = o2;
            acc[3U] = o3;
            acc[4U] = o4;
        }
        Hacl_MAC_Poly1305_Simd256_fmul_r4_normalize(acc, pre);
    }
    uint32_t len1 = len - len0;
    uint8_t* t1 = text + len0;
    uint32_t nb = len1 / 16U;
    uint32_t rem = len1 % 16U;
    for (uint32_t i = 0U; i < nb; i++) {
        uint8_t* block = t1 + i * 16U;
        KRML_PRE_ALIGN(32)
        Lib_IntVector_Intrinsics_vec256 e[5U] KRML_POST_ALIGN(32) = { 0U };
        uint64_t u0 = READ64LE(block);
        uint64_t lo = u0;
        uint64_t u = READ64LE(block + 8U);
        uint64_t hi = u;
        Lib_IntVector_Intrinsics_vec256 f0 =
          Lib_IntVector_Intrinsics_vec256_load64(lo);
        Lib_IntVector_Intrinsics_vec256 f1 =
          Lib_IntVector_Intrinsics_vec256_load64(hi);
        Lib_IntVector_Intrinsics_vec256 f010 =
          Lib_IntVector_Intrinsics_vec256_and(
            f0, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f110 =
          Lib_IntVector_Intrinsics_vec256_and(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f0, 26U),
            Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f20 =
          Lib_IntVector_Intrinsics_vec256_or(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f0, 52U),
            Lib_IntVector_Intrinsics_vec256_shift_left64(
              Lib_IntVector_Intrinsics_vec256_and(
                f1, Lib_IntVector_Intrinsics_vec256_load64(0x3fffULL)),
              12U));
        Lib_IntVector_Intrinsics_vec256 f30 =
          Lib_IntVector_Intrinsics_vec256_and(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f1, 14U),
            Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f40 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(f1, 40U);
        Lib_IntVector_Intrinsics_vec256 f01 = f010;
        Lib_IntVector_Intrinsics_vec256 f111 = f110;
        Lib_IntVector_Intrinsics_vec256 f2 = f20;
        Lib_IntVector_Intrinsics_vec256 f3 = f30;
        Lib_IntVector_Intrinsics_vec256 f41 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f41;
        uint64_t b = 0x1000000ULL;
        Lib_IntVector_Intrinsics_vec256 mask =
          Lib_IntVector_Intrinsics_vec256_load64(b);
        Lib_IntVector_Intrinsics_vec256 f4 = e[4U];
        e[4U] = Lib_IntVector_Intrinsics_vec256_or(f4, mask);
        Lib_IntVector_Intrinsics_vec256* r = pre;
        Lib_IntVector_Intrinsics_vec256* r5 = pre + 5U;
        Lib_IntVector_Intrinsics_vec256 r0 = r[0U];
        Lib_IntVector_Intrinsics_vec256 r1 = r[1U];
        Lib_IntVector_Intrinsics_vec256 r2 = r[2U];
        Lib_IntVector_Intrinsics_vec256 r3 = r[3U];
        Lib_IntVector_Intrinsics_vec256 r4 = r[4U];
        Lib_IntVector_Intrinsics_vec256 r51 = r5[1U];
        Lib_IntVector_Intrinsics_vec256 r52 = r5[2U];
        Lib_IntVector_Intrinsics_vec256 r53 = r5[3U];
        Lib_IntVector_Intrinsics_vec256 r54 = r5[4U];
        Lib_IntVector_Intrinsics_vec256 f10 = e[0U];
        Lib_IntVector_Intrinsics_vec256 f11 = e[1U];
        Lib_IntVector_Intrinsics_vec256 f12 = e[2U];
        Lib_IntVector_Intrinsics_vec256 f13 = e[3U];
        Lib_IntVector_Intrinsics_vec256 f14 = e[4U];
        Lib_IntVector_Intrinsics_vec256 a0 = acc[0U];
        Lib_IntVector_Intrinsics_vec256 a1 = acc[1U];
        Lib_IntVector_Intrinsics_vec256 a2 = acc[2U];
        Lib_IntVector_Intrinsics_vec256 a3 = acc[3U];
        Lib_IntVector_Intrinsics_vec256 a4 = acc[4U];
        Lib_IntVector_Intrinsics_vec256 a01 =
          Lib_IntVector_Intrinsics_vec256_add64(a0, f10);
        Lib_IntVector_Intrinsics_vec256 a11 =
          Lib_IntVector_Intrinsics_vec256_add64(a1, f11);
        Lib_IntVector_Intrinsics_vec256 a21 =
          Lib_IntVector_Intrinsics_vec256_add64(a2, f12);
        Lib_IntVector_Intrinsics_vec256 a31 =
          Lib_IntVector_Intrinsics_vec256_add64(a3, f13);
        Lib_IntVector_Intrinsics_vec256 a41 =
          Lib_IntVector_Intrinsics_vec256_add64(a4, f14);
        Lib_IntVector_Intrinsics_vec256 a02 =
          Lib_IntVector_Intrinsics_vec256_mul64(r0, a01);
        Lib_IntVector_Intrinsics_vec256 a12 =
          Lib_IntVector_Intrinsics_vec256_mul64(r1, a01);
        Lib_IntVector_Intrinsics_vec256 a22 =
          Lib_IntVector_Intrinsics_vec256_mul64(r2, a01);
        Lib_IntVector_Intrinsics_vec256 a32 =
          Lib_IntVector_Intrinsics_vec256_mul64(r3, a01);
        Lib_IntVector_Intrinsics_vec256 a42 =
          Lib_IntVector_Intrinsics_vec256_mul64(r4, a01);
        Lib_IntVector_Intrinsics_vec256 a03 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a02, Lib_IntVector_Intrinsics_vec256_mul64(r54, a11));
        Lib_IntVector_Intrinsics_vec256 a13 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a12, Lib_IntVector_Intrinsics_vec256_mul64(r0, a11));
        Lib_IntVector_Intrinsics_vec256 a23 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a22, Lib_IntVector_Intrinsics_vec256_mul64(r1, a11));
        Lib_IntVector_Intrinsics_vec256 a33 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a32, Lib_IntVector_Intrinsics_vec256_mul64(r2, a11));
        Lib_IntVector_Intrinsics_vec256 a43 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a42, Lib_IntVector_Intrinsics_vec256_mul64(r3, a11));
        Lib_IntVector_Intrinsics_vec256 a04 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a03, Lib_IntVector_Intrinsics_vec256_mul64(r53, a21));
        Lib_IntVector_Intrinsics_vec256 a14 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a13, Lib_IntVector_Intrinsics_vec256_mul64(r54, a21));
        Lib_IntVector_Intrinsics_vec256 a24 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a23, Lib_IntVector_Intrinsics_vec256_mul64(r0, a21));
        Lib_IntVector_Intrinsics_vec256 a34 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a33, Lib_IntVector_Intrinsics_vec256_mul64(r1, a21));
        Lib_IntVector_Intrinsics_vec256 a44 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a43, Lib_IntVector_Intrinsics_vec256_mul64(r2, a21));
        Lib_IntVector_Intrinsics_vec256 a05 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a04, Lib_IntVector_Intrinsics_vec256_mul64(r52, a31));
        Lib_IntVector_Intrinsics_vec256 a15 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a14, Lib_IntVector_Intrinsics_vec256_mul64(r53, a31));
        Lib_IntVector_Intrinsics_vec256 a25 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a24, Lib_IntVector_Intrinsics_vec256_mul64(r54, a31));
        Lib_IntVector_Intrinsics_vec256 a35 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a34, Lib_IntVector_Intrinsics_vec256_mul64(r0, a31));
        Lib_IntVector_Intrinsics_vec256 a45 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a44, Lib_IntVector_Intrinsics_vec256_mul64(r1, a31));
        Lib_IntVector_Intrinsics_vec256 a06 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a05, Lib_IntVector_Intrinsics_vec256_mul64(r51, a41));
        Lib_IntVector_Intrinsics_vec256 a16 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a15, Lib_IntVector_Intrinsics_vec256_mul64(r52, a41));
        Lib_IntVector_Intrinsics_vec256 a26 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a25, Lib_IntVector_Intrinsics_vec256_mul64(r53, a41));
        Lib_IntVector_Intrinsics_vec256 a36 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a35, Lib_IntVector_Intrinsics_vec256_mul64(r54, a41));
        Lib_IntVector_Intrinsics_vec256 a46 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a45, Lib_IntVector_Intrinsics_vec256_mul64(r0, a41));
        Lib_IntVector_Intrinsics_vec256 t01 = a06;
        Lib_IntVector_Intrinsics_vec256 t11 = a16;
        Lib_IntVector_Intrinsics_vec256 t2 = a26;
        Lib_IntVector_Intrinsics_vec256 t3 = a36;
        Lib_IntVector_Intrinsics_vec256 t4 = a46;
        Lib_IntVector_Intrinsics_vec256 mask26 =
          Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
        Lib_IntVector_Intrinsics_vec256 z0 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(t01, 26U);
        Lib_IntVector_Intrinsics_vec256 z1 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 26U);
        Lib_IntVector_Intrinsics_vec256 x0 =
          Lib_IntVector_Intrinsics_vec256_and(t01, mask26);
        Lib_IntVector_Intrinsics_vec256 x3 =
          Lib_IntVector_Intrinsics_vec256_and(t3, mask26);
        Lib_IntVector_Intrinsics_vec256 x1 =
          Lib_IntVector_Intrinsics_vec256_add64(t11, z0);
        Lib_IntVector_Intrinsics_vec256 x4 =
          Lib_IntVector_Intrinsics_vec256_add64(t4, z1);
        Lib_IntVector_Intrinsics_vec256 z01 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x1, 26U);
        Lib_IntVector_Intrinsics_vec256 z11 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x4, 26U);
        Lib_IntVector_Intrinsics_vec256 t =
          Lib_IntVector_Intrinsics_vec256_shift_left64(z11, 2U);
        Lib_IntVector_Intrinsics_vec256 z12 =
          Lib_IntVector_Intrinsics_vec256_add64(z11, t);
        Lib_IntVector_Intrinsics_vec256 x11 =
          Lib_IntVector_Intrinsics_vec256_and(x1, mask26);
        Lib_IntVector_Intrinsics_vec256 x41 =
          Lib_IntVector_Intrinsics_vec256_and(x4, mask26);
        Lib_IntVector_Intrinsics_vec256 x2 =
          Lib_IntVector_Intrinsics_vec256_add64(t2, z01);
        Lib_IntVector_Intrinsics_vec256 x01 =
          Lib_IntVector_Intrinsics_vec256_add64(x0, z12);
        Lib_IntVector_Intrinsics_vec256 z02 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x2, 26U);
        Lib_IntVector_Intrinsics_vec256 z13 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x01, 26U);
        Lib_IntVector_Intrinsics_vec256 x21 =
          Lib_IntVector_Intrinsics_vec256_and(x2, mask26);
        Lib_IntVector_Intrinsics_vec256 x02 =
          Lib_IntVector_Intrinsics_vec256_and(x01, mask26);
        Lib_IntVector_Intrinsics_vec256 x31 =
          Lib_IntVector_Intrinsics_vec256_add64(x3, z02);
        Lib_IntVector_Intrinsics_vec256 x12 =
          Lib_IntVector_Intrinsics_vec256_add64(x11, z13);
        Lib_IntVector_Intrinsics_vec256 z03 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x31, 26U);
        Lib_IntVector_Intrinsics_vec256 x32 =
          Lib_IntVector_Intrinsics_vec256_and(x31, mask26);
        Lib_IntVector_Intrinsics_vec256 x42 =
          Lib_IntVector_Intrinsics_vec256_add64(x41, z03);
        Lib_IntVector_Intrinsics_vec256 o0 = x02;
        Lib_IntVector_Intrinsics_vec256 o1 = x12;
        Lib_IntVector_Intrinsics_vec256 o2 = x21;
        Lib_IntVector_Intrinsics_vec256 o3 = x32;
        Lib_IntVector_Intrinsics_vec256 o4 = x42;
        acc[0U] = o0;
        acc[1U] = o1;
        acc[2U] = o2;
        acc[3U] = o3;
        acc[4U] = o4;
    }
    if (rem > 0U) {
        uint8_t* last = t1 + nb * 16U;
        KRML_PRE_ALIGN(32)
        Lib_IntVector_Intrinsics_vec256 e[5U] KRML_POST_ALIGN(32) = { 0U };
        uint8_t tmp[16U] = { 0U };
        memcpy(tmp, last, rem * sizeof(uint8_t));
        uint64_t u0 = READ64LE(tmp);
        uint64_t lo = u0;
        uint64_t u = READ64LE(tmp + 8U);
        uint64_t hi = u;
        Lib_IntVector_Intrinsics_vec256 f0 =
          Lib_IntVector_Intrinsics_vec256_load64(lo);
        Lib_IntVector_Intrinsics_vec256 f1 =
          Lib_IntVector_Intrinsics_vec256_load64(hi);
        Lib_IntVector_Intrinsics_vec256 f010 =
          Lib_IntVector_Intrinsics_vec256_and(
            f0, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f110 =
          Lib_IntVector_Intrinsics_vec256_and(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f0, 26U),
            Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f20 =
          Lib_IntVector_Intrinsics_vec256_or(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f0, 52U),
            Lib_IntVector_Intrinsics_vec256_shift_left64(
              Lib_IntVector_Intrinsics_vec256_and(
                f1, Lib_IntVector_Intrinsics_vec256_load64(0x3fffULL)),
              12U));
        Lib_IntVector_Intrinsics_vec256 f30 =
          Lib_IntVector_Intrinsics_vec256_and(
            Lib_IntVector_Intrinsics_vec256_shift_right64(f1, 14U),
            Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
        Lib_IntVector_Intrinsics_vec256 f40 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(f1, 40U);
        Lib_IntVector_Intrinsics_vec256 f01 = f010;
        Lib_IntVector_Intrinsics_vec256 f111 = f110;
        Lib_IntVector_Intrinsics_vec256 f2 = f20;
        Lib_IntVector_Intrinsics_vec256 f3 = f30;
        Lib_IntVector_Intrinsics_vec256 f4 = f40;
        e[0U] = f01;
        e[1U] = f111;
        e[2U] = f2;
        e[3U] = f3;
        e[4U] = f4;
        uint64_t b = 1ULL << rem * 8U % 26U;
        Lib_IntVector_Intrinsics_vec256 mask =
          Lib_IntVector_Intrinsics_vec256_load64(b);
        Lib_IntVector_Intrinsics_vec256 fi = e[rem * 8U / 26U];
        e[rem * 8U / 26U] = Lib_IntVector_Intrinsics_vec256_or(fi, mask);
        Lib_IntVector_Intrinsics_vec256* r = pre;
        Lib_IntVector_Intrinsics_vec256* r5 = pre + 5U;
        Lib_IntVector_Intrinsics_vec256 r0 = r[0U];
        Lib_IntVector_Intrinsics_vec256 r1 = r[1U];
        Lib_IntVector_Intrinsics_vec256 r2 = r[2U];
        Lib_IntVector_Intrinsics_vec256 r3 = r[3U];
        Lib_IntVector_Intrinsics_vec256 r4 = r[4U];
        Lib_IntVector_Intrinsics_vec256 r51 = r5[1U];
        Lib_IntVector_Intrinsics_vec256 r52 = r5[2U];
        Lib_IntVector_Intrinsics_vec256 r53 = r5[3U];
        Lib_IntVector_Intrinsics_vec256 r54 = r5[4U];
        Lib_IntVector_Intrinsics_vec256 f10 = e[0U];
        Lib_IntVector_Intrinsics_vec256 f11 = e[1U];
        Lib_IntVector_Intrinsics_vec256 f12 = e[2U];
        Lib_IntVector_Intrinsics_vec256 f13 = e[3U];
        Lib_IntVector_Intrinsics_vec256 f14 = e[4U];
        Lib_IntVector_Intrinsics_vec256 a0 = acc[0U];
        Lib_IntVector_Intrinsics_vec256 a1 = acc[1U];
        Lib_IntVector_Intrinsics_vec256 a2 = acc[2U];
        Lib_IntVector_Intrinsics_vec256 a3 = acc[3U];
        Lib_IntVector_Intrinsics_vec256 a4 = acc[4U];
        Lib_IntVector_Intrinsics_vec256 a01 =
          Lib_IntVector_Intrinsics_vec256_add64(a0, f10);
        Lib_IntVector_Intrinsics_vec256 a11 =
          Lib_IntVector_Intrinsics_vec256_add64(a1, f11);
        Lib_IntVector_Intrinsics_vec256 a21 =
          Lib_IntVector_Intrinsics_vec256_add64(a2, f12);
        Lib_IntVector_Intrinsics_vec256 a31 =
          Lib_IntVector_Intrinsics_vec256_add64(a3, f13);
        Lib_IntVector_Intrinsics_vec256 a41 =
          Lib_IntVector_Intrinsics_vec256_add64(a4, f14);
        Lib_IntVector_Intrinsics_vec256 a02 =
          Lib_IntVector_Intrinsics_vec256_mul64(r0, a01);
        Lib_IntVector_Intrinsics_vec256 a12 =
          Lib_IntVector_Intrinsics_vec256_mul64(r1, a01);
        Lib_IntVector_Intrinsics_vec256 a22 =
          Lib_IntVector_Intrinsics_vec256_mul64(r2, a01);
        Lib_IntVector_Intrinsics_vec256 a32 =
          Lib_IntVector_Intrinsics_vec256_mul64(r3, a01);
        Lib_IntVector_Intrinsics_vec256 a42 =
          Lib_IntVector_Intrinsics_vec256_mul64(r4, a01);
        Lib_IntVector_Intrinsics_vec256 a03 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a02, Lib_IntVector_Intrinsics_vec256_mul64(r54, a11));
        Lib_IntVector_Intrinsics_vec256 a13 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a12, Lib_IntVector_Intrinsics_vec256_mul64(r0, a11));
        Lib_IntVector_Intrinsics_vec256 a23 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a22, Lib_IntVector_Intrinsics_vec256_mul64(r1, a11));
        Lib_IntVector_Intrinsics_vec256 a33 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a32, Lib_IntVector_Intrinsics_vec256_mul64(r2, a11));
        Lib_IntVector_Intrinsics_vec256 a43 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a42, Lib_IntVector_Intrinsics_vec256_mul64(r3, a11));
        Lib_IntVector_Intrinsics_vec256 a04 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a03, Lib_IntVector_Intrinsics_vec256_mul64(r53, a21));
        Lib_IntVector_Intrinsics_vec256 a14 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a13, Lib_IntVector_Intrinsics_vec256_mul64(r54, a21));
        Lib_IntVector_Intrinsics_vec256 a24 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a23, Lib_IntVector_Intrinsics_vec256_mul64(r0, a21));
        Lib_IntVector_Intrinsics_vec256 a34 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a33, Lib_IntVector_Intrinsics_vec256_mul64(r1, a21));
        Lib_IntVector_Intrinsics_vec256 a44 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a43, Lib_IntVector_Intrinsics_vec256_mul64(r2, a21));
        Lib_IntVector_Intrinsics_vec256 a05 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a04, Lib_IntVector_Intrinsics_vec256_mul64(r52, a31));
        Lib_IntVector_Intrinsics_vec256 a15 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a14, Lib_IntVector_Intrinsics_vec256_mul64(r53, a31));
        Lib_IntVector_Intrinsics_vec256 a25 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a24, Lib_IntVector_Intrinsics_vec256_mul64(r54, a31));
        Lib_IntVector_Intrinsics_vec256 a35 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a34, Lib_IntVector_Intrinsics_vec256_mul64(r0, a31));
        Lib_IntVector_Intrinsics_vec256 a45 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a44, Lib_IntVector_Intrinsics_vec256_mul64(r1, a31));
        Lib_IntVector_Intrinsics_vec256 a06 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a05, Lib_IntVector_Intrinsics_vec256_mul64(r51, a41));
        Lib_IntVector_Intrinsics_vec256 a16 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a15, Lib_IntVector_Intrinsics_vec256_mul64(r52, a41));
        Lib_IntVector_Intrinsics_vec256 a26 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a25, Lib_IntVector_Intrinsics_vec256_mul64(r53, a41));
        Lib_IntVector_Intrinsics_vec256 a36 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a35, Lib_IntVector_Intrinsics_vec256_mul64(r54, a41));
        Lib_IntVector_Intrinsics_vec256 a46 =
          Lib_IntVector_Intrinsics_vec256_add64(
            a45, Lib_IntVector_Intrinsics_vec256_mul64(r0, a41));
        Lib_IntVector_Intrinsics_vec256 t01 = a06;
        Lib_IntVector_Intrinsics_vec256 t11 = a16;
        Lib_IntVector_Intrinsics_vec256 t2 = a26;
        Lib_IntVector_Intrinsics_vec256 t3 = a36;
        Lib_IntVector_Intrinsics_vec256 t4 = a46;
        Lib_IntVector_Intrinsics_vec256 mask26 =
          Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
        Lib_IntVector_Intrinsics_vec256 z0 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(t01, 26U);
        Lib_IntVector_Intrinsics_vec256 z1 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(t3, 26U);
        Lib_IntVector_Intrinsics_vec256 x0 =
          Lib_IntVector_Intrinsics_vec256_and(t01, mask26);
        Lib_IntVector_Intrinsics_vec256 x3 =
          Lib_IntVector_Intrinsics_vec256_and(t3, mask26);
        Lib_IntVector_Intrinsics_vec256 x1 =
          Lib_IntVector_Intrinsics_vec256_add64(t11, z0);
        Lib_IntVector_Intrinsics_vec256 x4 =
          Lib_IntVector_Intrinsics_vec256_add64(t4, z1);
        Lib_IntVector_Intrinsics_vec256 z01 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x1, 26U);
        Lib_IntVector_Intrinsics_vec256 z11 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x4, 26U);
        Lib_IntVector_Intrinsics_vec256 t =
          Lib_IntVector_Intrinsics_vec256_shift_left64(z11, 2U);
        Lib_IntVector_Intrinsics_vec256 z12 =
          Lib_IntVector_Intrinsics_vec256_add64(z11, t);
        Lib_IntVector_Intrinsics_vec256 x11 =
          Lib_IntVector_Intrinsics_vec256_and(x1, mask26);
        Lib_IntVector_Intrinsics_vec256 x41 =
          Lib_IntVector_Intrinsics_vec256_and(x4, mask26);
        Lib_IntVector_Intrinsics_vec256 x2 =
          Lib_IntVector_Intrinsics_vec256_add64(t2, z01);
        Lib_IntVector_Intrinsics_vec256 x01 =
          Lib_IntVector_Intrinsics_vec256_add64(x0, z12);
        Lib_IntVector_Intrinsics_vec256 z02 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x2, 26U);
        Lib_IntVector_Intrinsics_vec256 z13 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x01, 26U);
        Lib_IntVector_Intrinsics_vec256 x21 =
          Lib_IntVector_Intrinsics_vec256_and(x2, mask26);
        Lib_IntVector_Intrinsics_vec256 x02 =
          Lib_IntVector_Intrinsics_vec256_and(x01, mask26);
        Lib_IntVector_Intrinsics_vec256 x31 =
          Lib_IntVector_Intrinsics_vec256_add64(x3, z02);
        Lib_IntVector_Intrinsics_vec256 x12 =
          Lib_IntVector_Intrinsics_vec256_add64(x11, z13);
        Lib_IntVector_Intrinsics_vec256 z03 =
          Lib_IntVector_Intrinsics_vec256_shift_right64(x31, 26U);
        Lib_IntVector_Intrinsics_vec256 x32 =
          Lib_IntVector_Intrinsics_vec256_and(x31, mask26);
        Lib_IntVector_Intrinsics_vec256 x42 =
          Lib_IntVector_Intrinsics_vec256_add64(x41, z03);
        Lib_IntVector_Intrinsics_vec256 o0 = x02;
        Lib_IntVector_Intrinsics_vec256 o1 = x12;
        Lib_IntVector_Intrinsics_vec256 o2 = x21;
        Lib_IntVector_Intrinsics_vec256 o3 = x32;
        Lib_IntVector_Intrinsics_vec256 o4 = x42;
        acc[0U] = o0;
        acc[1U] = o1;
        acc[2U] = o2;
        acc[3U] = o3;
        acc[4U] = o4;
        return;
    }
}

void
Hacl_MAC_Poly1305_Simd256_poly1305_finish(uint8_t* tag,
                                          uint8_t* key,
                                          Lib_IntVector_Intrinsics_vec256* ctx)
{
    Lib_IntVector_Intrinsics_vec256* acc = ctx;
    uint8_t* ks = key + 16U;
    Lib_IntVector_Intrinsics_vec256 f0 = acc[0U];
    Lib_IntVector_Intrinsics_vec256 f13 = acc[1U];
    Lib_IntVector_Intrinsics_vec256 f23 = acc[2U];
    Lib_IntVector_Intrinsics_vec256 f33 = acc[3U];
    Lib_IntVector_Intrinsics_vec256 f40 = acc[4U];
    Lib_IntVector_Intrinsics_vec256 l0 = Lib_IntVector_Intrinsics_vec256_add64(
      f0, Lib_IntVector_Intrinsics_vec256_zero);
    Lib_IntVector_Intrinsics_vec256 tmp00 = Lib_IntVector_Intrinsics_vec256_and(
      l0, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c00 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l0, 26U);
    Lib_IntVector_Intrinsics_vec256 l1 =
      Lib_IntVector_Intrinsics_vec256_add64(f13, c00);
    Lib_IntVector_Intrinsics_vec256 tmp10 = Lib_IntVector_Intrinsics_vec256_and(
      l1, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c10 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l1, 26U);
    Lib_IntVector_Intrinsics_vec256 l2 =
      Lib_IntVector_Intrinsics_vec256_add64(f23, c10);
    Lib_IntVector_Intrinsics_vec256 tmp20 = Lib_IntVector_Intrinsics_vec256_and(
      l2, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c20 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l2, 26U);
    Lib_IntVector_Intrinsics_vec256 l3 =
      Lib_IntVector_Intrinsics_vec256_add64(f33, c20);
    Lib_IntVector_Intrinsics_vec256 tmp30 = Lib_IntVector_Intrinsics_vec256_and(
      l3, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c30 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l3, 26U);
    Lib_IntVector_Intrinsics_vec256 l4 =
      Lib_IntVector_Intrinsics_vec256_add64(f40, c30);
    Lib_IntVector_Intrinsics_vec256 tmp40 = Lib_IntVector_Intrinsics_vec256_and(
      l4, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c40 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l4, 26U);
    Lib_IntVector_Intrinsics_vec256 f010 =
      Lib_IntVector_Intrinsics_vec256_add64(
        tmp00, Lib_IntVector_Intrinsics_vec256_smul64(c40, 5ULL));
    Lib_IntVector_Intrinsics_vec256 f110 = tmp10;
    Lib_IntVector_Intrinsics_vec256 f210 = tmp20;
    Lib_IntVector_Intrinsics_vec256 f310 = tmp30;
    Lib_IntVector_Intrinsics_vec256 f410 = tmp40;
    Lib_IntVector_Intrinsics_vec256 l = Lib_IntVector_Intrinsics_vec256_add64(
      f010, Lib_IntVector_Intrinsics_vec256_zero);
    Lib_IntVector_Intrinsics_vec256 tmp0 = Lib_IntVector_Intrinsics_vec256_and(
      l, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c0 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l, 26U);
    Lib_IntVector_Intrinsics_vec256 l5 =
      Lib_IntVector_Intrinsics_vec256_add64(f110, c0);
    Lib_IntVector_Intrinsics_vec256 tmp1 = Lib_IntVector_Intrinsics_vec256_and(
      l5, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c1 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l5, 26U);
    Lib_IntVector_Intrinsics_vec256 l6 =
      Lib_IntVector_Intrinsics_vec256_add64(f210, c1);
    Lib_IntVector_Intrinsics_vec256 tmp2 = Lib_IntVector_Intrinsics_vec256_and(
      l6, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c2 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l6, 26U);
    Lib_IntVector_Intrinsics_vec256 l7 =
      Lib_IntVector_Intrinsics_vec256_add64(f310, c2);
    Lib_IntVector_Intrinsics_vec256 tmp3 = Lib_IntVector_Intrinsics_vec256_and(
      l7, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c3 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l7, 26U);
    Lib_IntVector_Intrinsics_vec256 l8 =
      Lib_IntVector_Intrinsics_vec256_add64(f410, c3);
    Lib_IntVector_Intrinsics_vec256 tmp4 = Lib_IntVector_Intrinsics_vec256_and(
      l8, Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL));
    Lib_IntVector_Intrinsics_vec256 c4 =
      Lib_IntVector_Intrinsics_vec256_shift_right64(l8, 26U);
    Lib_IntVector_Intrinsics_vec256 f02 = Lib_IntVector_Intrinsics_vec256_add64(
      tmp0, Lib_IntVector_Intrinsics_vec256_smul64(c4, 5ULL));
    Lib_IntVector_Intrinsics_vec256 f12 = tmp1;
    Lib_IntVector_Intrinsics_vec256 f22 = tmp2;
    Lib_IntVector_Intrinsics_vec256 f32 = tmp3;
    Lib_IntVector_Intrinsics_vec256 f42 = tmp4;
    Lib_IntVector_Intrinsics_vec256 mh =
      Lib_IntVector_Intrinsics_vec256_load64(0x3ffffffULL);
    Lib_IntVector_Intrinsics_vec256 ml =
      Lib_IntVector_Intrinsics_vec256_load64(0x3fffffbULL);
    Lib_IntVector_Intrinsics_vec256 mask =
      Lib_IntVector_Intrinsics_vec256_eq64(f42, mh);
    Lib_IntVector_Intrinsics_vec256 mask1 = Lib_IntVector_Intrinsics_vec256_and(
      mask, Lib_IntVector_Intrinsics_vec256_eq64(f32, mh));
    Lib_IntVector_Intrinsics_vec256 mask2 = Lib_IntVector_Intrinsics_vec256_and(
      mask1, Lib_IntVector_Intrinsics_vec256_eq64(f22, mh));
    Lib_IntVector_Intrinsics_vec256 mask3 = Lib_IntVector_Intrinsics_vec256_and(
      mask2, Lib_IntVector_Intrinsics_vec256_eq64(f12, mh));
    Lib_IntVector_Intrinsics_vec256 mask4 = Lib_IntVector_Intrinsics_vec256_and(
      mask3,
      Lib_IntVector_Intrinsics_vec256_lognot(
        Lib_IntVector_Intrinsics_vec256_gt64(ml, f02)));
    Lib_IntVector_Intrinsics_vec256 ph =
      Lib_IntVector_Intrinsics_vec256_and(mask4, mh);
    Lib_IntVector_Intrinsics_vec256 pl =
      Lib_IntVector_Intrinsics_vec256_and(mask4, ml);
    Lib_IntVector_Intrinsics_vec256 o0 =
      Lib_IntVector_Intrinsics_vec256_sub64(f02, pl);
    Lib_IntVector_Intrinsics_vec256 o1 =
      Lib_IntVector_Intrinsics_vec256_sub64(f12, ph);
    Lib_IntVector_Intrinsics_vec256 o2 =
      Lib_IntVector_Intrinsics_vec256_sub64(f22, ph);
    Lib_IntVector_Intrinsics_vec256 o3 =
      Lib_IntVector_Intrinsics_vec256_sub64(f32, ph);
    Lib_IntVector_Intrinsics_vec256 o4 =
      Lib_IntVector_Intrinsics_vec256_sub64(f42, ph);
    Lib_IntVector_Intrinsics_vec256 f011 = o0;
    Lib_IntVector_Intrinsics_vec256 f111 = o1;
    Lib_IntVector_Intrinsics_vec256 f211 = o2;
    Lib_IntVector_Intrinsics_vec256 f311 = o3;
    Lib_IntVector_Intrinsics_vec256 f411 = o4;
    acc[0U] = f011;
    acc[1U] = f111;
    acc[2U] = f211;
    acc[3U] = f311;
    acc[4U] = f411;
    Lib_IntVector_Intrinsics_vec256 f00 = acc[0U];
    Lib_IntVector_Intrinsics_vec256 f1 = acc[1U];
    Lib_IntVector_Intrinsics_vec256 f2 = acc[2U];
    Lib_IntVector_Intrinsics_vec256 f3 = acc[3U];
    Lib_IntVector_Intrinsics_vec256 f4 = acc[4U];
    uint64_t f01 = Lib_IntVector_Intrinsics_vec256_extract64(f00, 0U);
    uint64_t f112 = Lib_IntVector_Intrinsics_vec256_extract64(f1, 0U);
    uint64_t f212 = Lib_IntVector_Intrinsics_vec256_extract64(f2, 0U);
    uint64_t f312 = Lib_IntVector_Intrinsics_vec256_extract64(f3, 0U);
    uint64_t f41 = Lib_IntVector_Intrinsics_vec256_extract64(f4, 0U);
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

typedef struct option___Lib_IntVector_Intrinsics_vec256__s
{
    Hacl_Streaming_Types_optional tag;
    Lib_IntVector_Intrinsics_vec256* v;
} option___Lib_IntVector_Intrinsics_vec256_;

Hacl_MAC_Poly1305_Simd256_state_t*
Hacl_MAC_Poly1305_Simd256_malloc(uint8_t* key)
{
    uint8_t* buf = (uint8_t*)KRML_HOST_CALLOC(64U, sizeof(uint8_t));
    if (buf == NULL) {
        return NULL;
    }
    uint8_t* buf1 = buf;
    Lib_IntVector_Intrinsics_vec256* r1 =
      (Lib_IntVector_Intrinsics_vec256*)KRML_ALIGNED_MALLOC(
        32, sizeof(Lib_IntVector_Intrinsics_vec256) * 25U);
    if (r1 != NULL) {
        memset(r1, 0U, 25U * sizeof(Lib_IntVector_Intrinsics_vec256));
    }
    option___Lib_IntVector_Intrinsics_vec256_ block_state;
    if (r1 == NULL) {
        block_state = ((option___Lib_IntVector_Intrinsics_vec256_){
          .tag = Hacl_Streaming_Types_None });
    } else {
        block_state = ((option___Lib_IntVector_Intrinsics_vec256_){
          .tag = Hacl_Streaming_Types_Some, .v = r1 });
    }
    if (block_state.tag == Hacl_Streaming_Types_None) {
        KRML_HOST_FREE(buf1);
        return NULL;
    }
    if (block_state.tag == Hacl_Streaming_Types_Some) {
        Lib_IntVector_Intrinsics_vec256* block_state1 = block_state.v;
        uint8_t* b = (uint8_t*)KRML_HOST_CALLOC(32U, sizeof(uint8_t));
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
            KRML_ALIGNED_FREE(block_state1);
            KRML_HOST_FREE(buf1);
            k_0 = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_None });
        } else if (k_.tag == Hacl_Streaming_Types_Some) {
            uint8_t* k_1 = k_.v;
            memcpy(k_1, key, 32U * sizeof(uint8_t));
            k_0 = ((FStar_Pervasives_Native_option___uint8_t_){
              .tag = Hacl_Streaming_Types_Some, .v = k_1 });
        } else {
            k_0 =
              KRML_EABORT(FStar_Pervasives_Native_option___uint8_t_,
                          "unreachable (pattern matches are exhaustive in F*)");
        }
        if (k_0.tag == Hacl_Streaming_Types_None) {
            return NULL;
        }
        if (k_0.tag == Hacl_Streaming_Types_Some) {
            uint8_t* k_1 = k_0.v;
            Hacl_MAC_Poly1305_Simd256_state_t s = { .block_state = block_state1,
                                                    .buf = buf1,
                                                    .total_len = (uint64_t)0U,
                                                    .p_key = k_1 };
            Hacl_MAC_Poly1305_Simd256_state_t* p =
              (Hacl_MAC_Poly1305_Simd256_state_t*)KRML_HOST_MALLOC(
                sizeof(Hacl_MAC_Poly1305_Simd256_state_t));
            if (p != NULL) {
                p[0U] = s;
            }
            if (p == NULL) {
                KRML_HOST_FREE(k_1);
                KRML_ALIGNED_FREE(block_state1);
                KRML_HOST_FREE(buf1);
                return NULL;
            }
            Hacl_MAC_Poly1305_Simd256_poly1305_init(block_state1, key);
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
Hacl_MAC_Poly1305_Simd256_reset(Hacl_MAC_Poly1305_Simd256_state_t* state,
                                uint8_t* key)
{
    Hacl_MAC_Poly1305_Simd256_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    Lib_IntVector_Intrinsics_vec256* block_state = scrut.block_state;
    Hacl_MAC_Poly1305_Simd256_poly1305_init(block_state, key);
    memcpy(k_, key, 32U * sizeof(uint8_t));
    uint8_t* k_1 = k_;
    Hacl_MAC_Poly1305_Simd256_state_t tmp = { .block_state = block_state,
                                              .buf = buf,
                                              .total_len = (uint64_t)0U,
                                              .p_key = k_1 };
    state[0U] = tmp;
}

/**
0 = success, 1 = max length exceeded
*/
Hacl_Streaming_Types_error_code
Hacl_MAC_Poly1305_Simd256_update(Hacl_MAC_Poly1305_Simd256_state_t* state,
                                 uint8_t* chunk,
                                 uint32_t chunk_len)
{
    Hacl_MAC_Poly1305_Simd256_state_t s = *state;
    uint64_t total_len = s.total_len;
    if ((uint64_t)chunk_len > 0xffffffffULL - total_len) {
        return Hacl_Streaming_Types_MaximumLengthExceeded;
    }
    uint32_t sz;
    if (total_len % (uint64_t)64U == 0ULL && total_len > 0ULL) {
        sz = 64U;
    } else {
        sz = (uint32_t)(total_len % (uint64_t)64U);
    }
    if (chunk_len <= 64U - sz) {
        Hacl_MAC_Poly1305_Simd256_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec256* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)64U == 0ULL && total_len1 > 0ULL) {
            sz1 = 64U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)64U);
        }
        uint8_t* buf2 = buf + sz1;
        memcpy(buf2, chunk, chunk_len * sizeof(uint8_t));
        uint64_t total_len2 = total_len1 + (uint64_t)chunk_len;
        *state =
          ((Hacl_MAC_Poly1305_Simd256_state_t){ .block_state = block_state1,
                                                .buf = buf,
                                                .total_len = total_len2,
                                                .p_key = k_1 });
    } else if (sz == 0U) {
        Hacl_MAC_Poly1305_Simd256_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec256* block_state1 = s1.block_state;
        uint8_t* buf = s1.buf;
        uint64_t total_len1 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)64U == 0ULL && total_len1 > 0ULL) {
            sz1 = 64U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)64U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 64U, buf);
        }
        uint32_t ite;
        if ((uint64_t)chunk_len % (uint64_t)64U == 0ULL &&
            (uint64_t)chunk_len > 0ULL) {
            ite = 64U;
        } else {
            ite = (uint32_t)((uint64_t)chunk_len % (uint64_t)64U);
        }
        uint32_t n_blocks = (chunk_len - ite) / 64U;
        uint32_t data1_len = n_blocks * 64U;
        uint32_t data2_len = chunk_len - data1_len;
        uint8_t* data1 = chunk;
        uint8_t* data2 = chunk + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_Simd256_state_t){
          .block_state = block_state1,
          .buf = buf,
          .total_len = total_len1 + (uint64_t)chunk_len,
          .p_key = k_1 });
    } else {
        uint32_t diff = 64U - sz;
        uint8_t* chunk1 = chunk;
        uint8_t* chunk2 = chunk + diff;
        Hacl_MAC_Poly1305_Simd256_state_t s1 = *state;
        Lib_IntVector_Intrinsics_vec256* block_state10 = s1.block_state;
        uint8_t* buf0 = s1.buf;
        uint64_t total_len10 = s1.total_len;
        uint8_t* k_1 = s1.p_key;
        uint32_t sz10;
        if (total_len10 % (uint64_t)64U == 0ULL && total_len10 > 0ULL) {
            sz10 = 64U;
        } else {
            sz10 = (uint32_t)(total_len10 % (uint64_t)64U);
        }
        uint8_t* buf2 = buf0 + sz10;
        memcpy(buf2, chunk1, diff * sizeof(uint8_t));
        uint64_t total_len2 = total_len10 + (uint64_t)diff;
        *state =
          ((Hacl_MAC_Poly1305_Simd256_state_t){ .block_state = block_state10,
                                                .buf = buf0,
                                                .total_len = total_len2,
                                                .p_key = k_1 });
        Hacl_MAC_Poly1305_Simd256_state_t s10 = *state;
        Lib_IntVector_Intrinsics_vec256* block_state1 = s10.block_state;
        uint8_t* buf = s10.buf;
        uint64_t total_len1 = s10.total_len;
        uint8_t* k_10 = s10.p_key;
        uint32_t sz1;
        if (total_len1 % (uint64_t)64U == 0ULL && total_len1 > 0ULL) {
            sz1 = 64U;
        } else {
            sz1 = (uint32_t)(total_len1 % (uint64_t)64U);
        }
        if (!(sz1 == 0U)) {
            poly1305_update(block_state1, 64U, buf);
        }
        uint32_t ite;
        if ((uint64_t)(chunk_len - diff) % (uint64_t)64U == 0ULL &&
            (uint64_t)(chunk_len - diff) > 0ULL) {
            ite = 64U;
        } else {
            ite = (uint32_t)((uint64_t)(chunk_len - diff) % (uint64_t)64U);
        }
        uint32_t n_blocks = (chunk_len - diff - ite) / 64U;
        uint32_t data1_len = n_blocks * 64U;
        uint32_t data2_len = chunk_len - diff - data1_len;
        uint8_t* data1 = chunk2;
        uint8_t* data2 = chunk2 + data1_len;
        poly1305_update(block_state1, data1_len, data1);
        uint8_t* dst = buf;
        memcpy(dst, data2, data2_len * sizeof(uint8_t));
        *state = ((Hacl_MAC_Poly1305_Simd256_state_t){
          .block_state = block_state1,
          .buf = buf,
          .total_len = total_len1 + (uint64_t)(chunk_len - diff),
          .p_key = k_10 });
    }
    return Hacl_Streaming_Types_Success;
}

void
Hacl_MAC_Poly1305_Simd256_digest(Hacl_MAC_Poly1305_Simd256_state_t* state,
                                 uint8_t* output)
{
    Hacl_MAC_Poly1305_Simd256_state_t scrut = *state;
    Lib_IntVector_Intrinsics_vec256* block_state = scrut.block_state;
    uint8_t* buf_ = scrut.buf;
    uint64_t total_len = scrut.total_len;
    uint8_t* k_ = scrut.p_key;
    uint32_t r;
    if (total_len % (uint64_t)64U == 0ULL && total_len > 0ULL) {
        r = 64U;
    } else {
        r = (uint32_t)(total_len % (uint64_t)64U);
    }
    uint8_t* buf_1 = buf_;
    KRML_PRE_ALIGN(32)
    Lib_IntVector_Intrinsics_vec256 r1[25U] KRML_POST_ALIGN(32) = { 0U };
    Lib_IntVector_Intrinsics_vec256* tmp_block_state = r1;
    memcpy(tmp_block_state,
           block_state,
           25U * sizeof(Lib_IntVector_Intrinsics_vec256));
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
    KRML_PRE_ALIGN(32)
    Lib_IntVector_Intrinsics_vec256 tmp[25U] KRML_POST_ALIGN(32) = { 0U };
    memcpy(tmp, tmp_block_state, 25U * sizeof(Lib_IntVector_Intrinsics_vec256));
    Hacl_MAC_Poly1305_Simd256_poly1305_finish(output, k_, tmp);
}

void
Hacl_MAC_Poly1305_Simd256_free(Hacl_MAC_Poly1305_Simd256_state_t* state)
{
    Hacl_MAC_Poly1305_Simd256_state_t scrut = *state;
    uint8_t* k_ = scrut.p_key;
    uint8_t* buf = scrut.buf;
    Lib_IntVector_Intrinsics_vec256* block_state = scrut.block_state;
    KRML_HOST_FREE(k_);
    KRML_ALIGNED_FREE(block_state);
    KRML_HOST_FREE(buf);
    KRML_HOST_FREE(state);
}

void
Hacl_MAC_Poly1305_Simd256_mac(uint8_t* output,
                              uint8_t* input,
                              uint32_t input_len,
                              uint8_t* key)
{
    KRML_PRE_ALIGN(32)
    Lib_IntVector_Intrinsics_vec256 ctx[25U] KRML_POST_ALIGN(32) = { 0U };
    Hacl_MAC_Poly1305_Simd256_poly1305_init(ctx, key);
    poly1305_update(ctx, input_len, input);
    Hacl_MAC_Poly1305_Simd256_poly1305_finish(output, key, ctx);
}

#endif
