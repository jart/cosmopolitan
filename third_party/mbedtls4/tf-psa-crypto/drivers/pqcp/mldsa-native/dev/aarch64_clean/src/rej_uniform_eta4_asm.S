/*
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

#include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_AARCH64) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

// We save the output on the stack first, and copy to the actual
// output buffer only in the end. This is because the main loop can overwrite
// by up to 60 bytes, which we account for here (we use 64 bytes for alignment).
#define MLD_STACK_SIZE (2*MLDSA_N + 64)

.macro push_stack
        sub sp, sp, #MLD_STACK_SIZE
.endm

.macro pop_stack
        add sp, sp, #MLD_STACK_SIZE
.endm

    /* Parameters */
    output                      .req x0
    buf                         .req x1
    buflen                      .req x2
    table_idx                   .req x3

    len                         .req x4

    /* Temporary output on the stack */
    xtmp                        .req x7
    wtmp                        .req w7
    output_tmp                  .req x7
    output_tmp_base             .req x8

    /* Number of coefficients sampled so far */
    count                       .req x9
    buf_consumed                .req x10

    /* Temporary registers */
    tmp                         .req w11
    initial_zero_count          .req x11
    final_copy_count            .req x11

    rec_idx_0                   .req x12
    rec_idx_1                   .req x13

    rec_idx_0_w                 .req w12
    rec_idx_1_w                 .req w13

    ctr0                        .req x12
    ctr1                        .req x13

    ctr0_w                      .req w12
    ctr1_w                      .req w13

    ctr01                       .req ctr0

    /* Vector registers */
    buf0                        .req v0

    tmp0                        .req v26
    tmp1                        .req v27
    tmp2                        .req v28
    tmp3                        .req v29

    sign0                       .req v4
    sign1                       .req v5
    const4                      .req v7


    val0                        .req v16
    val0q                       .req q16
    val1                        .req v17
    val1q                       .req q17
    val2                        .req v18
    val2q                       .req q18
    val3                        .req v19
    val3q                       .req q19

    t0                          .req s20
    t1                          .req s21

    table0                      .req v24
    table0q                     .req q24
    table1                      .req v25
    table1q                     .req q25

    eta_bound                   .req v30
    bits                        .req v31

    .text
    .global MLD_ASM_NAMESPACE(rej_uniform_eta4_asm)
    .balign 4
MLD_ASM_FN_SYMBOL(rej_uniform_eta4_asm)
    push_stack

    // Load 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80
    movz xtmp, 0x1
    movk xtmp, 0x2, lsl 16
    movk xtmp, 0x4, lsl 32
    movk xtmp, 0x8, lsl 48
    mov bits.d[0], xtmp

    movz xtmp, 0x10
    movk xtmp, 0x20, lsl 16
    movk xtmp, 0x40, lsl 32
    movk xtmp, 0x80, lsl 48
    mov bits.d[1], xtmp

    // Load eta4 bound = 9
    movi eta_bound.8h, #9
    movi const4.8h, #4

    mov output_tmp_base, sp
    mov output_tmp, output_tmp_base

    // The entire temporary stack buffer is copied to the output buffer
    // at the end of this routine. To avoid leaking original stack contents
    // in case not enough bytes have been sampled, zero the temporary buffer.
    // The temporary buffer holds 16-bit values that are expanded to 32-bit
    // on copy out
    mov initial_zero_count, #0
    eor val0.16b, val0.16b, val0.16b
