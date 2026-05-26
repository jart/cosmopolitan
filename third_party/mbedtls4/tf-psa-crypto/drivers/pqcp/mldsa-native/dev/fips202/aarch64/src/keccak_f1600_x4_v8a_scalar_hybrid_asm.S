/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) 2021-2022 Arm Limited
 * Copyright (c) 2022 Matthias Kannwischer
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

// Author: Hanno Becker <hannobecker@posteo.de>
// Author: Matthias Kannwischer <matthias@kannwischer.eu>

#include "../../../../common.h"
#if defined(MLD_FIPS202_AARCH64_NEED_X4_V8A_SCALAR_HYBRID) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

/*
 * WARNING: This file is auto-derived from the mlkem-native source file
 *   dev/fips202/aarch64/src/keccak_f1600_x4_v8a_scalar_hybrid_asm.S using scripts/simpasm. Do not modify it directly.
 */


.text
.balign 4
.global MLD_ASM_NAMESPACE(keccak_f1600_x4_scalar_v8a_hybrid_asm)
MLD_ASM_FN_SYMBOL(keccak_f1600_x4_scalar_v8a_hybrid_asm)

        sub	sp, sp, #0xe0
        stp	x19, x20, [sp, #0x30]
        stp	x21, x22, [sp, #0x40]
        stp	x23, x24, [sp, #0x50]
        stp	x25, x26, [sp, #0x60]
        stp	x27, x28, [sp, #0x70]
        stp	x29, x30, [sp, #0x80]
        stp	d8, d9, [sp, #0x90]
        stp	d10, d11, [sp, #0xa0]
        stp	d12, d13, [sp, #0xb0]
        stp	d14, d15, [sp, #0xc0]
        mov	x29, x1
        mov	x30, #0x0               // =0
        str	x30, [sp, #0x20]
        str	x29, [sp, #0x8]
        str	x29, [sp, #0x10]
        str	x0, [sp]
        add	x4, x0, #0xc8
        ldp	q25, q26, [x0]
        ldp	q27, q28, [x4]
        trn1	v0.2d, v25.2d, v27.2d
        trn2	v1.2d, v25.2d, v27.2d
        trn1	v2.2d, v26.2d, v28.2d
        trn2	v3.2d, v26.2d, v28.2d
        ldp	q25, q26, [x0, #0x20]
        ldp	q27, q28, [x4, #0x20]
        trn1	v4.2d, v25.2d, v27.2d
        trn2	v5.2d, v25.2d, v27.2d
        trn1	v6.2d, v26.2d, v28.2d
        trn2	v7.2d, v26.2d, v28.2d
        ldp	q25, q26, [x0, #0x40]
        ldp	q27, q28, [x4, #0x40]
        trn1	v8.2d, v25.2d, v27.2d
        trn2	v9.2d, v25.2d, v27.2d
        trn1	v10.2d, v26.2d, v28.2d
        trn2	v11.2d, v26.2d, v28.2d
        ldp	q25, q26, [x0, #0x60]
        ldp	q27, q28, [x4, #0x60]
        trn1	v12.2d, v25.2d, v27.2d
        trn2	v13.2d, v25.2d, v27.2d
        trn1	v14.2d, v26.2d, v28.2d
        trn2	v15.2d, v26.2d, v28.2d
        ldp	q25, q26, [x0, #0x80]
        ldp	q27, q28, [x4, #0x80]
        trn1	v16.2d, v25.2d, v27.2d
        trn2	v17.2d, v25.2d, v27.2d
        trn1	v18.2d, v26.2d, v28.2d
        trn2	v19.2d, v26.2d, v28.2d
        ldp	q25, q26, [x0, #0xa0]
        ldp	q27, q28, [x4, #0xa0]
        trn1	v20.2d, v25.2d, v27.2d
        trn2	v21.2d, v25.2d, v27.2d
        trn1	v22.2d, v26.2d, v28.2d
        trn2	v23.2d, v26.2d, v28.2d
        ldr	d25, [x0, #0xc0]
        ldr	d27, [x4, #0xc0]
        trn1	v24.2d, v25.2d, v27.2d
        add	x0, x0, #0x190
        ldp	x1, x6, [x0]
        ldp	x11, x16, [x0, #0x10]
        ldp	x21, x2, [x0, #0x20]
        ldp	x7, x12, [x0, #0x30]
        ldp	x17, x22, [x0, #0x40]
        ldp	x3, x8, [x0, #0x50]
        ldp	x13, x28, [x0, #0x60]
        ldp	x23, x4, [x0, #0x70]
        ldp	x9, x14, [x0, #0x80]
        ldp	x19, x24, [x0, #0x90]
        ldp	x5, x10, [x0, #0xa0]
        ldp	x15, x20, [x0, #0xb0]
        ldr	x25, [x0, #0xc0]
        sub	x0, x0, #0x190

keccak_f1600_x4_scalar_v8a_hybrid_initial:
        eor	x30, x24, x25
        eor	x27, x9, x10
        eor	v30.16b, v0.16b, v5.16b
        eor	v30.16b, v30.16b, v10.16b
        eor	x0, x30, x21
        eor	v30.16b, v30.16b, v15.16b
        eor	x26, x27, x6
        eor	x27, x26, x7
        eor	v30.16b, v30.16b, v20.16b
        eor	x29, x0, x22
        eor	v29.16b, v1.16b, v6.16b
        eor	x26, x29, x23
        eor	v29.16b, v29.16b, v11.16b
        eor	x29, x4, x5
        eor	x30, x29, x1
        eor	v29.16b, v29.16b, v16.16b
        eor	x0, x27, x8
        eor	v29.16b, v29.16b, v21.16b
        eor	x29, x30, x2
        eor	v28.16b, v2.16b, v7.16b
        eor	x30, x19, x20
        eor	x30, x30, x16
        eor	v28.16b, v28.16b, v12.16b
        eor	x27, x26, x0, ror #63
        eor	v28.16b, v28.16b, v17.16b
        eor	x4, x4, x27
        eor	v28.16b, v28.16b, v22.16b
        eor	x30, x30, x17
        eor	x30, x30, x28
        eor	v27.16b, v3.16b, v8.16b
        eor	x29, x29, x3
        eor	v27.16b, v27.16b, v13.16b
        eor	x0, x0, x30, ror #63
        eor	v27.16b, v27.16b, v18.16b
        eor	x30, x30, x29, ror #63
        eor	x22, x22, x30
        eor	v27.16b, v27.16b, v23.16b
        eor	x23, x23, x30
        eor	v26.16b, v4.16b, v9.16b
        str	x23, [sp, #0xd0]
        eor	v26.16b, v26.16b, v14.16b
        eor	x23, x14, x15
        eor	x14, x14, x0
        eor	v26.16b, v26.16b, v19.16b
        eor	x23, x23, x11
        eor	v26.16b, v26.16b, v24.16b
        eor	x15, x15, x0
        eor	x1, x1, x27
        add	v31.2d, v28.2d, v28.2d
        eor	x23, x23, x12
        sri	v31.2d, v28.2d, #0x3f
        eor	x23, x23, x13
        eor	v25.16b, v31.16b, v30.16b
        eor	x11, x11, x0
        eor	x29, x29, x23, ror #63
        add	v31.2d, v26.2d, v26.2d
        eor	x23, x23, x26, ror #63
        sri	v31.2d, v26.2d, #0x3f
        eor	x26, x13, x0
        eor	v28.16b, v31.16b, v28.16b
        eor	x13, x28, x23
        eor	x28, x24, x30
        add	v31.2d, v29.2d, v29.2d
        eor	x24, x16, x23
        sri	v31.2d, v29.2d, #0x3f
        eor	x16, x21, x30
        eor	v26.16b, v31.16b, v26.16b
        eor	x21, x25, x30
        eor	x30, x19, x23
        add	v31.2d, v27.2d, v27.2d
        eor	x19, x20, x23
        sri	v31.2d, v27.2d, #0x3f
        eor	x20, x17, x23
        eor	v29.16b, v31.16b, v29.16b
        eor	x17, x12, x0
        eor	x0, x2, x27
        add	v31.2d, v30.2d, v30.2d
        eor	x2, x6, x29
        sri	v31.2d, v30.2d, #0x3f
        eor	x6, x8, x29
        eor	v27.16b, v31.16b, v27.16b
        bic	x8, x28, x13, ror #47
        eor	x12, x3, x27
        eor	v30.16b, v0.16b, v26.16b
        bic	x3, x13, x17, ror #19
        eor	v31.16b, v2.16b, v29.16b
        eor	x5, x5, x27
        ldr	x27, [sp, #0xd0]
        shl	v0.2d, v31.2d, #0x3e
        bic	x25, x17, x2, ror #5
        sri	v0.2d, v31.2d, #0x2
        eor	x9, x9, x29
        eor	v31.16b, v12.16b, v29.16b
        eor	x23, x25, x5, ror #52
        eor	x3, x3, x2, ror #24
        shl	v2.2d, v31.2d, #0x2b
        eor	x8, x8, x17, ror #2
        sri	v2.2d, v31.2d, #0x15
        eor	x17, x10, x29
        eor	v31.16b, v13.16b, v28.16b
        bic	x25, x12, x22, ror #47
        eor	x29, x7, x29
        shl	v12.2d, v31.2d, #0x19
        bic	x10, x4, x27, ror #2
        sri	v12.2d, v31.2d, #0x27
        bic	x7, x5, x28, ror #10
        eor	v31.16b, v19.16b, v27.16b
        eor	x10, x10, x20, ror #50
        eor	x13, x7, x13, ror #57
        shl	v13.2d, v31.2d, #0x8
        bic	x7, x2, x5, ror #47
        sri	v13.2d, v31.2d, #0x38
        eor	x2, x25, x24, ror #39
        eor	v31.16b, v23.16b, v28.16b
        bic	x25, x20, x11, ror #57
        bic	x5, x17, x4, ror #25
        shl	v19.2d, v31.2d, #0x38
        eor	x25, x25, x17, ror #53
        sri	v19.2d, v31.2d, #0x8
        bic	x17, x11, x17, ror #60
        eor	v31.16b, v15.16b, v26.16b
        eor	x28, x7, x28, ror #57
        bic	x7, x9, x12, ror #42
        shl	v23.2d, v31.2d, #0x29
        eor	x7, x7, x22, ror #25
        sri	v23.2d, v31.2d, #0x17
        bic	x22, x22, x24, ror #56
        bic	x24, x24, x15, ror #31
        eor	v31.16b, v1.16b, v25.16b
        eor	x22, x22, x15, ror #23
        shl	v15.2d, v31.2d, #0x1
        bic	x20, x27, x20, ror #48
        sri	v15.2d, v31.2d, #0x3f
        bic	x15, x15, x9, ror #16
        eor	x12, x15, x12, ror #58
        eor	v31.16b, v8.16b, v28.16b
        eor	x15, x5, x27, ror #27
        shl	v1.2d, v31.2d, #0x37
        eor	x5, x20, x11, ror #41
        sri	v1.2d, v31.2d, #0x9
        ldr	x11, [sp, #0x8]
        eor	x20, x17, x4, ror #21
        eor	v31.16b, v16.16b, v25.16b
        eor	x17, x24, x9, ror #47
        shl	v8.2d, v31.2d, #0x2d
        mov	x24, #0x1               // =1
        sri	v8.2d, v31.2d, #0x13
        bic	x9, x0, x16, ror #9
        str	x24, [sp, #0x18]
        eor	v31.16b, v7.16b, v29.16b
        bic	x24, x29, x1, ror #44
        shl	v16.2d, v31.2d, #0x6
        bic	x27, x1, x21, ror #50
        sri	v16.2d, v31.2d, #0x3a
        bic	x4, x26, x29, ror #63
        eor	x1, x1, x4, ror #21
        eor	v31.16b, v10.16b, v26.16b
        ldr	x11, [x11]
        shl	v7.2d, v31.2d, #0x3
        bic	x4, x21, x30, ror #57
        sri	v7.2d, v31.2d, #0x3d
        eor	x21, x24, x21, ror #30
        eor	x24, x9, x19, ror #44
        eor	v31.16b, v3.16b, v28.16b
        bic	x9, x14, x6, ror #5
        shl	v10.2d, v31.2d, #0x1c
        eor	x9, x9, x0, ror #43
        sri	v10.2d, v31.2d, #0x24
        bic	x0, x6, x0, ror #38
        eor	x1, x1, x11
        eor	v31.16b, v18.16b, v28.16b
        eor	x11, x4, x26, ror #35
        shl	v3.2d, v31.2d, #0x15
        eor	x4, x0, x16, ror #47
        bic	x0, x16, x19, ror #35
        sri	v3.2d, v31.2d, #0x2b
        eor	x16, x27, x30, ror #43
        eor	v31.16b, v17.16b, v29.16b
        bic	x27, x30, x26, ror #42
        shl	v18.2d, v31.2d, #0xf
        bic	x26, x19, x14, ror #41
        eor	x19, x0, x14, ror #12
        sri	v18.2d, v31.2d, #0x31
        eor	x14, x26, x6, ror #46
        eor	v31.16b, v11.16b, v25.16b
        eor	x6, x27, x29, ror #41
        shl	v17.2d, v31.2d, #0xa
        eor	x0, x15, x11, ror #52
        eor	x0, x0, x13, ror #48
        sri	v17.2d, v31.2d, #0x36
        eor	x26, x8, x9, ror #57
        eor	v31.16b, v9.16b, v27.16b
        eor	x27, x0, x14, ror #10
        shl	v11.2d, v31.2d, #0x14
        eor	x29, x16, x28, ror #63
        eor	x26, x26, x6, ror #51
        sri	v11.2d, v31.2d, #0x2c
        eor	x30, x23, x22, ror #50
        eor	v31.16b, v22.16b, v29.16b
        eor	x0, x26, x10, ror #31
        shl	v9.2d, v31.2d, #0x3d
        eor	x29, x29, x19, ror #37
        eor	x27, x27, x12, ror #5
        sri	v9.2d, v31.2d, #0x3
        eor	x30, x30, x24, ror #34
        eor	v31.16b, v14.16b, v27.16b
        eor	x0, x0, x7, ror #27
        shl	v22.2d, v31.2d, #0x27
        eor	x26, x30, x21, ror #26
        eor	x26, x26, x25, ror #15
        sri	v22.2d, v31.2d, #0x19
        ror	x30, x27, #0x3e
        eor	v31.16b, v20.16b, v26.16b
        eor	x30, x30, x26, ror #57
        ror	x26, x26, #0x3a
        shl	v14.2d, v31.2d, #0x12
        eor	x16, x30, x16
        sri	v14.2d, v31.2d, #0x2e
        eor	x28, x30, x28, ror #63
        eor	v31.16b, v4.16b, v27.16b
        str	x28, [sp, #0xd0]
        eor	x29, x29, x17, ror #36
        shl	v20.2d, v31.2d, #0x1b
        eor	x28, x1, x2, ror #61
        sri	v20.2d, v31.2d, #0x25
        eor	x19, x30, x19, ror #37
        eor	v31.16b, v24.16b, v27.16b
        eor	x29, x29, x20, ror #2
        eor	x28, x28, x4, ror #54
        shl	v4.2d, v31.2d, #0xe
        eor	x26, x26, x0, ror #55
        sri	v4.2d, v31.2d, #0x32
        eor	x28, x28, x3, ror #39
        eor	v31.16b, v21.16b, v25.16b
        eor	x28, x28, x5, ror #25
        ror	x0, x0, #0x38
        shl	v24.2d, v31.2d, #0x2
        eor	x0, x0, x29, ror #63
        sri	v24.2d, v31.2d, #0x3e
        eor	x27, x28, x27, ror #61
        eor	v31.16b, v5.16b, v26.16b
        eor	x13, x0, x13, ror #46
        eor	x28, x29, x28, ror #63
        shl	v21.2d, v31.2d, #0x24
        eor	x29, x30, x20, ror #2
        sri	v21.2d, v31.2d, #0x1c
        eor	x20, x26, x3, ror #39
        eor	v31.16b, v6.16b, v25.16b
        eor	x11, x0, x11, ror #50
        eor	x25, x28, x25, ror #9
        shl	v27.2d, v31.2d, #0x2c
        eor	x3, x28, x21, ror #20
        sri	v27.2d, v31.2d, #0x14
        eor	x21, x26, x1
        eor	x9, x27, x9, ror #49
        bic	v31.16b, v7.16b, v11.16b
        eor	x24, x28, x24, ror #28
        eor	v5.16b, v31.16b, v10.16b
        eor	x1, x30, x17, ror #36
        bic	v31.16b, v8.16b, v7.16b
        eor	x14, x0, x14, ror #8
        eor	x22, x28, x22, ror #44
        eor	v6.16b, v31.16b, v11.16b
        eor	x8, x27, x8, ror #56
        bic	v31.16b, v9.16b, v8.16b
        eor	x17, x27, x7, ror #19
        eor	v7.16b, v31.16b, v7.16b
        eor	x15, x0, x15, ror #62
        bic	x7, x20, x22, ror #47
        bic	v31.16b, v10.16b, v9.16b
        eor	x4, x26, x4, ror #54
        eor	v8.16b, v31.16b, v8.16b
        eor	x0, x0, x12, ror #3
        bic	v31.16b, v11.16b, v10.16b
        eor	x28, x28, x23, ror #58
        eor	x23, x26, x2, ror #61
        eor	v9.16b, v31.16b, v9.16b
        eor	x26, x26, x5, ror #25
        bic	v31.16b, v12.16b, v16.16b
        eor	x2, x7, x16, ror #39
        eor	v10.16b, v31.16b, v15.16b
        bic	x7, x9, x20, ror #42
        bic	x30, x15, x9, ror #16
        bic	v31.16b, v13.16b, v12.16b
        eor	x7, x7, x22, ror #25
        eor	v11.16b, v31.16b, v16.16b
        eor	x12, x30, x20, ror #58
        bic	v31.16b, v14.16b, v13.16b
        bic	x20, x22, x16, ror #56
        eor	x30, x27, x6, ror #43
        eor	v12.16b, v31.16b, v12.16b
        eor	x22, x20, x15, ror #23
        bic	v31.16b, v15.16b, v14.16b
        bic	x6, x19, x13, ror #42
        eor	v13.16b, v31.16b, v13.16b
        eor	x6, x6, x17, ror #41
        bic	x5, x13, x17, ror #63
        bic	v31.16b, v16.16b, v15.16b
        eor	x5, x21, x5, ror #21
        eor	v14.16b, v31.16b, v14.16b
        bic	x17, x17, x21, ror #44
        eor	x27, x27, x10, ror #23
        bic	v31.16b, v17.16b, v21.16b
        bic	x21, x21, x25, ror #50
        eor	v15.16b, v31.16b, v20.16b
        bic	x20, x27, x4, ror #25
        bic	v31.16b, v18.16b, v17.16b
        bic	x10, x16, x15, ror #31
        eor	x16, x21, x19, ror #43
        eor	v16.16b, v31.16b, v21.16b
        eor	x21, x17, x25, ror #30
        bic	v31.16b, v19.16b, v18.16b
        bic	x19, x25, x19, ror #57
        eor	v17.16b, v31.16b, v17.16b
        ldr	x25, [sp, #0x18]
        eor	x17, x10, x9, ror #47
        bic	v31.16b, v20.16b, v19.16b
        ldr	x9, [sp, #0x8]
        eor	v18.16b, v31.16b, v18.16b
        eor	x15, x20, x28, ror #27
        bic	v31.16b, v21.16b, v20.16b
        bic	x20, x4, x28, ror #2
        eor	x10, x20, x1, ror #50
        eor	v19.16b, v31.16b, v19.16b
        bic	x20, x11, x27, ror #60
        bic	v31.16b, v22.16b, v1.16b
        eor	x20, x20, x4, ror #21
        eor	v20.16b, v31.16b, v0.16b
        bic	x4, x28, x1, ror #48
        bic	x1, x1, x11, ror #57
        bic	v31.16b, v23.16b, v22.16b
        ldr	x28, [x9, x25, lsl #3]
        eor	v21.16b, v31.16b, v1.16b
        ldr	x9, [sp, #0xd0]
        bic	v31.16b, v24.16b, v23.16b
        add	x25, x25, #0x1
        str	x25, [sp, #0x18]
        eor	v22.16b, v31.16b, v22.16b
        cmp	x25, #0x17
        bic	v31.16b, v0.16b, v24.16b
        eor	x25, x1, x27, ror #53
        bic	x27, x30, x26, ror #47
        eor	v23.16b, v31.16b, v23.16b
        eor	x1, x5, x28
        bic	v31.16b, v1.16b, v0.16b
        eor	x5, x4, x11, ror #41
        eor	v24.16b, v31.16b, v24.16b
        eor	x11, x19, x13, ror #35
        bic	x13, x26, x24, ror #10
        bic	v31.16b, v2.16b, v27.16b
        eor	x28, x27, x24, ror #57
        eor	v0.16b, v31.16b, v30.16b
        bic	x27, x24, x9, ror #47
        bic	v31.16b, v3.16b, v2.16b
        bic	x19, x23, x3, ror #9
        bic	x4, x29, x14, ror #41
        eor	v1.16b, v31.16b, v27.16b
        eor	x24, x19, x29, ror #44
        bic	v31.16b, v4.16b, v3.16b
        bic	x29, x3, x29, ror #35
        eor	v2.16b, v31.16b, v2.16b
        eor	x13, x13, x9, ror #57
        eor	x19, x29, x14, ror #12
        bic	v31.16b, v30.16b, v4.16b
        bic	x29, x9, x0, ror #19
        eor	v3.16b, v31.16b, v3.16b
        bic	x14, x14, x8, ror #5
        bic	v31.16b, v27.16b, v30.16b
        eor	x9, x14, x23, ror #43
        eor	x14, x4, x8, ror #46
        eor	v4.16b, v31.16b, v4.16b
        bic	x23, x8, x23, ror #38
        eor	x8, x27, x0, ror #2
        eor	x4, x23, x3, ror #47
        bic	x3, x0, x30, ror #5
        eor	x23, x3, x26, ror #52
        eor	x3, x29, x30, ror #24
        ldr	x30, [sp, #0x10]
        ld1r	{ v28.2d }, [x30], #8
        str	x30, [sp, #0x10]
        eor	v0.16b, v0.16b, v28.16b

keccak_f1600_x4_scalar_v8a_hybrid_loop:
        eor	x0, x15, x11, ror #52
        eor	x0, x0, x13, ror #48
        eor	v30.16b, v0.16b, v5.16b
        eor	v30.16b, v30.16b, v10.16b
        eor	x26, x8, x9, ror #57
        eor	v30.16b, v30.16b, v15.16b
        eor	x27, x0, x14, ror #10
        eor	x29, x16, x28, ror #63
        eor	v30.16b, v30.16b, v20.16b
        eor	x26, x26, x6, ror #51
        eor	v29.16b, v1.16b, v6.16b
        eor	x30, x23, x22, ror #50
        eor	v29.16b, v29.16b, v11.16b
        eor	x0, x26, x10, ror #31
        eor	x29, x29, x19, ror #37
        eor	v29.16b, v29.16b, v16.16b
        eor	x27, x27, x12, ror #5
        eor	v29.16b, v29.16b, v21.16b
        eor	x30, x30, x24, ror #34
        eor	x0, x0, x7, ror #27
        eor	v28.16b, v2.16b, v7.16b
        eor	x26, x30, x21, ror #26
        eor	v28.16b, v28.16b, v12.16b
        eor	x26, x26, x25, ror #15
        eor	v28.16b, v28.16b, v17.16b
        ror	x30, x27, #0x3e
        eor	x30, x30, x26, ror #57
        eor	v28.16b, v28.16b, v22.16b
        ror	x26, x26, #0x3a
        eor	v27.16b, v3.16b, v8.16b
        eor	x16, x30, x16
        eor	v27.16b, v27.16b, v13.16b
        eor	x28, x30, x28, ror #63
        str	x28, [sp, #0xd0]
        eor	v27.16b, v27.16b, v18.16b
        eor	x29, x29, x17, ror #36
        eor	v27.16b, v27.16b, v23.16b
        eor	x28, x1, x2, ror #61
        eor	x19, x30, x19, ror #37
        eor	v26.16b, v4.16b, v9.16b
        eor	x29, x29, x20, ror #2
        eor	v26.16b, v26.16b, v14.16b
        eor	x28, x28, x4, ror #54
        eor	v26.16b, v26.16b, v19.16b
        eor	x26, x26, x0, ror #55
        eor	x28, x28, x3, ror #39
        eor	v26.16b, v26.16b, v24.16b
        eor	x28, x28, x5, ror #25
        add	v31.2d, v28.2d, v28.2d
        ror	x0, x0, #0x38
        eor	x0, x0, x29, ror #63
        sri	v31.2d, v28.2d, #0x3f
        eor	x27, x28, x27, ror #61
        eor	v25.16b, v31.16b, v30.16b
        eor	x13, x0, x13, ror #46
        add	v31.2d, v26.2d, v26.2d
        eor	x28, x29, x28, ror #63
        eor	x29, x30, x20, ror #2
        sri	v31.2d, v26.2d, #0x3f
        eor	x20, x26, x3, ror #39
        eor	v28.16b, v31.16b, v28.16b
        eor	x11, x0, x11, ror #50
        add	v31.2d, v29.2d, v29.2d
        eor	x25, x28, x25, ror #9
        eor	x3, x28, x21, ror #20
        sri	v31.2d, v29.2d, #0x3f
        eor	x21, x26, x1
        eor	v26.16b, v31.16b, v26.16b
        eor	x9, x27, x9, ror #49
        eor	x24, x28, x24, ror #28
        add	v31.2d, v27.2d, v27.2d
        eor	x1, x30, x17, ror #36
        sri	v31.2d, v27.2d, #0x3f
        eor	x14, x0, x14, ror #8
        eor	v29.16b, v31.16b, v29.16b
        eor	x22, x28, x22, ror #44
        eor	x8, x27, x8, ror #56
        add	v31.2d, v30.2d, v30.2d
        eor	x17, x27, x7, ror #19
        sri	v31.2d, v30.2d, #0x3f
        eor	x15, x0, x15, ror #62
        bic	x7, x20, x22, ror #47
        eor	v27.16b, v31.16b, v27.16b
        eor	x4, x26, x4, ror #54
        eor	v30.16b, v0.16b, v26.16b
        eor	x0, x0, x12, ror #3
        eor	v31.16b, v2.16b, v29.16b
        eor	x28, x28, x23, ror #58
        eor	x23, x26, x2, ror #61
        shl	v0.2d, v31.2d, #0x3e
        eor	x26, x26, x5, ror #25
        sri	v0.2d, v31.2d, #0x2
        eor	x2, x7, x16, ror #39
        eor	v31.16b, v12.16b, v29.16b
        bic	x7, x9, x20, ror #42
        bic	x30, x15, x9, ror #16
        shl	v2.2d, v31.2d, #0x2b
        eor	x7, x7, x22, ror #25
        sri	v2.2d, v31.2d, #0x15
        eor	x12, x30, x20, ror #58
        bic	x20, x22, x16, ror #56
        eor	v31.16b, v13.16b, v28.16b
        eor	x30, x27, x6, ror #43
        shl	v12.2d, v31.2d, #0x19
        eor	x22, x20, x15, ror #23
        sri	v12.2d, v31.2d, #0x27
        bic	x6, x19, x13, ror #42
        eor	x6, x6, x17, ror #41
        eor	v31.16b, v19.16b, v27.16b
        bic	x5, x13, x17, ror #63
        shl	v13.2d, v31.2d, #0x8
        eor	x5, x21, x5, ror #21
        sri	v13.2d, v31.2d, #0x38
        bic	x17, x17, x21, ror #44
        eor	x27, x27, x10, ror #23
        eor	v31.16b, v23.16b, v28.16b
        bic	x21, x21, x25, ror #50
        shl	v19.2d, v31.2d, #0x38
        bic	x20, x27, x4, ror #25
        bic	x10, x16, x15, ror #31
        sri	v19.2d, v31.2d, #0x8
        eor	x16, x21, x19, ror #43
        eor	v31.16b, v15.16b, v26.16b
        eor	x21, x17, x25, ror #30
        shl	v23.2d, v31.2d, #0x29
        bic	x19, x25, x19, ror #57
        ldr	x25, [sp, #0x18]
        sri	v23.2d, v31.2d, #0x17
        eor	x17, x10, x9, ror #47
        eor	v31.16b, v1.16b, v25.16b
        ldr	x9, [sp, #0x8]
        eor	x15, x20, x28, ror #27
        shl	v15.2d, v31.2d, #0x1
        bic	x20, x4, x28, ror #2
        sri	v15.2d, v31.2d, #0x3f
        eor	x10, x20, x1, ror #50
        eor	v31.16b, v8.16b, v28.16b
        bic	x20, x11, x27, ror #60
        eor	x20, x20, x4, ror #21
        shl	v1.2d, v31.2d, #0x37
        bic	x4, x28, x1, ror #48
        sri	v1.2d, v31.2d, #0x9
        bic	x1, x1, x11, ror #57
        eor	v31.16b, v16.16b, v25.16b
        ldr	x28, [x9, x25, lsl #3]
        ldr	x9, [sp, #0xd0]
        shl	v8.2d, v31.2d, #0x2d
        add	x25, x25, #0x1
        sri	v8.2d, v31.2d, #0x13
        str	x25, [sp, #0x18]
        cmp	x25, #0x17
        eor	v31.16b, v7.16b, v29.16b
        eor	x25, x1, x27, ror #53
        shl	v16.2d, v31.2d, #0x6
        bic	x27, x30, x26, ror #47
        sri	v16.2d, v31.2d, #0x3a
        eor	x1, x5, x28
        eor	x5, x4, x11, ror #41
        eor	v31.16b, v10.16b, v26.16b
        eor	x11, x19, x13, ror #35
        shl	v7.2d, v31.2d, #0x3
        bic	x13, x26, x24, ror #10
        eor	x28, x27, x24, ror #57
        sri	v7.2d, v31.2d, #0x3d
        bic	x27, x24, x9, ror #47
        eor	v31.16b, v3.16b, v28.16b
        bic	x19, x23, x3, ror #9
        shl	v10.2d, v31.2d, #0x1c
        bic	x4, x29, x14, ror #41
        eor	x24, x19, x29, ror #44
        sri	v10.2d, v31.2d, #0x24
        bic	x29, x3, x29, ror #35
        eor	v31.16b, v18.16b, v28.16b
        eor	x13, x13, x9, ror #57
        shl	v3.2d, v31.2d, #0x15
        eor	x19, x29, x14, ror #12
        bic	x29, x9, x0, ror #19
        sri	v3.2d, v31.2d, #0x2b
        bic	x14, x14, x8, ror #5
        eor	v31.16b, v17.16b, v29.16b
        eor	x9, x14, x23, ror #43
        eor	x14, x4, x8, ror #46
        shl	v18.2d, v31.2d, #0xf
        bic	x23, x8, x23, ror #38
        sri	v18.2d, v31.2d, #0x31
        eor	x8, x27, x0, ror #2
        eor	v31.16b, v11.16b, v25.16b
        eor	x4, x23, x3, ror #47
        bic	x3, x0, x30, ror #5
        shl	v17.2d, v31.2d, #0xa
        eor	x23, x3, x26, ror #52
        sri	v17.2d, v31.2d, #0x36
        eor	x3, x29, x30, ror #24
        eor	x0, x15, x11, ror #52
        eor	v31.16b, v9.16b, v27.16b
        eor	x0, x0, x13, ror #48
        shl	v11.2d, v31.2d, #0x14
        eor	x26, x8, x9, ror #57
        sri	v11.2d, v31.2d, #0x2c
        eor	x27, x0, x14, ror #10
        eor	x29, x16, x28, ror #63
        eor	v31.16b, v22.16b, v29.16b
        eor	x26, x26, x6, ror #51
        shl	v9.2d, v31.2d, #0x3d
        eor	x30, x23, x22, ror #50
        sri	v9.2d, v31.2d, #0x3
        eor	x0, x26, x10, ror #31
        eor	x29, x29, x19, ror #37
        eor	v31.16b, v14.16b, v27.16b
        eor	x27, x27, x12, ror #5
        shl	v22.2d, v31.2d, #0x27
        eor	x30, x30, x24, ror #34
        eor	x0, x0, x7, ror #27
        sri	v22.2d, v31.2d, #0x19
        eor	x26, x30, x21, ror #26
        eor	v31.16b, v20.16b, v26.16b
        eor	x26, x26, x25, ror #15
        shl	v14.2d, v31.2d, #0x12
        ror	x30, x27, #0x3e
        eor	x30, x30, x26, ror #57
        sri	v14.2d, v31.2d, #0x2e
        ror	x26, x26, #0x3a
        eor	v31.16b, v4.16b, v27.16b
        eor	x16, x30, x16
        shl	v20.2d, v31.2d, #0x1b
        eor	x28, x30, x28, ror #63
        str	x28, [sp, #0xd0]
        sri	v20.2d, v31.2d, #0x25
        eor	x29, x29, x17, ror #36
        eor	v31.16b, v24.16b, v27.16b
        eor	x28, x1, x2, ror #61
        eor	x19, x30, x19, ror #37
        shl	v4.2d, v31.2d, #0xe
        eor	x29, x29, x20, ror #2
        sri	v4.2d, v31.2d, #0x32
        eor	x28, x28, x4, ror #54
        eor	v31.16b, v21.16b, v25.16b
        eor	x26, x26, x0, ror #55
        eor	x28, x28, x3, ror #39
        shl	v24.2d, v31.2d, #0x2
        eor	x28, x28, x5, ror #25
        sri	v24.2d, v31.2d, #0x3e
        ror	x0, x0, #0x38
        eor	x0, x0, x29, ror #63
        eor	v31.16b, v5.16b, v26.16b
        eor	x27, x28, x27, ror #61
        shl	v21.2d, v31.2d, #0x24
        eor	x13, x0, x13, ror #46
        sri	v21.2d, v31.2d, #0x1c
        eor	x28, x29, x28, ror #63
        eor	x29, x30, x20, ror #2
        eor	v31.16b, v6.16b, v25.16b
        eor	x20, x26, x3, ror #39
        shl	v27.2d, v31.2d, #0x2c
        eor	x11, x0, x11, ror #50
        sri	v27.2d, v31.2d, #0x14
        eor	x25, x28, x25, ror #9
        eor	x3, x28, x21, ror #20
        bic	v31.16b, v7.16b, v11.16b
        eor	x21, x26, x1
        eor	v5.16b, v31.16b, v10.16b
        eor	x9, x27, x9, ror #49
        eor	x24, x28, x24, ror #28
        bic	v31.16b, v8.16b, v7.16b
        eor	x1, x30, x17, ror #36
        eor	v6.16b, v31.16b, v11.16b
        eor	x14, x0, x14, ror #8
        bic	v31.16b, v9.16b, v8.16b
        eor	x22, x28, x22, ror #44
        eor	x8, x27, x8, ror #56
        eor	v7.16b, v31.16b, v7.16b
        eor	x17, x27, x7, ror #19
        bic	v31.16b, v10.16b, v9.16b
        eor	x15, x0, x15, ror #62
        bic	x7, x20, x22, ror #47
        eor	v8.16b, v31.16b, v8.16b
        eor	x4, x26, x4, ror #54
        bic	v31.16b, v11.16b, v10.16b
        eor	x0, x0, x12, ror #3
        eor	v9.16b, v31.16b, v9.16b
        eor	x28, x28, x23, ror #58
        eor	x23, x26, x2, ror #61
        bic	v31.16b, v12.16b, v16.16b
        eor	x26, x26, x5, ror #25
        eor	v10.16b, v31.16b, v15.16b
        eor	x2, x7, x16, ror #39
        bic	v31.16b, v13.16b, v12.16b
        bic	x7, x9, x20, ror #42
        bic	x30, x15, x9, ror #16
        eor	v11.16b, v31.16b, v16.16b
        eor	x7, x7, x22, ror #25
        bic	v31.16b, v14.16b, v13.16b
        eor	x12, x30, x20, ror #58
        bic	x20, x22, x16, ror #56
        eor	v12.16b, v31.16b, v12.16b
        eor	x30, x27, x6, ror #43
        bic	v31.16b, v15.16b, v14.16b
        eor	x22, x20, x15, ror #23
        eor	v13.16b, v31.16b, v13.16b
        bic	x6, x19, x13, ror #42
        eor	x6, x6, x17, ror #41
        bic	v31.16b, v16.16b, v15.16b
        bic	x5, x13, x17, ror #63
        eor	v14.16b, v31.16b, v14.16b
        eor	x5, x21, x5, ror #21
        bic	v31.16b, v17.16b, v21.16b
        bic	x17, x17, x21, ror #44
        eor	x27, x27, x10, ror #23
        eor	v15.16b, v31.16b, v20.16b
        bic	x21, x21, x25, ror #50
        bic	v31.16b, v18.16b, v17.16b
        bic	x20, x27, x4, ror #25
        bic	x10, x16, x15, ror #31
        eor	v16.16b, v31.16b, v21.16b
        eor	x16, x21, x19, ror #43
        bic	v31.16b, v19.16b, v18.16b
        eor	x21, x17, x25, ror #30
        eor	v17.16b, v31.16b, v17.16b
        bic	x19, x25, x19, ror #57
        ldr	x25, [sp, #0x18]
        bic	v31.16b, v20.16b, v19.16b
        eor	x17, x10, x9, ror #47
        eor	v18.16b, v31.16b, v18.16b
        ldr	x9, [sp, #0x8]
        eor	x15, x20, x28, ror #27
        bic	v31.16b, v21.16b, v20.16b
        bic	x20, x4, x28, ror #2
        eor	v19.16b, v31.16b, v19.16b
        eor	x10, x20, x1, ror #50
        bic	v31.16b, v22.16b, v1.16b
        bic	x20, x11, x27, ror #60
        eor	x20, x20, x4, ror #21
        eor	v20.16b, v31.16b, v0.16b
        bic	x4, x28, x1, ror #48
        bic	v31.16b, v23.16b, v22.16b
        bic	x1, x1, x11, ror #57
        eor	v21.16b, v31.16b, v1.16b
        ldr	x28, [x9, x25, lsl #3]
        ldr	x9, [sp, #0xd0]
        bic	v31.16b, v24.16b, v23.16b
        add	x25, x25, #0x1
        eor	v22.16b, v31.16b, v22.16b
        str	x25, [sp, #0x18]
        cmp	x25, #0x17
        bic	v31.16b, v0.16b, v24.16b
        eor	x25, x1, x27, ror #53
        eor	v23.16b, v31.16b, v23.16b
        bic	x27, x30, x26, ror #47
        bic	v31.16b, v1.16b, v0.16b
        eor	x1, x5, x28
        eor	x5, x4, x11, ror #41
        eor	v24.16b, v31.16b, v24.16b
        eor	x11, x19, x13, ror #35
        bic	v31.16b, v2.16b, v27.16b
        bic	x13, x26, x24, ror #10
        eor	x28, x27, x24, ror #57
        eor	v0.16b, v31.16b, v30.16b
        bic	x27, x24, x9, ror #47
        bic	v31.16b, v3.16b, v2.16b
        bic	x19, x23, x3, ror #9
        eor	v1.16b, v31.16b, v27.16b
        bic	x4, x29, x14, ror #41
        eor	x24, x19, x29, ror #44
        bic	v31.16b, v4.16b, v3.16b
        bic	x29, x3, x29, ror #35
        eor	v2.16b, v31.16b, v2.16b
        eor	x13, x13, x9, ror #57
        bic	v31.16b, v30.16b, v4.16b
        eor	x19, x29, x14, ror #12
        bic	x29, x9, x0, ror #19
        eor	v3.16b, v31.16b, v3.16b
        bic	x14, x14, x8, ror #5
        bic	v31.16b, v27.16b, v30.16b
        eor	x9, x14, x23, ror #43
        eor	x14, x4, x8, ror #46
        eor	v4.16b, v31.16b, v4.16b
        bic	x23, x8, x23, ror #38
        eor	x8, x27, x0, ror #2
        eor	x4, x23, x3, ror #47
        bic	x3, x0, x30, ror #5
        eor	x23, x3, x26, ror #52
        eor	x3, x29, x30, ror #24
        ldr	x30, [sp, #0x10]
        ld1r	{ v28.2d }, [x30], #8
        str	x30, [sp, #0x10]
        eor	v0.16b, v0.16b, v28.16b

keccak_f1600_x4_scalar_v8a_hybrid_loop_end:
        b.le	keccak_f1600_x4_scalar_v8a_hybrid_loop
        ror	x2, x2, #0x3d
        ror	x3, x3, #0x27
        ror	x4, x4, #0x36
        ror	x5, x5, #0x19
        ror	x6, x6, #0x2b
        ror	x7, x7, #0x13
        ror	x8, x8, #0x38
        ror	x9, x9, #0x31
        ror	x10, x10, #0x17
        ror	x11, x11, #0x32
        ror	x12, x12, #0x3
        ror	x13, x13, #0x2e
        ror	x14, x14, #0x8
        ror	x15, x15, #0x3e
        ror	x17, x17, #0x24
        ror	x28, x28, #0x3f
        ror	x19, x19, #0x25
        ror	x20, x20, #0x2
        ror	x21, x21, #0x14
        ror	x22, x22, #0x2c
        ror	x23, x23, #0x3a
        ror	x24, x24, #0x1c
        ror	x25, x25, #0x9
        ldr	x30, [sp, #0x20]
        cmp	x30, #0x1
        b.eq	keccak_f1600_x4_scalar_v8a_hybrid_done
        mov	x30, #0x1               // =1
        str	x30, [sp, #0x20]
        ldr	x0, [sp]
        add	x0, x0, #0x190
        stp	x1, x6, [x0]
        stp	x11, x16, [x0, #0x10]
        stp	x21, x2, [x0, #0x20]
        stp	x7, x12, [x0, #0x30]
        stp	x17, x22, [x0, #0x40]
        stp	x3, x8, [x0, #0x50]
        stp	x13, x28, [x0, #0x60]
        stp	x23, x4, [x0, #0x70]
        stp	x9, x14, [x0, #0x80]
        stp	x19, x24, [x0, #0x90]
        stp	x5, x10, [x0, #0xa0]
        stp	x15, x20, [x0, #0xb0]
        str	x25, [x0, #0xc0]
        sub	x0, x0, #0x190
        add	x0, x0, #0x258
        ldp	x1, x6, [x0]
        ldp	x11, x16, [x0, #0x10]
        ldp	x21, x2, [x0, #0x20]
        ldp	x7, x12, [x0, #0x30]
        ldp	x17, x22, [x0, #0x40]
        ldp	x3, x8, [x0, #0x50]
        ldp	x13, x28, [x0, #0x60]
        ldp	x23, x4, [x0, #0x70]
        ldp	x9, x14, [x0, #0x80]
        ldp	x19, x24, [x0, #0x90]
        ldp	x5, x10, [x0, #0xa0]
        ldp	x15, x20, [x0, #0xb0]
        ldr	x25, [x0, #0xc0]
        sub	x0, x0, #0x258
        b	keccak_f1600_x4_scalar_v8a_hybrid_initial

keccak_f1600_x4_scalar_v8a_hybrid_done:
        ldr	x0, [sp]
        add	x0, x0, #0x258
        stp	x1, x6, [x0]
        stp	x11, x16, [x0, #0x10]
        stp	x21, x2, [x0, #0x20]
        stp	x7, x12, [x0, #0x30]
        stp	x17, x22, [x0, #0x40]
        stp	x3, x8, [x0, #0x50]
        stp	x13, x28, [x0, #0x60]
        stp	x23, x4, [x0, #0x70]
        stp	x9, x14, [x0, #0x80]
        stp	x19, x24, [x0, #0x90]
        stp	x5, x10, [x0, #0xa0]
        stp	x15, x20, [x0, #0xb0]
        str	x25, [x0, #0xc0]
        sub	x0, x0, #0x258
        add	x4, x0, #0xc8
        trn1	v25.2d, v0.2d, v1.2d
        trn1	v26.2d, v2.2d, v3.2d
        stp	q25, q26, [x0]
        trn2	v27.2d, v0.2d, v1.2d
        trn2	v28.2d, v2.2d, v3.2d
        stp	q27, q28, [x4]
        trn1	v25.2d, v4.2d, v5.2d
        trn1	v26.2d, v6.2d, v7.2d
        stp	q25, q26, [x0, #0x20]
        trn2	v27.2d, v4.2d, v5.2d
        trn2	v28.2d, v6.2d, v7.2d
        stp	q27, q28, [x4, #0x20]
        trn1	v25.2d, v8.2d, v9.2d
        trn1	v26.2d, v10.2d, v11.2d
        stp	q25, q26, [x0, #0x40]
        trn2	v27.2d, v8.2d, v9.2d
        trn2	v28.2d, v10.2d, v11.2d
        stp	q27, q28, [x4, #0x40]
        trn1	v25.2d, v12.2d, v13.2d
        trn1	v26.2d, v14.2d, v15.2d
        stp	q25, q26, [x0, #0x60]
        trn2	v27.2d, v12.2d, v13.2d
        trn2	v28.2d, v14.2d, v15.2d
        stp	q27, q28, [x4, #0x60]
        trn1	v25.2d, v16.2d, v17.2d
        trn1	v26.2d, v18.2d, v19.2d
        stp	q25, q26, [x0, #0x80]
        trn2	v27.2d, v16.2d, v17.2d
        trn2	v28.2d, v18.2d, v19.2d
        stp	q27, q28, [x4, #0x80]
        trn1	v25.2d, v20.2d, v21.2d
        trn1	v26.2d, v22.2d, v23.2d
        stp	q25, q26, [x0, #0xa0]
        trn2	v27.2d, v20.2d, v21.2d
        trn2	v28.2d, v22.2d, v23.2d
        stp	q27, q28, [x4, #0xa0]
        str	d24, [x0, #0xc0]
        trn2	v25.2d, v24.2d, v24.2d
        str	d25, [x4, #0xc0]
        ldp	d14, d15, [sp, #0xc0]
        ldp	d12, d13, [sp, #0xb0]
        ldp	d10, d11, [sp, #0xa0]
        ldp	d8, d9, [sp, #0x90]
        ldp	x19, x20, [sp, #0x30]
        ldp	x21, x22, [sp, #0x40]
        ldp	x23, x24, [sp, #0x50]
        ldp	x25, x26, [sp, #0x60]
        ldp	x27, x28, [sp, #0x70]
        ldp	x29, x30, [sp, #0x80]
        add	sp, sp, #0xe0
        ret
/* simpasm: footer-start */
#endif /* MLD_FIPS202_AARCH64_NEED_X4_V8A_SCALAR_HYBRID && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED */
