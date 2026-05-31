/* Copyright (c) 2022 Arm Limited
 * Copyright (c) 2022 Hanno Becker
 * Copyright (c) 2023 Amin Abdulrahman, Matthias Kannwischer
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [NeonNTT]
 *   Neon NTT: Faster Dilithium, Kyber, and Saber on Cortex-A72 and Apple M1
 *   Becker, Hwang, Kannwischer, Yang, Yang
 *   https://eprint.iacr.org/2021/986
 *
 * - [SLOTHY_Paper]
 *   Fast and Clean: Auditable high-performance assembly via constraint solving
 *   Abdulrahman, Becker, Kannwischer, Klein
 *   https://eprint.iacr.org/2022/1303
 */

/* AArch64 ML-DSA inverse NTT following @[NeonNTT] and @[SLOTHY_Paper] */

#include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_AARCH64) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

.macro mulmodq dst, src, const, idx0, idx1
        // Signed barrett multiplication @[NeonNTT, Section 3.1.2] using
        // round-to-nearest-even-integer approximation. Following loc.cit.,
        // this is functionally the same as a signed Montgomery multiplication
        // with a suitable constant of absolute value < q.
        sqrdmulh t2.4s,      \src\().4s, \const\().s[\idx1\()]
        mul      \dst\().4s, \src\().4s, \const\().s[\idx0\()]
        mls      \dst\().4s, t2.4s,      consts.s[0]
        // Bounds: |dst| < BARMUL_BOUND
        //
        // (See the end of this file for the exact value of BARMUL_BOUND)
.endm

.macro mulmod dst, src, const, const_twisted
        // Same as mulmodq, but with different multiplier across each lanes.
        sqrdmulh t2.4s,      \src\().4s, \const_twisted\().4s
        mul      \dst\().4s, \src\().4s, \const\().4s
        mls      \dst\().4s, t2.4s,      consts.s[0]
        // Bounds: |dst| < BARMUL_BOUND
.endm

.macro gs_butterfly a, b, root, idx0, idx1
        // Compute a' = a + b, b' = barmul(a - b, zeta), commonly known as the
        // Gentleman-Sande butterfly.
        sub tmp.4s,   \a\().4s, \b\().4s
        add \a\().4s, \a\().4s, \b\().4s
        mulmodq  \b, tmp, \root, \idx0, \idx1
        // Bounds: |a'| <= |a| + |b|
        //         |b'| < BARMUL_BOUND
.endm

.macro gs_butterfly_v a, b, root, root_twisted
        // Same as cs_butterfly, but with different zetas across each lanes.
        sub tmp.4s,   \a\().4s, \b\().4s
        add \a\().4s, \a\().4s, \b\().4s
        mulmod  \b, tmp, \root, \root_twisted
        // Bounds: |a'| <= |a| + |b|
        //         |b'| < BARMUL_BOUND
.endm

.macro mul_ninv dst0, dst1, dst2, dst3, src0, src1, src2, src3
        // Scale by 1/256*R via Barrett multiplication
        mulmod \dst0, \src0, ninv, ninv_tw
        mulmod \dst1, \src1, ninv, ninv_tw
        mulmod \dst2, \src2, ninv, ninv_tw
        mulmod \dst3, \src3, ninv, ninv_tw
        // Bounds: |dst{i}| < BARMUL_BOUND
.endm

