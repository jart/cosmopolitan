/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include "../../../common.h"

#if defined(MLD_ARITH_BACKEND_AARCH64) && !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

/*
 * WARNING: This file is auto-derived from the mldsa-native source file
 *   dev/aarch64_opt/src/poly_chknorm_asm.S using scripts/simpasm. Do not modify it directly.
 */

#if defined(__ELF__)
.section .note.GNU-stack,"",@progbits
#endif

.text
.balign 4
.global MLD_ASM_NAMESPACE(poly_chknorm_asm)
MLD_ASM_FN_SYMBOL(poly_chknorm_asm)

        .cfi_startproc
        dup	v20.4s, w1
        movi	v21.4s, #0x0
        mov	x2, #0x10               // =16

Lpoly_chknorm_loop:
        ldr	q1, [x0, #0x10]
        ldr	q2, [x0, #0x20]
        ldr	q3, [x0, #0x30]
        ldr	q0, [x0], #0x40
        abs	v1.4s, v1.4s
        cmge	v1.4s, v1.4s, v20.4s
        orr	v21.16b, v21.16b, v1.16b
        abs	v2.4s, v2.4s
        cmge	v2.4s, v2.4s, v20.4s
        orr	v21.16b, v21.16b, v2.16b
        abs	v3.4s, v3.4s
        cmge	v3.4s, v3.4s, v20.4s
        orr	v21.16b, v21.16b, v3.16b
        abs	v0.4s, v0.4s
        cmge	v0.4s, v0.4s, v20.4s
        orr	v21.16b, v21.16b, v0.16b
        subs	x2, x2, #0x1
        b.ne	Lpoly_chknorm_loop
        umaxv	s21, v21.4s
        fmov	w0, s21
        and	w0, w0, #0x1
        ret
        .cfi_endproc

#endif /* MLD_ARITH_BACKEND_AARCH64 && !MLD_CONFIG_MULTILEVEL_NO_SHARED */
