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

/* AArch64 ML-DSA forward NTT following @[NeonNTT] and @[SLOTHY_Paper] */

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
        // (See the end of dev/aarch64_clean/src/intt.S for the exact value of
        // BARMUL_BOUND)
.endm

.macro mulmod dst, src, const, const_twisted
        // Same as mulmodq, but with different multiplier across each lanes.
        sqrdmulh t2.4s,   \src\().4s, \const_twisted\().4s
        mul      \dst\().4s, \src\().4s, \const\().4s
        mls      \dst\().4s, t2.4s,   consts.s[0]
        // Bounds: |dst| < BARMUL_BOUND
.endm

.macro ct_butterfly a, b, root, idx0, idx1
        // Compute a' = a + barmul(b, zeta), b' = a - barmul(b, zeta), commonly
        // known as the Cooley-Tukey butterfly.
        mulmodq tmp, \b, \root, \idx0, \idx1
        sub \b\().4s, \a\().4s, tmp.4s
        add \a\().4s, \a\().4s, tmp.4s
        // Bounds: |a'|, |b'| < |a| + BARMUL_BOUND <= |a| + q
        //
        // In conclusion, the magnitudes of all coefficients grow by at most q
        // after each layer.
.endm

.macro ct_butterfly_v a, b, root, root_twisted
        // Same as ct_butterfly, but with different zetas across each lanes.
        mulmod tmp, \b, \root, \root_twisted
        sub \b\().4s, \a\().4s, tmp.4s
        add \a\().4s, \a\().4s, tmp.4s
        // Bounds: |a'|, |b'| < |a| + BARMUL_BOUND <= |a| + q
.endm

