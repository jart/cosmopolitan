/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

.macro caddq inout
        ushr tmp.4s, \inout\().4s, #31
        mla \inout\().4s, tmp.4s, q_reg.4s
.endm

.global MLD_ASM_NAMESPACE(poly_caddq_asm)
.balign 4
MLD_ASM_FN_SYMBOL(poly_caddq_asm)
        // Function signature: void mld_poly_caddq_asm(int32_t *a)
        // x0: pointer to polynomial coefficients

        // Register assignments
        a_ptr           .req x0
        count           .req x1
        q_reg           .req v4
        tmp             .req v5

        // Load constants
        // MLDSA_Q = 8380417 = 0x7FE001
        movz w9, #0xE001
        movk w9, #0x7F, lsl #16
        dup q_reg.4s, w9        // Load Q values

        mov count, #64/4
poly_caddq_loop:
        ldr q0, [a_ptr, #0*16]
        ldr q1, [a_ptr, #1*16]
        ldr q2, [a_ptr, #2*16]
        ldr q3, [a_ptr, #3*16]

        caddq v0
        caddq v1
        caddq v2
        caddq v3

        str q1, [a_ptr, #1*16]
        str q2, [a_ptr, #2*16]
        str q3, [a_ptr, #3*16]
        str q0, [a_ptr], #4*16

        subs count, count, #1
        bne poly_caddq_loop

        ret

        .unreq a_ptr
        .unreq count
        .unreq q_reg
        .unreq tmp
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