.macro load_roots_1234
        ldr q_root0, [r123456_ptr], #(8*16)
        ldr q_root1, [r123456_ptr, #(-8*16 + 1*16)]
        ldr q_root2, [r123456_ptr, #(-8*16 + 2*16)]
        ldr q_root3, [r123456_ptr, #(-8*16 + 3*16)]
        ldr q_root4, [r123456_ptr, #(-8*16 + 4*16)]
        ldr q_root5, [r123456_ptr, #(-8*16 + 5*16)]
        ldr q_root6, [r123456_ptr, #(-8*16 + 6*16)]
        ldr q_root7, [r123456_ptr, #(-8*16 + 7*16)]
.endm

.macro load_next_roots_56
        ldr d_root1, [r123456_ptr], #32
        ldr q_root0, [r123456_ptr, #-16]
.endm

.macro load_next_roots_78
        ldr q_root0,    [r78_ptr], #(6*16)
        ldr q_root0_tw, [r78_ptr, #(-6*16 + 1*16)]
        ldr q_root1,    [r78_ptr, #(-6*16 + 2*16)]
        ldr q_root1_tw, [r78_ptr, #(-6*16 + 3*16)]
        ldr q_root2,    [r78_ptr, #(-6*16 + 4*16)]
        ldr q_root2_tw, [r78_ptr, #(-6*16 + 5*16)]
.endm

.macro transpose4 data0, data1, data2, data3
        trn1 t0.4s, \data0\().4s, \data1\().4s
        trn2 t1.4s, \data0\().4s, \data1\().4s
        trn1 t2.4s, \data2\().4s, \data3\().4s
        trn2 t3.4s, \data2\().4s, \data3\().4s

        trn2 \data2\().2d, t0.2d, t2.2d
        trn2 \data3\().2d, t1.2d, t3.2d
        trn1 \data0\().2d, t0.2d, t2.2d
        trn1 \data1\().2d, t1.2d, t3.2d
.endm

.macro save_vregs
        sub sp, sp, #(16*4)
        stp  d8,  d9, [sp, #16*0]
        stp d10, d11, [sp, #16*1]
        stp d12, d13, [sp, #16*2]
        stp d14, d15, [sp, #16*3]
.endm

.macro restore_vregs
        ldp  d8,  d9, [sp, #16*0]
        ldp d10, d11, [sp, #16*1]
        ldp d12, d13, [sp, #16*2]
        ldp d14, d15, [sp, #16*3]
        add sp, sp, #(16*4)
.endm

.macro push_stack
        save_vregs
.endm

.macro pop_stack
        restore_vregs
.endm

        in          .req x0
        r78_ptr     .req x1
        r123456_ptr .req x2

        inp     .req x3
        count   .req x4
        wtmp    .req w5

        data0  .req v8
        data1  .req v9
        data2  .req v10
        data3  .req v11
        data4  .req v12
        data5  .req v13
        data6  .req v14
        data7  .req v15
        data8  .req v16
        data9  .req v17
        data10 .req v18
        data11 .req v19
        data12 .req v20
        data13 .req v21
        data14 .req v22
        data15 .req v23

        q_data0  .req q8
        q_data1  .req q9
        q_data2  .req q10
        q_data3  .req q11
        q_data4  .req q12
        q_data5  .req q13
        q_data6  .req q14
        q_data7  .req q15
        q_data8  .req q16
        q_data9  .req q17
        q_data10 .req q18
        q_data11 .req q19
        q_data12 .req q20
        q_data13 .req q21
        q_data14 .req q22
        q_data15 .req q23

        root0    .req v0
        root1    .req v1
        root2    .req v2
        root3    .req v3
        root4    .req v4
        root5    .req v5
        root6    .req v6
        root7    .req v7
        root0_tw .req v4
        root1_tw .req v5
        root2_tw .req v6
        root3_tw .req v7

        consts     .req v31
        q_consts   .req q31

        q_root0    .req q0
        q_root1    .req q1
        q_root2    .req q2
        q_root3    .req q3
        q_root4    .req q4
        q_root5    .req q5
        q_root6    .req q6
        q_root7    .req q7
        q_root0_tw .req q4
        q_root1_tw .req q5
        q_root2_tw .req q6
        q_root3_tw .req q7

        d_root1    .req d1

        tmp .req v24
        t0  .req v25
        t1  .req v26
        t2  .req v27
        t3  .req v28

        ninv             .req v29
        ninv_tw          .req v30

        .text
        .global MLD_ASM_NAMESPACE(intt_asm)
        .balign 4
MLD_ASM_FN_SYMBOL(intt_asm)
        push_stack

        // Load MLDSA_Q = 8380417
        movz wtmp, #0xe001
        movk wtmp, #0x7f, lsl #16
        dup consts.4s, wtmp

        mov inp, in
        mov count, #16

        .p2align 2
intt_layer5678_start:

        ldr q_data0, [inp, #(16*0)]
        ldr q_data1, [inp, #(16*1)]
        ldr q_data2, [inp, #(16*2)]
        ldr q_data3, [inp, #(16*3)]

        transpose4 data0, data1, data2, data3

        // Bounds: |data{i}| < q

        load_next_roots_78

        // Layer 8
        gs_butterfly_v data0, data1, root1, root1_tw
        gs_butterfly_v data2, data3, root2, root2_tw

        // Bounds: |data{i}| < 2q           for i in 0, 2
        //                   < BARMUL_BOUND for i in 1, 3
        //
        // Note that since 2^31 / q ~ 256.25 > 256, the sum of all 256
        // coefficients does not overflow. This allows us to greatly simplify
        // the range analysis by relaxing and unifying the bounds of all
        // coefficients on the same layer. As a concrete example, here we relax
        // the bounds on data{1,3} and conclude that for all i
        //
        // Bounds: |data{i}| < 2q
        //
        // In all but last of the following levels of butterflies, we do the
        // same relaxation without explicit mention.

        // Layer 7
        gs_butterfly_v data0, data2, root0, root0_tw
        gs_butterfly_v data1, data3, root0, root0_tw

        // Bounds: |data{i}| < 4q

        transpose4 data0, data1, data2, data3

        load_next_roots_56

        // Layer 6
        gs_butterfly data0, data1, root0, 0, 1
        gs_butterfly data2, data3, root0, 2, 3

        // Bounds: |data{i}| < 8q

        // Layer 5
        gs_butterfly data0, data2, root1, 0, 1
        gs_butterfly data1, data3, root1, 0, 1

        // Bounds: |data{i}| < 16q

        str q_data0, [inp], #(64)
        str q_data1, [inp, #(-64 + 16*1)]
        str q_data2, [inp, #(-64 + 16*2)]
        str q_data3, [inp, #(-64 + 16*3)]

        subs count, count, #1
        cbnz count, intt_layer5678_start

        // ---------------------------------------------------------------------

        /* check-magic: 16382 == pow(2, 32-8, MLDSA_Q) */
        // Load scaling factor 1/256*R
        mov wtmp, #16382 // 2^(32 - 8) mod Q
        dup ninv.4s, wtmp

        /* check-magic: 4197891 == round(16382 * 2**31 / MLDSA_Q) */
        // Load Barrett constant for 1/256*R
        movz wtmp, #0x0e03
        movk wtmp, #0x40, lsl #16
        dup ninv_tw.4s, wtmp

        mov count, #4
        load_roots_1234

        .p2align 2

intt_layer1234_start:

        ldr q_data0,  [in, #0]
        ldr q_data1,  [in, #(1*(512/8))]
        ldr q_data2,  [in, #(2*(512/8))]
        ldr q_data3,  [in, #(3*(512/8))]
        ldr q_data4,  [in, #(4*(512/8))]
        ldr q_data5,  [in, #(5*(512/8))]
        ldr q_data6,  [in, #(6*(512/8))]
        ldr q_data7,  [in, #(7*(512/8))]
        ldr q_data8,  [in, #(8*(512/8))]
        ldr q_data9,  [in, #(9*(512/8))]
        ldr q_data10, [in, #(10*(512/8))]
        ldr q_data11, [in, #(11*(512/8))]
        ldr q_data12, [in, #(12*(512/8))]
        ldr q_data13, [in, #(13*(512/8))]
        ldr q_data14, [in, #(14*(512/8))]
        ldr q_data15, [in, #(15*(512/8))]

        // Layer 4
        gs_butterfly data0, data1, root3, 2, 3
        gs_butterfly data2, data3, root4, 0, 1
        gs_butterfly data4, data5, root4, 2, 3
        gs_butterfly data6, data7, root5, 0, 1
        gs_butterfly data8, data9, root5, 2, 3
        gs_butterfly data10, data11, root6, 0, 1
        gs_butterfly data12, data13, root6, 2, 3
        gs_butterfly data14, data15, root7, 0, 1

        // Bounds: |data{i}| < 32q

        // Layer 3
        gs_butterfly data0, data2, root1, 2, 3
        gs_butterfly data1, data3, root1, 2, 3
        gs_butterfly data4, data6, root2, 0, 1
        gs_butterfly data5, data7, root2, 0, 1
        gs_butterfly data8, data10, root2, 2, 3
        gs_butterfly data9, data11, root2, 2, 3
        gs_butterfly data12, data14, root3, 0, 1
        gs_butterfly data13, data15, root3, 0, 1

        // Bounds: |data{i}| < 64q

        // Layer 2
        gs_butterfly data0, data4, root0, 2, 3
        gs_butterfly data1, data5, root0, 2, 3
        gs_butterfly data2, data6, root0, 2, 3
        gs_butterfly data3, data7, root0, 2, 3
        gs_butterfly data8, data12, root1, 0, 1
        gs_butterfly data9, data13, root1, 0, 1
        gs_butterfly data10, data14, root1, 0, 1
        gs_butterfly data11, data15, root1, 0, 1

        // Bounds: |data{i}| < 128q

        // Layer 1
        gs_butterfly data0, data8, root0, 0, 1
        gs_butterfly data1, data9, root0, 0, 1
        gs_butterfly data2, data10, root0, 0, 1
        gs_butterfly data3, data11, root0, 0, 1
        gs_butterfly data4, data12, root0, 0, 1
        gs_butterfly data5, data13, root0, 0, 1
        gs_butterfly data6, data14, root0, 0, 1
        gs_butterfly data7, data15, root0, 0, 1

        // Bounds: |data{i}| < 256q         for i in 0...7
        //                   < BARMUL_BOUND for i in 8...15

        str q_data8, [in, #(8*(512/8))]
        str q_data9, [in, #(9*(512/8))]
        str q_data10, [in, #(10*(512/8))]
        str q_data11, [in, #(11*(512/8))]
        str q_data12, [in, #(12*(512/8))]
        str q_data13, [in, #(13*(512/8))]
        str q_data14, [in, #(14*(512/8))]
        str q_data15, [in, #(15*(512/8))]

        // Scale by 1/256*R
        // For second half, the scaling has been merged into the last butterfly,
        // for the other half, we scale explicitly.
        mul_ninv data0, data1, data2, data3, data0, data1, data2, data3
        mul_ninv data4, data5, data6, data7, data4, data5, data6, data7

        // Bounds: |data{i}| < BARMUL_BOUND for i in 0...7

        // In the following we show that |barmul(a, b)| < BARMUL_BOUND := q,
        // given that a fits inside int32_t (thus |a| <= R/2) and b is signed
        // canonical (in the range -(Q-1)/2...(Q-1)/2).
        //
        // (It's worth noting that if we allow BARMUL_BOUND to be parametric in
        // a (more precisely, range of |a|) and/or b, it's possible to tighten
        // the bound to be considerably smaller than q.)
        //
        // By Corollary 2 of @[NeonNTT], we have
        //
        //   |barmul(a, b)| <= |a| |bR mod⟦⟧ q| / R + q / 2,
        //
        // where ⟦⟧ denotes round-to-nearest-even-integer approximation (thus
        // |bR mod⟦⟧ q| < q). From this, simple computation gives
        // |barmul(a, b)| < q.
        //
        // See test/test_bounds.py for more empirical evidence.

        str q_data0, [in], #(16)
        str q_data1, [in, #(-16 + 1*(512/8))]
        str q_data2, [in, #(-16 + 2*(512/8))]
        str q_data3, [in, #(-16 + 3*(512/8))]
        str q_data4, [in, #(-16 + 4*(512/8))]
        str q_data5, [in, #(-16 + 5*(512/8))]
        str q_data6, [in, #(-16 + 6*(512/8))]
        str q_data7, [in, #(-16 + 7*(512/8))]

        subs count, count, #1
        cbnz count, intt_layer1234_start

        pop_stack
        ret

/****************** REGISTER DEALLOCATIONS *******************/
    .unreq in
    .unreq r78_ptr
    .unreq r123456_ptr
    .unreq inp
    .unreq count
    .unreq wtmp
    .unreq data0
    .unreq data1
    .unreq data2
    .unreq data3
    .unreq data4
    .unreq data5
    .unreq data6
    .unreq data7
    .unreq data8
    .unreq data9
    .unreq data10
    .unreq data11
    .unreq data12
    .unreq data13
    .unreq data14
    .unreq data15
    .unreq q_data0
    .unreq q_data1
    .unreq q_data2
    .unreq q_data3
    .unreq q_data4
    .unreq q_data5
    .unreq q_data6
    .unreq q_data7
    .unreq q_data8
    .unreq q_data9
    .unreq q_data10
    .unreq q_data11
    .unreq q_data12
    .unreq q_data13
    .unreq q_data14
    .unreq q_data15
    .unreq root0
    .unreq root1
    .unreq root2
    .unreq root3
    .unreq root4
    .unreq root5
    .unreq root6
    .unreq root7
    .unreq root0_tw
    .unreq root1_tw
    .unreq root2_tw
    .unreq root3_tw
    .unreq consts
    .unreq q_consts
    .unreq q_root0
    .unreq q_root1
    .unreq q_root2
    .unreq q_root3
    .unreq q_root4
    .unreq q_root5
    .unreq q_root6
    .unreq q_root7
    .unreq q_root0_tw
    .unreq q_root1_tw
    .unreq q_root2_tw
    .unreq q_root3_tw
    .unreq d_root1
    .unreq tmp
    .unreq t0
    .unreq t1
    .unreq t2
    .unreq t3
    .unreq ninv
    .unreq ninv_tw

/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