.macro load_roots_123
        ldr q_root0, [r012345_ptr], #(4*16)
        ldr q_root1, [r012345_ptr, #(-4*16 + 1*16)]
        ldr q_root2, [r012345_ptr, #(-4*16 + 2*16)]
        ldr q_root3, [r012345_ptr, #(-4*16 + 3*16)]
.endm

.macro load_next_roots_456
       ldr q_root0, [r012345_ptr], #(4*16)
       ldr q_root1, [r012345_ptr, #(-4*16 + 1*16)]
       ldr q_root2, [r012345_ptr, #(-4*16 + 2*16)]
       ldr q_root3, [r012345_ptr, #(-4*16 + 3*16)]
.endm

.macro load_next_roots_78_1
        ldr q_root0,    [r67_ptr], #(12*16)
        ldr q_root0_tw, [r67_ptr, #(-12*16 + 1*16)]
        ldr q_root1,    [r67_ptr, #(-12*16 + 2*16)]
        ldr q_root1_tw, [r67_ptr, #(-12*16 + 3*16)]
        ldr q_root2,    [r67_ptr, #(-12*16 + 4*16)]
        ldr q_root2_tw, [r67_ptr, #(-12*16 + 5*16)]
.endm

.macro load_next_roots_78_2
        ldr q_root0,    [r67_ptr, #(-12*16 + 6*16)]
        ldr q_root0_tw, [r67_ptr, #(-12*16 + 7*16)]
        ldr q_root1,    [r67_ptr, #(-12*16 + 8*16)]
        ldr q_root1_tw, [r67_ptr, #(-12*16 + 9*16)]
        ldr q_root2,    [r67_ptr, #(-12*16 + 10*16)]
        ldr q_root2_tw, [r67_ptr, #(-12*16 + 11*16)]
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

        // Arguments
        in          .req x0  // Input/output buffer
        r012345_ptr .req x1  // twiddles for layer 0,1,2,3,4,5
        r67_ptr     .req x2  // twiddles for layer 6,7

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

        q_data0  .req q8
        q_data1  .req q9
        q_data2  .req q10
        q_data3  .req q11
        q_data4  .req q12
        q_data5  .req q13
        q_data6  .req q14
        q_data7  .req q15

        root0    .req v0
        root1    .req v1
        root2    .req v2
        root3    .req v3
        root0_tw .req v4
        root1_tw .req v5
        root2_tw .req v6

        q_root0    .req q0
        q_root1    .req q1
        q_root2    .req q2
        q_root3    .req q3
        q_root0_tw .req q4
        q_root1_tw .req q5
        q_root2_tw .req q6

        consts    .req v7

        tmp .req v24
        t0  .req v25
        t1  .req v26
        t2  .req v27
        t3  .req v28

        .text
        .global MLD_ASM_NAMESPACE(ntt_asm)
        .balign 4
MLD_ASM_FN_SYMBOL(ntt_asm)
        push_stack

        // Load MLDSA_Q = 8380417
        movz wtmp, #0xe001
        movk wtmp, #0x7f, lsl #16
        dup consts.4s, wtmp

        mov inp, in
        mov count, #8

        load_roots_123

        .p2align 2

ntt_layer123_start:

        ldr q_data0, [in, #0]
        ldr q_data1, [in, #(1*(1024/8))]
        ldr q_data2, [in, #(2*(1024/8))]
        ldr q_data3, [in, #(3*(1024/8))]
        ldr q_data4, [in, #(4*(1024/8))]
        ldr q_data5, [in, #(5*(1024/8))]
        ldr q_data6, [in, #(6*(1024/8))]
        ldr q_data7, [in, #(7*(1024/8))]

        ct_butterfly data0, data4, root0, 0, 1
        ct_butterfly data1, data5, root0, 0, 1
        ct_butterfly data2, data6, root0, 0, 1
        ct_butterfly data3, data7, root0, 0, 1
        // Bounds: |data{i}| < 2q

        ct_butterfly data0, data2, root0, 2, 3
        ct_butterfly data1, data3, root0, 2, 3
        ct_butterfly data4, data6, root1, 0, 1
        ct_butterfly data5, data7, root1, 0, 1
        // Bounds: |data{i}| < 3q

        ct_butterfly data0, data1, root1, 2, 3
        ct_butterfly data2, data3, root2, 0, 1
        ct_butterfly data4, data5, root2, 2, 3
        ct_butterfly data6, data7, root3, 0, 1
        // Bounds: |data{i}| < 4q

        str q_data0, [in], #(16)
        str q_data1, [in, #(-16 + 1*(1024/8))]
        str q_data2, [in, #(-16 + 2*(1024/8))]
        str q_data3, [in, #(-16 + 3*(1024/8))]
        str q_data4, [in, #(-16 + 4*(1024/8))]
        str q_data5, [in, #(-16 + 5*(1024/8))]
        str q_data6, [in, #(-16 + 6*(1024/8))]
        str q_data7, [in, #(-16 + 7*(1024/8))]

        subs count, count, #1
        cbnz count, ntt_layer123_start

        mov in, inp
        mov count, #8

        .p2align 2
ntt_layer45678_start:

        ldr q_data0, [in, #(16*0)]
        ldr q_data1, [in, #(16*1)]
        ldr q_data2, [in, #(16*2)]
        ldr q_data3, [in, #(16*3)]
        ldr q_data4, [in, #(16*4)]
        ldr q_data5, [in, #(16*5)]
        ldr q_data6, [in, #(16*6)]
        ldr q_data7, [in, #(16*7)]

        load_next_roots_456

        ct_butterfly data0, data4, root0, 0, 1
        ct_butterfly data1, data5, root0, 0, 1
        ct_butterfly data2, data6, root0, 0, 1
        ct_butterfly data3, data7, root0, 0, 1
        // Bounds: |data{i}| < 5q

        ct_butterfly data0, data2, root0, 2, 3
        ct_butterfly data1, data3, root0, 2, 3
        ct_butterfly data4, data6, root1, 0, 1
        ct_butterfly data5, data7, root1, 0, 1
        // Bounds: |data{i}| < 6q

        ct_butterfly data0, data1, root1, 2, 3
        ct_butterfly data2, data3, root2, 0, 1
        ct_butterfly data4, data5, root2, 2, 3
        ct_butterfly data6, data7, root3, 0, 1
        // Bounds: |data{i}| < 7q

        transpose4 data0, data1, data2, data3
        transpose4 data4, data5, data6, data7
        load_next_roots_78_1

        ct_butterfly_v data0, data2, root0, root0_tw
        ct_butterfly_v data1, data3, root0, root0_tw
        ct_butterfly_v data0, data1, root1, root1_tw
        ct_butterfly_v data2, data3, root2, root2_tw
        // Bounds: |data{i}| < 8q

        load_next_roots_78_2

        ct_butterfly_v data4, data6, root0, root0_tw
        ct_butterfly_v data5, data7, root0, root0_tw
        ct_butterfly_v data4, data5, root1, root1_tw
        ct_butterfly_v data6, data7, root2, root2_tw
        // Bounds: |data{i}| < 9q

        transpose4 data0, data1, data2, data3
        transpose4 data4, data5, data6, data7

        str q_data0, [in], #(16*8)
        str q_data1, [in, #(-16*7)]
        str q_data2, [in, #(-16*6)]
        str q_data3, [in, #(-16*5)]
        str q_data4, [in, #(-16*4)]
        str q_data5, [in, #(-16*3)]
        str q_data6, [in, #(-16*2)]
        str q_data7, [in, #(-16*1)]

        subs count, count, #1
        cbnz count, ntt_layer45678_start

        pop_stack
        ret

/****************** REGISTER DEALLOCATIONS *******************/
    .unreq in
    .unreq r012345_ptr
    .unreq r67_ptr
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
    .unreq q_data0
    .unreq q_data1
    .unreq q_data2
    .unreq q_data3
    .unreq q_data4
    .unreq q_data5
    .unreq q_data6
    .unreq q_data7
    .unreq root0
    .unreq root1
    .unreq root2
    .unreq root3
    .unreq root0_tw
    .unreq root1_tw
    .unreq root2_tw
    .unreq q_root0
    .unreq q_root1
    .unreq q_root2
    .unreq q_root3
    .unreq q_root0_tw
    .unreq q_root1_tw
    .unreq q_root2_tw
    .unreq consts
    .unreq tmp
    .unreq t0
    .unreq t1
    .unreq t2
    .unreq t3

/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
