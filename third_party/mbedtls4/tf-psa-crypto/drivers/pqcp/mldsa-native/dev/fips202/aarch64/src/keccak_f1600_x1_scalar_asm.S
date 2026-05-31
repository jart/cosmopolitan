/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * Copyright (c) 2021-2022 Arm Limited
 * Copyright (c) 2022 Matthias Kannwischer
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

// Author: Hanno Becker <hanno.becker@arm.com>
// Author: Matthias Kannwischer <matthias@kannwischer.eu>

#include "../../../../common.h"
#if defined(MLD_FIPS202_AARCH64_NEED_X1_SCALAR) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

/*
 * WARNING: This file is auto-derived from the mlkem-native source file
 *   dev/fips202/aarch64/src/keccak_f1600_x1_scalar_asm.S using scripts/simpasm. Do not modify it directly.
 */


.text
.balign 4
.global MLD_ASM_NAMESPACE(keccak_f1600_x1_scalar_asm)
MLD_ASM_FN_SYMBOL(keccak_f1600_x1_scalar_asm)

        sub	sp, sp, #0x80
        stp	x19, x20, [sp, #0x20]
        stp	x21, x22, [sp, #0x30]
        stp	x23, x24, [sp, #0x40]
        stp	x25, x26, [sp, #0x50]
        stp	x27, x28, [sp, #0x60]
        stp	x29, x30, [sp, #0x70]

keccak_f1600_x1_scalar_initial:
        mov	x26, x1
        str	x1, [sp, #0x8]
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
        str	x0, [sp]
        eor	x30, x24, x25
        eor	x27, x9, x10
        eor	x0, x30, x21
        eor	x26, x27, x6
        eor	x27, x26, x7
        eor	x29, x0, x22
        eor	x26, x29, x23
        eor	x29, x4, x5
        eor	x30, x29, x1
        eor	x0, x27, x8
        eor	x29, x30, x2
        eor	x30, x19, x20
        eor	x30, x30, x16
        eor	x27, x26, x0, ror #63
        eor	x4, x4, x27
        eor	x30, x30, x17
        eor	x30, x30, x28
        eor	x29, x29, x3
        eor	x0, x0, x30, ror #63
        eor	x30, x30, x29, ror #63
        eor	x22, x22, x30
        eor	x23, x23, x30
        str	x23, [sp, #0x18]
        eor	x23, x14, x15
        eor	x14, x14, x0
        eor	x23, x23, x11
        eor	x15, x15, x0
        eor	x1, x1, x27
        eor	x23, x23, x12
        eor	x23, x23, x13
        eor	x11, x11, x0
        eor	x29, x29, x23, ror #63
        eor	x23, x23, x26, ror #63
        eor	x26, x13, x0
        eor	x13, x28, x23
        eor	x28, x24, x30
        eor	x24, x16, x23
        eor	x16, x21, x30
        eor	x21, x25, x30
        eor	x30, x19, x23
        eor	x19, x20, x23
        eor	x20, x17, x23
        eor	x17, x12, x0
        eor	x0, x2, x27
        eor	x2, x6, x29
        eor	x6, x8, x29
        bic	x8, x28, x13, ror #47
        eor	x12, x3, x27
        bic	x3, x13, x17, ror #19
        eor	x5, x5, x27
        ldr	x27, [sp, #0x18]
        bic	x25, x17, x2, ror #5
        eor	x9, x9, x29
        eor	x23, x25, x5, ror #52
        eor	x3, x3, x2, ror #24
        eor	x8, x8, x17, ror #2
        eor	x17, x10, x29
        bic	x25, x12, x22, ror #47
        eor	x29, x7, x29
        bic	x10, x4, x27, ror #2
        bic	x7, x5, x28, ror #10
        eor	x10, x10, x20, ror #50
        eor	x13, x7, x13, ror #57
        bic	x7, x2, x5, ror #47
        eor	x2, x25, x24, ror #39
        bic	x25, x20, x11, ror #57
        bic	x5, x17, x4, ror #25
        eor	x25, x25, x17, ror #53
        bic	x17, x11, x17, ror #60
        eor	x28, x7, x28, ror #57
        bic	x7, x9, x12, ror #42
        eor	x7, x7, x22, ror #25
        bic	x22, x22, x24, ror #56
        bic	x24, x24, x15, ror #31
        eor	x22, x22, x15, ror #23
        bic	x20, x27, x20, ror #48
        bic	x15, x15, x9, ror #16
        eor	x12, x15, x12, ror #58
        eor	x15, x5, x27, ror #27
        eor	x5, x20, x11, ror #41
        ldr	x11, [sp, #0x8]
        eor	x20, x17, x4, ror #21
        eor	x17, x24, x9, ror #47
        mov	x24, #0x1               // =1
        bic	x9, x0, x16, ror #9
        str	x24, [sp, #0x10]
        bic	x24, x29, x1, ror #44
        bic	x27, x1, x21, ror #50
        bic	x4, x26, x29, ror #63
        eor	x1, x1, x4, ror #21
        ldr	x11, [x11]
        bic	x4, x21, x30, ror #57
        eor	x21, x24, x21, ror #30
        eor	x24, x9, x19, ror #44
        bic	x9, x14, x6, ror #5
        eor	x9, x9, x0, ror #43
        bic	x0, x6, x0, ror #38
        eor	x1, x1, x11
        eor	x11, x4, x26, ror #35
        eor	x4, x0, x16, ror #47
        bic	x0, x16, x19, ror #35
        eor	x16, x27, x30, ror #43
        bic	x27, x30, x26, ror #42
        bic	x26, x19, x14, ror #41
        eor	x19, x0, x14, ror #12
        eor	x14, x26, x6, ror #46
        eor	x6, x27, x29, ror #41

keccak_f1600_x1_scalar_loop:
        eor	x0, x15, x11, ror #52
        eor	x0, x0, x13, ror #48
        eor	x26, x8, x9, ror #57
        eor	x27, x0, x14, ror #10
        eor	x29, x16, x28, ror #63
        eor	x26, x26, x6, ror #51
        eor	x30, x23, x22, ror #50
        eor	x0, x26, x10, ror #31
        eor	x29, x29, x19, ror #37
        eor	x27, x27, x12, ror #5
        eor	x30, x30, x24, ror #34
        eor	x0, x0, x7, ror #27
        eor	x26, x30, x21, ror #26
        eor	x26, x26, x25, ror #15
        ror	x30, x27, #0x3e
        eor	x30, x30, x26, ror #57
        ror	x26, x26, #0x3a
        eor	x16, x30, x16
        eor	x28, x30, x28, ror #63
        str	x28, [sp, #0x18]
        eor	x29, x29, x17, ror #36
        eor	x28, x1, x2, ror #61
        eor	x19, x30, x19, ror #37
        eor	x29, x29, x20, ror #2
        eor	x28, x28, x4, ror #54
        eor	x26, x26, x0, ror #55
        eor	x28, x28, x3, ror #39
        eor	x28, x28, x5, ror #25
        ror	x0, x0, #0x38
        eor	x0, x0, x29, ror #63
        eor	x27, x28, x27, ror #61
        eor	x13, x0, x13, ror #46
        eor	x28, x29, x28, ror #63
        eor	x29, x30, x20, ror #2
        eor	x20, x26, x3, ror #39
        eor	x11, x0, x11, ror #50
        eor	x25, x28, x25, ror #9
        eor	x3, x28, x21, ror #20
        eor	x21, x26, x1
        eor	x9, x27, x9, ror #49
        eor	x24, x28, x24, ror #28
        eor	x1, x30, x17, ror #36
        eor	x14, x0, x14, ror #8
        eor	x22, x28, x22, ror #44
        eor	x8, x27, x8, ror #56
        eor	x17, x27, x7, ror #19
        eor	x15, x0, x15, ror #62
        bic	x7, x20, x22, ror #47
        eor	x4, x26, x4, ror #54
        eor	x0, x0, x12, ror #3
        eor	x28, x28, x23, ror #58
        eor	x23, x26, x2, ror #61
        eor	x26, x26, x5, ror #25
        eor	x2, x7, x16, ror #39
        bic	x7, x9, x20, ror #42
        bic	x30, x15, x9, ror #16
        eor	x7, x7, x22, ror #25
        eor	x12, x30, x20, ror #58
        bic	x20, x22, x16, ror #56
        eor	x30, x27, x6, ror #43
        eor	x22, x20, x15, ror #23
        bic	x6, x19, x13, ror #42
        eor	x6, x6, x17, ror #41
        bic	x5, x13, x17, ror #63
        eor	x5, x21, x5, ror #21
        bic	x17, x17, x21, ror #44
        eor	x27, x27, x10, ror #23
        bic	x21, x21, x25, ror #50
        bic	x20, x27, x4, ror #25
        bic	x10, x16, x15, ror #31
        eor	x16, x21, x19, ror #43
        eor	x21, x17, x25, ror #30
        bic	x19, x25, x19, ror #57
        ldr	x25, [sp, #0x10]
        eor	x17, x10, x9, ror #47
        ldr	x9, [sp, #0x8]
        eor	x15, x20, x28, ror #27
        bic	x20, x4, x28, ror #2
        eor	x10, x20, x1, ror #50
        bic	x20, x11, x27, ror #60
        eor	x20, x20, x4, ror #21
        bic	x4, x28, x1, ror #48
        bic	x1, x1, x11, ror #57
        ldr	x28, [x9, x25, lsl #3]
        ldr	x9, [sp, #0x18]
        add	x25, x25, #0x1
        str	x25, [sp, #0x10]
        cmp	x25, #0x17
        eor	x25, x1, x27, ror #53
        bic	x27, x30, x26, ror #47
        eor	x1, x5, x28
        eor	x5, x4, x11, ror #41
        eor	x11, x19, x13, ror #35
        bic	x13, x26, x24, ror #10
        eor	x28, x27, x24, ror #57
        bic	x27, x24, x9, ror #47
        bic	x19, x23, x3, ror #9
        bic	x4, x29, x14, ror #41
        eor	x24, x19, x29, ror #44
        bic	x29, x3, x29, ror #35
        eor	x13, x13, x9, ror #57
        eor	x19, x29, x14, ror #12
        bic	x29, x9, x0, ror #19
        bic	x14, x14, x8, ror #5
        eor	x9, x14, x23, ror #43
        eor	x14, x4, x8, ror #46
        bic	x23, x8, x23, ror #38
        eor	x8, x27, x0, ror #2
        eor	x4, x23, x3, ror #47
        bic	x3, x0, x30, ror #5
        eor	x23, x3, x26, ror #52
        eor	x3, x29, x30, ror #24
        b.le	keccak_f1600_x1_scalar_loop
        ror	x6, x6, #0x2b
        ror	x11, x11, #0x32
        ror	x21, x21, #0x14
        ror	x2, x2, #0x3d
        ror	x7, x7, #0x13
        ror	x12, x12, #0x3
        ror	x17, x17, #0x24
        ror	x22, x22, #0x2c
        ror	x3, x3, #0x27
        ror	x8, x8, #0x38
        ror	x13, x13, #0x2e
        ror	x28, x28, #0x3f
        ror	x23, x23, #0x3a
        ror	x4, x4, #0x36
        ror	x9, x9, #0x31
        ror	x14, x14, #0x8
        ror	x19, x19, #0x25
        ror	x24, x24, #0x1c
        ror	x5, x5, #0x19
        ror	x10, x10, #0x17
        ror	x15, x15, #0x3e
        ror	x20, x20, #0x2
        ror	x25, x25, #0x9
        ldr	x0, [sp]
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
        ldp	x19, x20, [sp, #0x20]
        ldp	x21, x22, [sp, #0x30]
        ldp	x23, x24, [sp, #0x40]
        ldp	x25, x26, [sp, #0x50]
        ldp	x27, x28, [sp, #0x60]
        ldp	x29, x30, [sp, #0x70]
        add	sp, sp, #0x80
        ret
/* simpasm: footer-start */
#endif /* MLD_FIPS202_AARCH64_NEED_X1_SCALAR && \
          !MLD_CONFIG_MULTILEVEL_NO_SHARED */
