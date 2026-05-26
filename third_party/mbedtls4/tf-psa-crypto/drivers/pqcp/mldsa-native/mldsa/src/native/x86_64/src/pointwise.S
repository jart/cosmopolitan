/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */

/* References
 * ==========
 *
 * - [REF_AVX2]
 *   CRYSTALS-Dilithium optimized AVX2 implementation
 *   Bai, Ducas, Kiltz, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehlé
 *   https://github.com/pq-crystals/dilithium/tree/master/avx2
 */

/*
 * This file is derived from the public domain
 * AVX2 Dilithium implementation @[REF_AVX2].
 */

#include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_X86_64_DEFAULT) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)

/*
 * WARNING: This file is auto-derived from the mldsa-native source file
 *   dev/x86_64/src/pointwise.S using scripts/simpasm. Do not modify it directly.
 */

#if defined(__ELF__)
.section .note.GNU-stack,"",@progbits
#endif

.text
.balign 4
.global MLD_ASM_NAMESPACE(pointwise_avx2)
MLD_ASM_FN_SYMBOL(pointwise_avx2)

        .cfi_startproc
        vmovdqa	0x20(%rcx), %ymm0
        vmovdqa	(%rcx), %ymm1
        xorl	%eax, %eax

Lpointwise_avx2_looptop1:
        vmovdqa	(%rsi), %ymm2
        vmovdqa	0x20(%rsi), %ymm4
        vmovdqa	0x40(%rsi), %ymm6
        vmovdqa	(%rdx), %ymm10
        vmovdqa	0x20(%rdx), %ymm12
        vmovdqa	0x40(%rdx), %ymm14
        vpsrlq	$0x20, %ymm2, %ymm3
        vpsrlq	$0x20, %ymm4, %ymm5
        vmovshdup	%ymm6, %ymm7    # ymm7 = ymm6[1,1,3,3,5,5,7,7]
        vpsrlq	$0x20, %ymm10, %ymm11
        vpsrlq	$0x20, %ymm12, %ymm13
        vmovshdup	%ymm14, %ymm15  # ymm15 = ymm14[1,1,3,3,5,5,7,7]
        vpmuldq	%ymm10, %ymm2, %ymm2
        vpmuldq	%ymm11, %ymm3, %ymm3
        vpmuldq	%ymm12, %ymm4, %ymm4
        vpmuldq	%ymm13, %ymm5, %ymm5
        vpmuldq	%ymm14, %ymm6, %ymm6
        vpmuldq	%ymm15, %ymm7, %ymm7
        vpmuldq	%ymm2, %ymm0, %ymm10
        vpmuldq	%ymm3, %ymm0, %ymm11
        vpmuldq	%ymm4, %ymm0, %ymm12
        vpmuldq	%ymm5, %ymm0, %ymm13
        vpmuldq	%ymm6, %ymm0, %ymm14
        vpmuldq	%ymm7, %ymm0, %ymm15
        vpmuldq	%ymm10, %ymm1, %ymm10
        vpmuldq	%ymm11, %ymm1, %ymm11
        vpmuldq	%ymm12, %ymm1, %ymm12
        vpmuldq	%ymm13, %ymm1, %ymm13
        vpmuldq	%ymm14, %ymm1, %ymm14
        vpmuldq	%ymm15, %ymm1, %ymm15
        vpsubq	%ymm10, %ymm2, %ymm2
        vpsubq	%ymm11, %ymm3, %ymm3
        vpsubq	%ymm12, %ymm4, %ymm4
        vpsubq	%ymm13, %ymm5, %ymm5
        vpsubq	%ymm14, %ymm6, %ymm6
        vpsubq	%ymm15, %ymm7, %ymm7
        vpsrlq	$0x20, %ymm2, %ymm2
        vpsrlq	$0x20, %ymm4, %ymm4
        vmovshdup	%ymm6, %ymm6    # ymm6 = ymm6[1,1,3,3,5,5,7,7]
        vpblendd	$0xaa, %ymm3, %ymm2, %ymm2 # ymm2 = ymm2[0],ymm3[1],ymm2[2],ymm3[3],ymm2[4],ymm3[5],ymm2[6],ymm3[7]
        vpblendd	$0xaa, %ymm5, %ymm4, %ymm4 # ymm4 = ymm4[0],ymm5[1],ymm4[2],ymm5[3],ymm4[4],ymm5[5],ymm4[6],ymm5[7]
        vpblendd	$0xaa, %ymm7, %ymm6, %ymm6 # ymm6 = ymm6[0],ymm7[1],ymm6[2],ymm7[3],ymm6[4],ymm7[5],ymm6[6],ymm7[7]
        vmovdqa	%ymm2, (%rdi)
        vmovdqa	%ymm4, 0x20(%rdi)
        vmovdqa	%ymm6, 0x40(%rdi)
        addq	$0x60, %rdi
        addq	$0x60, %rsi
        addq	$0x60, %rdx
        addl	$0x1, %eax
        cmpl	$0xa, %eax
        jb	Lpointwise_avx2_looptop1
        vmovdqa	(%rsi), %ymm2
        vmovdqa	0x20(%rsi), %ymm4
        vmovdqa	(%rdx), %ymm10
        vmovdqa	0x20(%rdx), %ymm12
        vpsrlq	$0x20, %ymm2, %ymm3
        vpsrlq	$0x20, %ymm4, %ymm5
        vmovshdup	%ymm10, %ymm11  # ymm11 = ymm10[1,1,3,3,5,5,7,7]
        vmovshdup	%ymm12, %ymm13  # ymm13 = ymm12[1,1,3,3,5,5,7,7]
        vpmuldq	%ymm10, %ymm2, %ymm2
        vpmuldq	%ymm11, %ymm3, %ymm3
        vpmuldq	%ymm12, %ymm4, %ymm4
        vpmuldq	%ymm13, %ymm5, %ymm5
        vpmuldq	%ymm2, %ymm0, %ymm10
        vpmuldq	%ymm3, %ymm0, %ymm11
        vpmuldq	%ymm4, %ymm0, %ymm12
        vpmuldq	%ymm5, %ymm0, %ymm13
        vpmuldq	%ymm10, %ymm1, %ymm10
        vpmuldq	%ymm11, %ymm1, %ymm11
        vpmuldq	%ymm12, %ymm1, %ymm12
        vpmuldq	%ymm13, %ymm1, %ymm13
        vpsubq	%ymm10, %ymm2, %ymm2
        vpsubq	%ymm11, %ymm3, %ymm3
        vpsubq	%ymm12, %ymm4, %ymm4
        vpsubq	%ymm13, %ymm5, %ymm5
        vpsrlq	$0x20, %ymm2, %ymm2
        vmovshdup	%ymm4, %ymm4    # ymm4 = ymm4[1,1,3,3,5,5,7,7]
        vpblendd	$0x55, %ymm2, %ymm3, %ymm2 # ymm2 = ymm2[0],ymm3[1],ymm2[2],ymm3[3],ymm2[4],ymm3[5],ymm2[6],ymm3[7]
        vpblendd	$0x55, %ymm4, %ymm5, %ymm4 # ymm4 = ymm4[0],ymm5[1],ymm4[2],ymm5[3],ymm4[4],ymm5[5],ymm4[6],ymm5[7]
        vmovdqa	%ymm2, (%rdi)
        vmovdqa	%ymm4, 0x20(%rdi)
        retq
        .cfi_endproc

#endif /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
        */
