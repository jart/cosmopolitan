/* Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

.macro montgomery_reduce_long res, inl, inh
    uzp1   \res\().4s, \inl\().4s, \inh\().4s
    mul    \res\().4s, \res\().4s, modulus_twisted.4s
    smlsl  \inl\().2d, \res\().2s, modulus.2s
    smlsl2 \inh\().2d, \res\().4s, modulus.4s
    uzp2   \res\().4s, \inl\().4s, \inh\().4s
.endm

.macro load_polys p0, p1, p2, p3, ptr, idx
.if \idx == 0
    ldr \p1, [\ptr, #1*16]
    ldr \p2, [\ptr, #2*16]
    ldr \p3, [\ptr, #3*16]
    ldr \p0, [\ptr], #4*16
.else
    ldr \p0, [\ptr, #(1024*\idx-4*16)]
    ldr \p1, [\ptr, #(1024*\idx-3*16)]
    ldr \p2, [\ptr, #(1024*\idx-2*16)]
    ldr \p3, [\ptr, #(1024*\idx-1*16)]
.endif
.endm

.macro pmull dl, dh, a, b
    smull  \dl\().2d, \a\().2s, \b\().2s
    smull2 \dh\().2d, \a\().4s, \b\().4s
.endm

.macro pmlal dl, dh, a, b
    smlal  \dl\().2d, \a\().2s, \b\().2s
    smlal2 \dh\().2d, \a\().4s, \b\().4s
.endm

    out_ptr .req x0
    a_ptr   .req x1
    b_ptr   .req x2
    count   .req x3
    wtmp    .req w3

    modulus         .req v0
    modulus_twisted .req v1

    a_0      .req v16
    a_1      .req v17
    a_2      .req v18
    a_3      .req v19

    b_0      .req v20
    b_1      .req v21
    b_2      .req v22
    b_3      .req v23

    c_0_lo   .req v24
    c_0_hi   .req v25
    c_1_lo   .req v26
    c_1_hi   .req v27
    c_2_lo   .req v28
    c_2_hi   .req v29
    c_3_lo   .req v30
    c_3_hi   .req v31

    c_0      .req v16
    c_1      .req v17
    c_2      .req v18
    c_3      .req v19

    q_a_0    .req q16
    q_a_1    .req q17
    q_a_2    .req q18
    q_a_3    .req q19

    q_b_0    .req q20
    q_b_1    .req q21
    q_b_2    .req q22
    q_b_3    .req q23

    q_c_0    .req q16
    q_c_1    .req q17
    q_c_2    .req q18
    q_c_3    .req q19

.text
.global MLD_ASM_NAMESPACE(polyvecl_pointwise_acc_montgomery_l7_asm)
.balign 4
MLD_ASM_FN_SYMBOL(polyvecl_pointwise_acc_montgomery_l7_asm)
    // Load MLDSA_Q = 8380417
    movz wtmp, #0xe001
    movk wtmp, #0x7f, lsl #16
    dup modulus.4s, wtmp

    /* check-magic: 58728449 == unsigned_mod(pow(MLDSA_Q, -1, 2**32), 2**32) */
    // Load MLDSA_Q^-1 = 58728449
    movz wtmp, #0x2001
    movk wtmp, #0x380, lsl #16
    dup modulus_twisted.4s, wtmp

    mov count, #(MLDSA_N / 4)

polyvecl_pointwise_acc_montgomery_l7_loop_start:
    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 0
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 0
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmull c_0_lo, c_0_hi, a_0, b_0
    pmull c_1_lo, c_1_hi, a_1, b_1
    pmull c_2_lo, c_2_hi, a_2, b_2
    pmull c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 9q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 1
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 1
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 18q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 2
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 2
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 27q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 3
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 3
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 36q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 4
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 4
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 45q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 5
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 5
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 54q^2

    load_polys q_a_0, q_a_1, q_a_2, q_a_3, a_ptr, 6
    load_polys q_b_0, q_b_1, q_b_2, q_b_3, b_ptr, 6
    // Bounds: |a_{i}| < q, |b_{i}| < 9q

    pmlal c_0_lo, c_0_hi, a_0, b_0
    pmlal c_1_lo, c_1_hi, a_1, b_1
    pmlal c_2_lo, c_2_hi, a_2, b_2
    pmlal c_3_lo, c_3_hi, a_3, b_3
    // Bounds: |c_{i}_lo|, |c_{i}_hi| < 63q^2 < qR/2

    montgomery_reduce_long c_0, c_0_lo, c_0_hi
    montgomery_reduce_long c_1, c_1_lo, c_1_hi
    montgomery_reduce_long c_2, c_2_lo, c_2_hi
    montgomery_reduce_long c_3, c_3_lo, c_3_hi
    // All coefficients are Montgomery-reduced, resulting in
    //
    // Bounds: |c_{i}| < q
    //
    // See description of mld_montgomery_reduce() in mldsa/src/reduce.h.

    str q_c_1, [out_ptr, #1*16]
    str q_c_2, [out_ptr, #2*16]
    str q_c_3, [out_ptr, #3*16]
    str q_c_0, [out_ptr], #4*16

    subs count, count, #4
    cbnz count, polyvecl_pointwise_acc_montgomery_l7_loop_start

    ret

    .unreq out_ptr
    .unreq a_ptr
    .unreq b_ptr
    .unreq count
    .unreq wtmp
    .unreq modulus
    .unreq modulus_twisted
    .unreq a_0
    .unreq a_1
    .unreq a_2
    .unreq a_3
    .unreq b_0
    .unreq b_1
    .unreq b_2
    .unreq b_3
    .unreq c_0_lo
    .unreq c_0_hi
    .unreq c_1_lo
    .unreq c_1_hi
    .unreq c_2_lo
    .unreq c_2_hi
    .unreq c_3_lo
    .unreq c_3_hi
    .unreq c_0
    .unreq c_1
    .unreq c_2
    .unreq c_3
    .unreq q_a_0
    .unreq q_a_1
    .unreq q_a_2
    .unreq q_a_3
    .unreq q_b_0
    .unreq q_b_1
    .unreq q_b_2
    .unreq q_b_3
    .unreq q_c_0
    .unreq q_c_1
    .unreq q_c_2
    .unreq q_c_3

/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
