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
/* simpasm: header-end */

#include "consts.h"

        .intel_syntax noprefix
        .text

.macro pointwise off
// Load
        vmovdqa ymm6, [rsi + \off]
        vmovdqa ymm8, [rsi + \off + 32]
        vmovdqa ymm10, [rdx + \off]
        vmovdqa ymm12, [rdx + \off + 32]
        vpsrlq  ymm7, ymm6, 32
        vpsrlq  ymm9, ymm8, 32
        vmovshdup ymm11, ymm10
        vmovshdup ymm13, ymm12
        /*
         * ymm{i} stores a's coefficients for i in 6...9, and b's coefficients
         * for i in 10...13.
         *
         * Bounds: |ymm{i}| < q  for i in 6...9
         *                  < 9q for i in 10...13
         */

// Multiply
        vpmuldq ymm6, ymm6, ymm10
        vpmuldq ymm7, ymm7, ymm11
        vpmuldq ymm8, ymm8, ymm12
        vpmuldq ymm9, ymm9, ymm13
        /* Bounds: |ymm{i}| < 9q^2 for i in 6...9 */
.endm

.macro acc
        vpaddq  ymm2, ymm6, ymm2
        vpaddq  ymm3, ymm7, ymm3
        vpaddq  ymm4, ymm8, ymm4
        vpaddq  ymm5, ymm9, ymm5
.endm

/*
 * void mld_pointwise_acc_l4_avx2(__m256i *c, const __m256i *a, const __m256i *b, const __m256i *qdata)
 *
 * Pointwise multiplication with accumulation across multiple polynomial vectors
 *
 * Arguments:
 *   rdi: pointer to output polynomial c
 *   rsi: pointer to input polynomial a (multiple vectors)
 *   rdx: pointer to input polynomial b (multiple vectors)
 *   rcx: pointer to qdata constants
 */
        .balign 4
        .global MLD_ASM_NAMESPACE(pointwise_acc_l4_avx2)
MLD_ASM_FN_SYMBOL(pointwise_acc_l4_avx2)

// Load constants
        vmovdqa ymm0, [rcx + (MLD_AVX2_BACKEND_DATA_OFFSET_8XQINV)*4]
        vmovdqa ymm1, [rcx + (MLD_AVX2_BACKEND_DATA_OFFSET_8XQ)*4]

        xor     eax, eax
pointwise_acc_l4_avx2_looptop2:
        pointwise 0

// Move
        vmovdqa ymm2, ymm6
        vmovdqa ymm3, ymm7
        vmovdqa ymm4, ymm8
        vmovdqa ymm5, ymm9
        /* Bounds: |ymm{i}| < 9q^2 */

        pointwise 1024
        acc
        /* Bounds: |ymm{i}| < 18q^2 */

        pointwise 2048
        acc
        /* Bounds: |ymm{i}| < 27q^2 */

        pointwise 3072
        acc
        /* Bounds: |ymm{i}| < 36q^2 < MONTGOMERY_REDUCE_STRONG_DOMAIN_MAX */

// Reduce
        vpmuldq ymm6, ymm0, ymm2
        vpmuldq ymm7, ymm0, ymm3
        vpmuldq ymm8, ymm0, ymm4
        vpmuldq ymm9, ymm0, ymm5
        vpmuldq ymm6, ymm1, ymm6
        vpmuldq ymm7, ymm1, ymm7
        vpmuldq ymm8, ymm1, ymm8
        vpmuldq ymm9, ymm1, ymm9
        vpsubq  ymm2, ymm2, ymm6
        vpsubq  ymm3, ymm3, ymm7
        vpsubq  ymm4, ymm4, ymm8
        vpsubq  ymm5, ymm5, ymm9
        /*
         * All coefficients are Montgomery-reduced, resulting in
         *
         * Bounds: |ymm{i}| < q for i in 2...5
         *
         * See description of mld_montgomery_reduce() in mldsa/src/reduce.h.
         */

// Store
        vpsrlq  ymm2, ymm2, 32
        vmovshdup ymm4, ymm4
        vpblendd ymm2, ymm2, ymm3, 0xAA
        vpblendd ymm4, ymm4, ymm5, 0xAA

        vmovdqa [rdi], ymm2
        vmovdqa [rdi + 32], ymm4

        add     rsi, 64
        add     rdx, 64
        add     rdi, 64
        add     eax, 1
        cmp     eax, 16
        jb      pointwise_acc_l4_avx2_looptop2

        ret
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
        */