rej_uniform_eta4_initial_zero:
        str val0q, [output_tmp], #64
        str val0q, [output_tmp, #-48]
        str val0q, [output_tmp, #-32]
        str val0q, [output_tmp, #-16]
        add initial_zero_count, initial_zero_count, #32
        cmp initial_zero_count, #MLDSA_N
        b.lt rej_uniform_eta4_initial_zero

    mov output_tmp, output_tmp_base

    mov count, #0
    mov len, #MLDSA_N

rej_uniform_eta4_loop8:
        // Finish once we've generated sufficiently many coefficients
        cmp     count, len
        b.hs    rej_uniform_eta4_memory_copy

        // Load 8 bytes and extract nibbles to get 16 4-bit values
        sub     buflen, buflen, #8
        ld1     {buf0.8b}, [buf], #8

        // Extract nibbles
        movi    tmp0.8b, #0x0F
        and     tmp1.8b, buf0.8b, tmp0.8b     // Low nibbles [L0, L1, L2, L3, L4, L5, L6, L7]
        ushr    tmp2.8b, buf0.8b, #4          // High nibbles [H0, H1, H2, H3, H4, H5, H6, H7]

        // Interleave low and high nibbles: L0,H0,L1,H1,L2,H2,L3,H3,...
        zip1    tmp0.8b, tmp1.8b, tmp2.8b     // First 8 nibbles interleaved [L0,H0,L1,H1,L2,H2,L3,H3]
        zip2    tmp3.8b, tmp1.8b, tmp2.8b     // Next 8 nibbles interleaved [L4,H4,L5,H5,L6,H6,L7,H7]

        // Convert to 16-bit values
        uxtl    val0.8h, tmp0.8b
        uxtl    val1.8h, tmp3.8b

        // At this point, val0-val1 are the signed integers to do rejection
        // sampling on. For each of them, do the following:
        // - Check which coefficients are within range, and represent the set
        //   of lane-indices of those coefficients as an 8-bit bitmap.
        // - Move the respective lanes to the front of the vector. This is the
        //   most complex part, and is done by interpreting the 8-bit bitmap as
        //   an index into a lookup table giving the lane-table to be use for
        //   the `tbl` instruction.
        // - Write the vector to the output buffer, but merely increase the output
        //   buffer pointer by the number of valid coefficients.

        // Check which coefficients are within range (< 9)
        cmhi    sign0.8h, eta_bound.8h, val0.8h
        cmhi    sign1.8h, eta_bound.8h, val1.8h

        // If lane i is valid and has value -1, retain only i-th bit
        and     sign0.16b, sign0.16b, bits.16b
        and     sign1.16b, sign1.16b, bits.16b

        // Get 8-bit bitmap of valid lane indices by adding lanes
        uaddlv  t0, sign0.8h
        uaddlv  t1, sign1.8h

        fmov    rec_idx_0_w, t0
        fmov    rec_idx_1_w, t1

        ldr     table0q, [table_idx, rec_idx_0, lsl #4]
        ldr     table1q, [table_idx, rec_idx_1, lsl #4]

        // Compute number of valid coefficients. Recall that at this
        // point, lane i has value 2^i (hence popcount 1) if its coefficient
        // is valid, and 0 otherwise.
        cnt     sign0.16b, sign0.16b
        cnt     sign1.16b, sign1.16b

        // Extract number of valid coefficients
        uaddlv  t0, sign0.8h
        uaddlv  t1, sign1.8h

        fmov    ctr0_w, t0
        fmov    ctr1_w, t1

        // Move valid coefficients to the front
        tbl     val0.16b, {val0.16b}, table0.16b
        tbl     val1.16b, {val1.16b}, table1.16b


        // We store 16-bit coefficients here. They will be expanded to 32-bit
        // on copy out
        str     val0q, [output_tmp]
        add     output_tmp, output_tmp, ctr0, lsl #1

        str     val1q, [output_tmp]
        add     output_tmp, output_tmp, ctr1, lsl #1

        add     ctr01, ctr0, ctr1
        add     count, count, ctr01

        cmp buflen, #8
        b.hs rej_uniform_eta4_loop8

rej_uniform_eta4_memory_copy:
    // min = min(count,len)
    cmp  count, len
    csel count, count, len, lo

    // Always copy MLDSA_N coefficients from the stack to the destination
    mov final_copy_count, #0
    mov output_tmp, output_tmp_base
rej_uniform_eta4_final_copy:
        ldr val0q, [output_tmp], #32
        ldr val2q, [output_tmp, #-16]

        // Apply eta4 transformation: 4 - nibble
        sub     val0.8h, const4.8h, val0.8h
        sub     val2.8h, const4.8h, val2.8h

        // Expand from 16-bit to 32-bit
        sxtl2 val1.4s, val0.8h
        sxtl  val0.4s, val0.4h

        sxtl2 val3.4s, val2.8h
        sxtl  val2.4s, val2.4h

        str val0q, [output], #64
        str val1q, [output, #-48]
        str val2q, [output, #-32]
        str val3q, [output, #-16]
        add final_copy_count, final_copy_count, #16
        cmp final_copy_count, #MLDSA_N
        b.lt rej_uniform_eta4_final_copy

    mov x0, count
    pop_stack
    ret

/****************** REGISTER DEALLOCATIONS *******************/
    .unreq output
    .unreq buf
    .unreq buflen
    .unreq table_idx
    .unreq len
    .unreq output_tmp
    .unreq output_tmp_base
    .unreq count
    .unreq buf_consumed
    .unreq tmp
    .unreq xtmp
    .unreq final_copy_count
    .unreq initial_zero_count
    .unreq rec_idx_0
    .unreq rec_idx_1
    .unreq rec_idx_0_w
    .unreq rec_idx_1_w
    .unreq ctr0
    .unreq ctr1
    .unreq ctr0_w
    .unreq ctr1_w
    .unreq ctr01
    .unreq buf0
    .unreq tmp0
    .unreq tmp1
    .unreq tmp2
    .unreq tmp3
    .unreq sign0
    .unreq sign1
    .unreq val0
    .unreq val0q
    .unreq val1
    .unreq val1q
    .unreq val2
    .unreq val2q
    .unreq val3
    .unreq val3q
    .unreq t0
    .unreq t1
    .unreq table0
    .unreq table0q
    .unreq table1
    .unreq table1q
    .unreq eta_bound
    .unreq bits
    .unreq wtmp
    .unreq const4

/* To facilitate single-compilation-unit (SCU) builds, undefine all macros.
 * Don't modify by hand -- this is auto-generated by scripts/autogen. */
#undef MLD_STACK_SIZE

/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
