/*
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) The mlkem-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

 #include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_AARCH64) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

/*
 * WARNING: This file is auto-derived from the mldsa-native source file
 *   dev/aarch64_opt/src/rej_uniform_asm.S using scripts/simpasm. Do not modify it directly.
 */

#if defined(__ELF__)
.section .note.GNU-stack,"",@progbits
#endif

.text
.balign 4
.global MLD_ASM_NAMESPACE(rej_uniform_asm)
MLD_ASM_FN_SYMBOL(rej_uniform_asm)

        .cfi_startproc
        sub	sp, sp, #0x440
        .cfi_adjust_cfa_offset 0x440
        mov	x7, #0x1                // =1
        movk	x7, #0x2, lsl #32
        mov	v31.d[0], x7
        mov	x7, #0x4                // =4
        movk	x7, #0x8, lsl #32
        mov	v31.d[1], x7
        mov	w7, #0xe001             // =57345
        movk	w7, #0x7f, lsl #16
        dup	v30.4s, w7
        mov	x8, sp
        mov	x7, x8
        mov	x11, #0x0               // =0
        eor	v16.16b, v16.16b, v16.16b

Lrej_uniform_initial_zero:
        str	q16, [x7], #0x40
        stur	q16, [x7, #-0x30]
        stur	q16, [x7, #-0x20]
        stur	q16, [x7, #-0x10]
        add	x11, x11, #0x10
        cmp	x11, #0x100
        b.lt	Lrej_uniform_initial_zero
        mov	x7, x8
        mov	x9, #0x0                // =0
        mov	x4, #0x100              // =256
        cmp	x2, #0x30
        b.lo	Lrej_uniform_loop48_end

Lrej_uniform_loop48:
        cmp	x9, x4
        b.hs	Lrej_uniform_memory_copy
        sub	x2, x2, #0x30
        ld3	{ v0.16b, v1.16b, v2.16b }, [x1], #48
        movi	v4.16b, #0x80
        bic	v2.16b, v2.16b, v4.16b
        zip1	v4.16b, v0.16b, v1.16b
        zip2	v5.16b, v0.16b, v1.16b
        ushll	v6.8h, v2.8b, #0x0
        ushll2	v7.8h, v2.16b, #0x0
        zip1	v16.8h, v4.8h, v6.8h
        zip2	v17.8h, v4.8h, v6.8h
        zip1	v18.8h, v5.8h, v7.8h
        zip2	v19.8h, v5.8h, v7.8h
        cmhi	v4.4s, v30.4s, v16.4s
        cmhi	v5.4s, v30.4s, v17.4s
        cmhi	v6.4s, v30.4s, v18.4s
        cmhi	v7.4s, v30.4s, v19.4s
        and	v4.16b, v4.16b, v31.16b
        and	v5.16b, v5.16b, v31.16b
        and	v6.16b, v6.16b, v31.16b
        and	v7.16b, v7.16b, v31.16b
        uaddlv	d20, v4.4s
        uaddlv	d21, v5.4s
        uaddlv	d22, v6.4s
        uaddlv	d23, v7.4s
        fmov	x12, d20
        fmov	x13, d21
        fmov	x14, d22
        fmov	x15, d23
        ldr	q24, [x3, x12, lsl #4]
        ldr	q25, [x3, x13, lsl #4]
        ldr	q26, [x3, x14, lsl #4]
        ldr	q27, [x3, x15, lsl #4]
        cnt	v4.16b, v4.16b
        cnt	v5.16b, v5.16b
        cnt	v6.16b, v6.16b
        cnt	v7.16b, v7.16b
        uaddlv	d20, v4.4s
        uaddlv	d21, v5.4s
        uaddlv	d22, v6.4s
        uaddlv	d23, v7.4s
        fmov	x12, d20
        fmov	x13, d21
        fmov	x14, d22
        fmov	x15, d23
        tbl	v16.16b, { v16.16b }, v24.16b
        tbl	v17.16b, { v17.16b }, v25.16b
        tbl	v18.16b, { v18.16b }, v26.16b
        tbl	v19.16b, { v19.16b }, v27.16b
        str	q16, [x7]
        add	x7, x7, x12, lsl #2
        str	q17, [x7]
        add	x7, x7, x13, lsl #2
        str	q18, [x7]
        add	x7, x7, x14, lsl #2
        str	q19, [x7]
        add	x7, x7, x15, lsl #2
        add	x12, x12, x13
        add	x14, x14, x15
        add	x9, x9, x12
        add	x9, x9, x14
        cmp	x2, #0x30
        b.hs	Lrej_uniform_loop48

Lrej_uniform_loop48_end:
        cmp	x9, x4
        b.hs	Lrej_uniform_memory_copy
        cmp	x2, #0x18
        b.lo	Lrej_uniform_memory_copy
        sub	x2, x2, #0x18
        ld3	{ v0.8b, v1.8b, v2.8b }, [x1], #24
        movi	v4.16b, #0x80
        bic	v2.16b, v2.16b, v4.16b
        zip1	v4.16b, v0.16b, v1.16b
        ushll	v6.8h, v2.8b, #0x0
        zip1	v16.8h, v4.8h, v6.8h
        zip2	v17.8h, v4.8h, v6.8h
        cmhi	v4.4s, v30.4s, v16.4s
        cmhi	v5.4s, v30.4s, v17.4s
        and	v4.16b, v4.16b, v31.16b
        and	v5.16b, v5.16b, v31.16b
        uaddlv	d20, v4.4s
        uaddlv	d21, v5.4s
        fmov	x12, d20
        fmov	x13, d21
        ldr	q24, [x3, x12, lsl #4]
        ldr	q25, [x3, x13, lsl #4]
        cnt	v4.16b, v4.16b
        cnt	v5.16b, v5.16b
        uaddlv	d20, v4.4s
        uaddlv	d21, v5.4s
        fmov	x12, d20
        fmov	x13, d21
        tbl	v16.16b, { v16.16b }, v24.16b
        tbl	v17.16b, { v17.16b }, v25.16b
        str	q16, [x7]
        add	x7, x7, x12, lsl #2
        str	q17, [x7]
        add	x7, x7, x13, lsl #2
        add	x9, x9, x12
        add	x9, x9, x13

Lrej_uniform_memory_copy:
        cmp	x9, x4
        csel	x9, x9, x4, lo
        mov	x11, #0x0               // =0
        mov	x7, x8

Lrej_uniform_final_copy:
        ldr	q16, [x7], #0x40
        ldur	q17, [x7, #-0x30]
        ldur	q18, [x7, #-0x20]
        ldur	q19, [x7, #-0x10]
        str	q16, [x0], #0x40
        stur	q17, [x0, #-0x30]
        stur	q18, [x0, #-0x20]
        stur	q19, [x0, #-0x10]
        add	x11, x11, #0x10
        cmp	x11, #0x100
        b.lt	Lrej_uniform_final_copy
        mov	x0, x9
        b	Lrej_uniform_return

Lrej_uniform_return:
        add	sp, sp, #0x440
        .cfi_adjust_cfa_offset -0x440
        ret
        .cfi_endproc

#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
