/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

// a aliased with a0
.macro decompose88 a1, a, temp
        // Compute a1 = round-(a / 190464) ≈ round(a * 1477838209 /
        // 2^48), where round-() denotes "round half down". This is
        // exact for 0 <= a < Q. Note that half is rounded down since
        // 1477838209 / 2^48 ≲ 1 / 190464.
        sqdmulh \a1\().4s, \a\().4s, barrett_const.4s
        srshr \a1\().4s, \a1\().4s, #17

        // If a1 = 44, i.e. a > 87*GAMMA2, proceed as if a' = a - Q was
        // given instead. (For a = 87*GAMMA2 + 1 thus a' = -GAMMA2, we
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
.macro use_hint88 b, a, h, temp
        decompose88 \b, \a, \temp

        // delta = (a0 <= 0) ? -1 : 1
        cmle \a\().4s, \a\().4s, #0
        orr \a\().4s, #1

        // b = (b + delta * h) % 44
        mla \b\().4s, \a\().4s, \h\().4s
        cmgt \temp\().4s, \b\().4s, const_43.4s
        bic \b\().16b, \b\().16b, \temp\().16b
        umin \b\().4s, \b\().4s, const_43.4s
.endm

        /* Parameters */
        b_ptr           .req x0     // Output polynomial
        a_ptr           .req x1     // Input polynomial
        h_ptr           .req x2     // Input hints

        count           .req x3

        /* Constant register assignments */
        q               .req v20    // Q = 8380417
        q_bound         .req v21    // 87*GAMMA2 = 8285184
        gamma2_2x       .req v22    // 2*GAMMA2 = 190464
        barrett_const   .req v23    // Barrett constant = 1477838209
        const_43        .req v24    // mask = 43

.text
.global MLD_ASM_NAMESPACE(poly_use_hint_88_asm)
.balign 4
MLD_ASM_FN_SYMBOL(poly_use_hint_88_asm)
        // Load constants into SIMD registers
        movz w4, #57345
        movk w4, #127, lsl #16
        dup q.4s, w4

        movz w5, #0x6c00
        movk w5, #0x7e, lsl #16
        dup q_bound.4s, w5

        movz w7, #0xe800
        movk w7, #0x2, lsl #16
        dup gamma2_2x.4s, w7

        movz w11, #0x0581
        movk w11, #0x5816, lsl #16
        dup barrett_const.4s, w11

        movi const_43.4s, #43

        mov count, #(64/4)

poly_use_hint_88_loop:
        ldr q1, [a_ptr, #1*16]
        ldr q2, [a_ptr, #2*16]
        ldr q3, [a_ptr, #3*16]
        ldr q0, [a_ptr], #4*16

        ldr q5, [h_ptr, #1*16]
        ldr q6, [h_ptr, #2*16]
        ldr q7, [h_ptr, #3*16]
        ldr q4, [h_ptr], #4*16

        use_hint88 v17, v1, v5, v25
        use_hint88 v18, v2, v6, v25
        use_hint88 v19, v3, v7, v25
        use_hint88 v16, v0, v4, v25

        str q17, [b_ptr, #1*16]
        str q18, [b_ptr, #2*16]
        str q19, [b_ptr, #3*16]
        str q16, [b_ptr], #4*16

        subs count, count, #1
        bne poly_use_hint_88_loop

        ret

        .unreq b_ptr
        .unreq a_ptr
        .unreq h_ptr
        .unreq count
        .unreq q
        .unreq q_bound
        .unreq gamma2_2x
        .unreq barrett_const
        .unreq const_43
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
