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
#define MLD_STACK_SIZE (4*MLDSA_N + 64)

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

    /* Temporary registers */
    initial_zero_count          .req x11
    final_copy_count            .req x11

    rec_idx_0                   .req x12
    rec_idx_1                   .req x13
    rec_idx_2                   .req x14
    rec_idx_3                   .req x15

    ctr0                        .req x12
    ctr1                        .req x13
    ctr2                        .req x14
    ctr3                        .req x15

    ctr01                       .req ctr0
    ctr23                       .req ctr2

    /* Vector registers */

    buf0                        .req v0
    buf1                        .req v1
    buf2                        .req v2

    tmp0                        .req v4
    tmp1                        .req v5
    tmp2                        .req v6
    tmp3                        .req v7

    sign0                       .req v4
    sign1                       .req v5
    sign2                       .req v6
    sign3                       .req v7

    val0                        .req v16
    val0q                       .req q16
    val1                        .req v17
    val1q                       .req q17
    val2                        .req v18
    val2q                       .req q18
    val3                        .req v19
    val3q                       .req q19

    t0                          .req d20
    t1                          .req d21
    t2                          .req d22
    t3                          .req d23

    table0                      .req v24
    table0q                     .req q24
    table1                      .req v25
    table1q                     .req q25
    table2                      .req v26
    table2q                     .req q26
    table3                      .req v27
    table3q                     .req q27

    mldsa_q                     .req v30
    bits                        .req v31

    .text
    .global MLD_ASM_NAMESPACE(rej_uniform_asm)
    .balign 4
MLD_ASM_FN_SYMBOL(rej_uniform_asm)
    push_stack

    // Load 0x1, 0x2, 0x4, 0x8
    movz xtmp, 0x1
    movk xtmp, 0x2, lsl 32
    mov bits.d[0], xtmp

    movz xtmp, 0x4
    movk xtmp, 0x8, lsl 32
    mov bits.d[1], xtmp

    // load q = 8380417
    movz wtmp, #57345
    movk wtmp, #127, lsl #16
    dup mldsa_q.4s, wtmp

    mov output_tmp_base, sp
    mov output_tmp, output_tmp_base

    // The entire temporary stack buffer is copied to the output buffer
    // at the end of this routine. To avoid leaking original stack contents
    // in case not enough bytes have been sampled, zero the temporary buffer.
    mov initial_zero_count, #0
    eor val0.16b, val0.16b, val0.16b
