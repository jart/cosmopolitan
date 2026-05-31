/*
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

 #include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

.macro trim_map_19 a
        // Keep only 20 out of 24 bits in each 32-bit lane
        //     Lane     0       1       2       3
        //     Bits     0..23   16..39  40..63  56..79
        ushl \a\().4s, \a\().4s, shifts.4s
        //     Bits     0..23   20..39  40..63  60..79
        and \a\().16b, \a\().16b, mask.16b
        //     Bits     0..19   20..39  40..59  60..79

        // Map [0, 1, ..., 2^20-1] to [2^19, 2^19-1, ..., -2^19+1]
        sub \a\().4s, gamma1.4s, \a\().4s
.endm

        /* Parameters */
        output          .req x0
        buf             .req x1
        indices         .req x2

        xtmp            .req x3
        count           .req x9

        /* Constant register assignments */
        idx0            .req v24
        idx1            .req v25
        idx2            .req v26
        idx3            .req v27
        shifts          .req v28
        mask            .req v29    // 2^20 - 1
        gamma1          .req v30    // 2^19

.text
.global MLD_ASM_NAMESPACE(polyz_unpack_19_asm)
.balign 4
MLD_ASM_FN_SYMBOL(polyz_unpack_19_asm)
        // Load indices
        ldr q24, [indices]
        ldr q25, [indices, #1*16]
        ldr q26, [indices, #2*16]
        ldr q27, [indices, #3*16]

        // Load per-lane shifts 0, -4, 0, -4. (Negative means right shift.)
        // The shifts for the 4 32-bit lanes are sign-extended from the lowest
        // 8 bits, so it suffices to set up only byte 0, 4, 8, 12.
        movz xtmp, 0xfc, lsl 32
        dup shifts.2d, xtmp

        movi mask.4s, 0xf, msl 16

        movi gamma1.4s, 0x8, lsl 16

        mov count, #(64/4)

polyz_unpack_19_loop:
        ldr q1, [buf, #16]
        ldr d2, [buf, #32]
        ldr q0, [buf], #40

        tbl v4.16b, {v0.16b}, idx0.16b
        tbl v5.16b, {v0.16b - v1.16b}, idx1.16b
        tbl v6.16b, {v1.16b}, idx2.16b
        tbl v7.16b, {v1.16b - v2.16b}, idx3.16b

        trim_map_19 v4
        trim_map_19 v5
        trim_map_19 v6
        trim_map_19 v7

        str q5, [output, #1*16]
        str q6, [output, #2*16]
        str q7, [output, #3*16]
        str q4, [output], #4*16

        subs count, count, #1
        bne polyz_unpack_19_loop

        ret

        .unreq output
        .unreq buf
        .unreq indices
        .unreq xtmp
        .unreq count
        .unreq idx0
        .unreq idx1
        .unreq idx2
        .unreq idx3
        .unreq shifts
        .unreq mask
        .unreq gamma1
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
