/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

// a aliased with a0
.macro decompose32 a1, a, temp
        // Compute a1 = round-(a / 523776) ≈ round(a * 1074791425 /
        // 2^49), where round-() denotes "round half down". This is
        // exact for 0 <= a < Q. Note that half is rounded down since
        // 1074791425 / 2^49 ≲ 1 / 523776.
        sqdmulh \a1\().4s, \a\().4s, barrett_const.4s
        srshr \a1\().4s, \a1\().4s, #18

        // If a1 = 16, i.e. a > 31*GAMMA2, proceed as if a' = a - Q was
        // given instead. (For a = 31*GAMMA2 + 1 thus a' = -GAMMA2, we
        // still round it to 0 like other "wrapped around" cases.)

        // Check for wrap-around
        cmgt \temp\().4s, \a\().4s, q_bound.4s

        // Compute remainder a0
        mls \a\().4s, \a1\().4s, gamma2_2x.4s

        // If wrap-around is required, set a1 = 0 and a0 -= 1
        bic \a1\().16b, \a1\().16b, \temp\().16b
        add \a\().4s, \a\().4s, \temp\().4s
.endm

// a aliased with delta
.macro use_hint32 b, a, h, temp
        decompose32 \b, \a, \temp

        // delta = (a0 <= 0) ? -1 : 1
        cmle \a\().4s, \a\().4s, #0
        orr \a\().4s, #1

        // b = (b + delta * h) % 16
        mla \b\().4s, \a\().4s, \h\().4s
        and \b\().16b, \b\().16b, mask_15.16b
.endm

        /* Parameters */
        b_ptr           .req x0     // Output polynomial
        a_ptr           .req x1     // Input polynomial
        h_ptr           .req x2     // Input hints

        count           .req x3

        /* Constant register assignments */
        q               .req v20    // Q = 8380417
        q_bound         .req v21    // 31*GAMMA2 = 8118528
        gamma2_2x       .req v22    // 2*GAMMA2 = 523776
        barrett_const   .req v23    // Barrett constant = 1074791425
        mask_15         .req v24    // mask = 15

.text
.global MLD_ASM_NAMESPACE(poly_use_hint_32_asm)
.balign 4
MLD_ASM_FN_SYMBOL(poly_use_hint_32_asm)
        // Load constants into SIMD registers
        movz w4, #57345
        movk w4, #127, lsl #16
        dup q.4s, w4

        movz w5, #0xe100
        movk w5, #0x7b, lsl #16
        dup q_bound.4s, w5

        movz w7, #0xfe00
        movk w7, #7, lsl #16
        dup gamma2_2x.4s, w7

        movz w11, #0x0401
        movk w11, #0x4010, lsl #16
        dup barrett_const.4s, w11

        movi mask_15.4s, #15

        mov count, #(64/4)

poly_use_hint_32_loop:
        ldr q1, [a_ptr, #1*16]
        ldr q2, [a_ptr, #2*16]
        ldr q3, [a_ptr, #3*16]
        ldr q0, [a_ptr], #4*16

        ldr q5, [h_ptr, #1*16]
        ldr q6, [h_ptr, #2*16]
        ldr q7, [h_ptr, #3*16]
        ldr q4, [h_ptr], #4*16

        use_hint32 v17, v1, v5, v25
        use_hint32 v18, v2, v6, v25
        use_hint32 v19, v3, v7, v25
        use_hint32 v16, v0, v4, v25

        str q17, [b_ptr, #1*16]
        str q18, [b_ptr, #2*16]
        str q19, [b_ptr, #3*16]
        str q16, [b_ptr], #4*16

        subs count, count, #1
        bne poly_use_hint_32_loop

        ret

        .unreq b_ptr
        .unreq a_ptr
        .unreq h_ptr
        .unreq count
        .unreq q
        .unreq q_bound
        .unreq gamma2_2x
        .unreq barrett_const
        .unreq mask_15
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