rej_uniform_initial_zero:
        str val0q, [output_tmp], #64
        str val0q, [output_tmp, #-48]
        str val0q, [output_tmp, #-32]
        str val0q, [output_tmp, #-16]
        add initial_zero_count, initial_zero_count, #16
        cmp initial_zero_count, #MLDSA_N
        b.lt rej_uniform_initial_zero

    mov output_tmp, output_tmp_base

    mov count, #0
    mov len, #MLDSA_N

    cmp buflen, #48
    b.lo rej_uniform_loop48_end

rej_uniform_loop48:
        // Finish once we've generated sufficiently many coefficients
        cmp     count, len
        b.hs    rej_uniform_memory_copy

        // First, we unpack the byte stream into a stream of signed
        // coefficients, interpreting each consecutive 3 bytes as one
        // signed 24-bit coefficients, presented as 32-bit integers.
        // The topmost bit is masked out making it a 23-bit coefficient.
        //
        // We handle 16 coefficients a time, and use ld3 for the required
        // de-interleaving of the byte stream.
        sub     buflen, buflen, #48
        ld3     {buf0.16b, buf1.16b, buf2.16b}, [buf], #48

        // Mask out top-most bit
        movi    tmp0.16b, #0x80
        bic     buf2.16b, buf2.16b, tmp0.16b

        // Unpack 16 triples of bytes into 16 32-bit integers,
        // represented as 4 vectors val0-val3.
        zip1    tmp0.16b, buf0.16b, buf1.16b
        zip2    tmp1.16b, buf0.16b, buf1.16b
        uxtl    tmp2.8h, buf2.8b
        uxtl2   tmp3.8h, buf2.16b

        zip1    val0.8h, tmp0.8h, tmp2.8h
        zip2    val1.8h, tmp0.8h, tmp2.8h
        zip1    val2.8h, tmp1.8h, tmp3.8h
        zip2    val3.8h, tmp1.8h, tmp3.8h

        // At this point, val0-val3 are the signed integers to do rejection
        // sampling on. For each of them, do the following:
        // - Check which coefficients are within range, and represent the set
        //   of lane-indices of those coefficients as an 4-bit bitmap.
        // - Move the respective lanes to the front of the vector. This is the
        //   most complex part, and is done by interpreting the 4-bit bitmap as
        //   an index into a lookup table giving the lane-table to be use for
        //   the `tbl` instruction.
        // - Write the vector to the output buffer, but merely increase the output
        //   buffer pointer by the number of valid coefficients.


        // Set valid lanes to -1 (0b1...1)
        cmhi    sign0.4s, mldsa_q.4s, val0.4s
        cmhi    sign1.4s, mldsa_q.4s, val1.4s
        cmhi    sign2.4s, mldsa_q.4s, val2.4s
        cmhi    sign3.4s, mldsa_q.4s, val3.4s

        // If lane i is valid and has value -1, retain only i-th bit
        and     sign0.16b, sign0.16b, bits.16b
        and     sign1.16b, sign1.16b, bits.16b
        and     sign2.16b, sign2.16b, bits.16b
        and     sign3.16b, sign3.16b, bits.16b

        // Get 4-bit bitmap of valid lane indices by adding lanes
        uaddlv  t0, sign0.4s
        uaddlv  t1, sign1.4s
        uaddlv  t2, sign2.4s
        uaddlv  t3, sign3.4s

        fmov    rec_idx_0, t0
        fmov    rec_idx_1, t1
        fmov    rec_idx_2, t2
        fmov    rec_idx_3, t3

        ldr     table0q, [table_idx, rec_idx_0, lsl #4]
        ldr     table1q, [table_idx, rec_idx_1, lsl #4]
        ldr     table2q, [table_idx, rec_idx_2, lsl #4]
        ldr     table3q, [table_idx, rec_idx_3, lsl #4]

        // Compute number of valid coefficients. Recall that at this
        // point, lane i has value 2^i (hence popcount 1) if its coefficient
        // is valid, and 0 otherwise.
        cnt     sign0.16b, sign0.16b
        cnt     sign1.16b, sign1.16b
        cnt     sign2.16b, sign2.16b
        cnt     sign3.16b, sign3.16b

        // Extract number of valid coefficients
        uaddlv  t0, sign0.4s
        uaddlv  t1, sign1.4s
        uaddlv  t2, sign2.4s
        uaddlv  t3, sign3.4s

        fmov    ctr0, t0
        fmov    ctr1, t1
        fmov    ctr2, t2
        fmov    ctr3, t3

        // Move valid coefficients to the front
        tbl     val0.16b, {val0.16b}, table0.16b
        tbl     val1.16b, {val1.16b}, table1.16b
        tbl     val2.16b, {val2.16b}, table2.16b
        tbl     val3.16b, {val3.16b}, table3.16b

        str     val0q, [output_tmp]
        add     output_tmp, output_tmp, ctr0, lsl #2

        str     val1q, [output_tmp]
        add     output_tmp, output_tmp, ctr1, lsl #2

        str     val2q, [output_tmp]
        add     output_tmp, output_tmp, ctr2, lsl #2

        str     val3q, [output_tmp]
        add     output_tmp, output_tmp, ctr3, lsl #2

        add     ctr01, ctr0, ctr1
        add     ctr23, ctr2, ctr3
        add     count, count, ctr01
        add     count, count, ctr23

        cmp buflen, #48
        b.hs rej_uniform_loop48
rej_uniform_loop48_end:

    // Finish once we've generated sufficiently many coefficients
    cmp     count, len
    b.hs    rej_uniform_memory_copy

    cmp buflen, #24
    b.lo rej_uniform_memory_copy

        sub     buflen, buflen, #24
        ld3     {buf0.8b, buf1.8b, buf2.8b}, [buf], #24

        // mask out top-most bit
        movi    tmp0.16b, #0x80
        bic     buf2.16b, buf2.16b, tmp0.16b

        zip1    tmp0.16b, buf0.16b, buf1.16b
        uxtl    tmp2.8h, buf2.8b

        zip1    val0.8h, tmp0.8h, tmp2.8h
        zip2    val1.8h, tmp0.8h, tmp2.8h

        cmhi    sign0.4s, mldsa_q.4s, val0.4s
        cmhi    sign1.4s, mldsa_q.4s, val1.4s

        and     sign0.16b, sign0.16b, bits.16b
        and     sign1.16b, sign1.16b, bits.16b

        uaddlv  t0, sign0.4s
        uaddlv  t1, sign1.4s

        fmov    rec_idx_0, t0
        fmov    rec_idx_1, t1

        ldr     table0q, [table_idx, rec_idx_0, lsl #4]
        ldr     table1q, [table_idx, rec_idx_1, lsl #4]

        cnt     sign0.16b, sign0.16b
        cnt     sign1.16b, sign1.16b

        uaddlv  t0, sign0.4s
        uaddlv  t1, sign1.4s

        fmov    ctr0, t0
        fmov    ctr1, t1

        tbl     val0.16b, {val0.16b}, table0.16b
        tbl     val1.16b, {val1.16b}, table1.16b

        str     val0q, [output_tmp]
        add     output_tmp, output_tmp, ctr0, lsl #2

        str     val1q, [output_tmp]
        add     output_tmp, output_tmp, ctr1, lsl #2

        add     count, count, ctr0
        add     count, count, ctr1

rej_uniform_memory_copy:
    // min = min(count,len)
    cmp  count, len
    csel count, count, len, lo

    // Always copy MLDSA_N coefficients from the stack to the destination,
    // even if not all of them may be valid. This simplifies the loop and
    // allows us to stick to vectorized code.
    mov final_copy_count, #0
    mov output_tmp, output_tmp_base
rej_uniform_final_copy:
        ldr val0q, [output_tmp], #64
        ldr val1q, [output_tmp, #-48]
        ldr val2q, [output_tmp, #-32]
        ldr val3q, [output_tmp, #-16]
        str val0q, [output], #64
        str val1q, [output, #-48]
        str val2q, [output, #-32]
        str val3q, [output, #-16]
        add final_copy_count, final_copy_count, #16
        cmp final_copy_count, #MLDSA_N
        b.lt rej_uniform_final_copy

    mov x0, count
    b rej_uniform_return


rej_uniform_return:
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
    .unreq xtmp
    .unreq wtmp
    .unreq final_copy_count
    .unreq initial_zero_count
    .unreq rec_idx_0
    .unreq rec_idx_1
    .unreq rec_idx_2
    .unreq rec_idx_3
    .unreq ctr0
    .unreq ctr1
    .unreq ctr2
    .unreq ctr3
    .unreq ctr01
    .unreq ctr23
    .unreq buf0
    .unreq buf1
    .unreq buf2
    .unreq tmp0
    .unreq tmp1
    .unreq tmp2
    .unreq tmp3
    .unreq sign0
    .unreq sign1
    .unreq sign2
    .unreq sign3
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
    .unreq t2
    .unreq t3
    .unreq table0
    .unreq table0q
    .unreq table1
    .unreq table1q
    .unreq table2
    .unreq table2q
    .unreq table3
    .unreq table3q
    .unreq mldsa_q
    .unreq bits


/* To facilitate single-compilation-unit (SCU) builds, undefine all macros.
 * Don't modify by hand -- this is auto-generated by scripts/autogen. */
#undef MLD_STACK_SIZE

/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
