/*
 * Copyright (c) The mlkem-native project authors
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
/*
 * This file is derived from the public domain
 * AVX2 Dilithium implementation @[REF_AVX2].
 */

/* References
 * ==========
 *
 * - [REF_AVX2]
 *   CRYSTALS-Dilithium optimized AVX2 implementation
 *   Bai, Ducas, Kiltz, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehlé
 *   https://github.com/pq-crystals/dilithium/tree/master/avx2
 */

#include "../../../common.h"
#if defined(MLD_ARITH_BACKEND_X86_64_DEFAULT) && \
    !defined(MLD_CONFIG_MULTILEVEL_NO_SHARED)
/* simpasm: header-end */

#include "consts.h"

.macro shuffle8 r0, r1, r2, r3
vperm2i128	$0x20,%ymm\r1,%ymm\r0,%ymm\r2
vperm2i128	$0x31,%ymm\r1,%ymm\r0,%ymm\r3
.endm

.macro shuffle4 r0, r1, r2, r3
vpunpcklqdq	%ymm\r1,%ymm\r0,%ymm\r2
vpunpckhqdq	%ymm\r1,%ymm\r0,%ymm\r3
.endm

.macro shuffle2 r0, r1, r2, r3
#vpsllq		$32,%ymm\r1,%ymm\r2
vmovsldup	%ymm\r1,%ymm\r2
vpblendd	$0xAA,%ymm\r2,%ymm\r0,%ymm\r2
vpsrlq		$32,%ymm\r0,%ymm\r0
#vmovshdup	%ymm\r0,%ymm\r0
vpblendd	$0xAA,%ymm\r1,%ymm\r0,%ymm\r3
.endm

/*
 * Compute l' = l + h, h' = montmul(h - l, zh)
 *
 * Bounds: |l'| <= |l| + |h|
 *         |h'| < MONTMUL_BOUND
 *         (See the end of this file for the exact value of MONTMUL_BOUND)
 */
.macro butterfly l, h, zl0=1, zl1=1, zh0=2, zh1=2
vpsubd		%ymm\l,%ymm\h,%ymm12
vpaddd		%ymm\h,%ymm\l,%ymm\l

vpmuldq		%ymm\zl0,%ymm12,%ymm13
vmovshdup	%ymm12,%ymm\h
vpmuldq		%ymm\zl1,%ymm\h,%ymm14

vpmuldq		%ymm\zh0,%ymm12,%ymm12
vpmuldq		%ymm\zh1,%ymm\h,%ymm\h

vpmuldq		%ymm0,%ymm13,%ymm13
vpmuldq		%ymm0,%ymm14,%ymm14

vpsubd		%ymm13,%ymm12,%ymm12
vpsubd		%ymm14,%ymm\h,%ymm\h

vmovshdup	%ymm12,%ymm12
vpblendd	$0xAA,%ymm\h,%ymm12,%ymm\h
.endm

.macro levels0t5 off
vmovdqa		256*\off+  0(%rdi),%ymm4
vmovdqa		256*\off+ 32(%rdi),%ymm5
vmovdqa		256*\off+ 64(%rdi),%ymm6
vmovdqa	 	256*\off+ 96(%rdi),%ymm7
vmovdqa		256*\off+128(%rdi),%ymm8
vmovdqa		256*\off+160(%rdi),%ymm9
vmovdqa		256*\off+192(%rdi),%ymm10
vmovdqa	 	256*\off+224(%rdi),%ymm11

/* Bounds: |ymm{i}| < q for i in 4...11 */

/* level 0 */
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+296-8*\off-8)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+296-8*\off-8)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	4, 5, 1, 3, 2, 15

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+296-8*\off-40)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+296-8*\off-40)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	6, 7, 1, 3, 2, 15

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+296-8*\off-72)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+296-8*\off-72)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	8, 9, 1, 3, 2, 15

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+296-8*\off-104)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+296-8*\off-104)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	10, 11, 1, 3, 2, 15

/*
 * Bounds: |ymm{i}| < 2q            for i in 4, 6, 8, 10
 *                  < MONTMUL_BOUND for i in 5, 7, 9, 11
 *
 * Note that since 2^31 / q > 256, the sum of all 256 coefficients does not
 * overflow. This allows us to greatly simplify the range analysis by relaxing
 * and unifying the bounds of all coefficients on the same layer. As a concrete
 * example, here we relax the bounds on 5, 7, 9, 11 and conclude that (for all
 * relevant i)
 *
 * Bounds: |ymm{i}| < 2q
 *
 * In all but last of the following levels of butterflies, we do the same
 * relaxation without explicit mention.
 */

/* level 1 */
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168-8*\off-8)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168-8*\off-8)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	4, 6, 1, 3, 2, 15
butterfly	5, 7, 1, 3, 2, 15

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168-8*\off-40)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168-8*\off-40)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	8, 10, 1, 3, 2, 15
butterfly	9, 11, 1, 3, 2, 15

/* Bounds: |ymm{i}| < 4q */

/* level 2 */
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+104-8*\off-8)*4(%rsi),%ymm3
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+104-8*\off-8)*4(%rsi),%ymm15
vmovshdup	%ymm3,%ymm1
vmovshdup	%ymm15,%ymm2
butterfly	4, 8, 1, 3, 2, 15
butterfly	5, 9, 1, 3, 2, 15
butterfly	6, 10, 1, 3, 2, 15
butterfly	7, 11, 1, 3, 2, 15

/* Bounds: |ymm{i}| < 8q */

/* level 3 */
shuffle2	4, 5, 3, 5
shuffle2	6, 7, 4, 7
shuffle2	8, 9, 6, 9
shuffle2	10, 11, 8, 11

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+72-8*\off-8)*4(%rsi),%ymm1
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+72-8*\off-8)*4(%rsi),%ymm2
butterfly	3, 5
butterfly	4, 7
butterfly	6, 9
butterfly	8, 11

/* Bounds: |ymm{i}| < 16q */

/* level 4 */
shuffle4	3, 4, 10, 4
shuffle4	6, 8, 3, 8
shuffle4	5, 7, 6, 7
shuffle4	9, 11, 5, 11

vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+40-8*\off-8)*4(%rsi),%ymm1
vpermq		$0x1B,(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+40-8*\off-8)*4(%rsi),%ymm2
butterfly	10, 4
butterfly	3, 8
butterfly	6, 7
butterfly	5, 11

/* Bounds: |ymm{i}| < 32q */

/* level 5 */
shuffle8	10, 3, 9, 3
shuffle8	6, 5, 10, 5
shuffle8	4, 8, 6, 8
shuffle8	7, 11, 4, 11

vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+7-\off)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+7-\off)*4(%rsi),%ymm2
butterfly	9, 3
butterfly	10, 5
butterfly	6, 8
butterfly	4, 11

/* Bounds: |ymm{i}| < 64q */

vmovdqa		%ymm9,256*\off+  0(%rdi)
vmovdqa		%ymm10,256*\off+ 32(%rdi)
vmovdqa		%ymm6,256*\off+ 64(%rdi)
vmovdqa		%ymm4,256*\off+ 96(%rdi)
vmovdqa		%ymm3,256*\off+128(%rdi)
vmovdqa		%ymm5,256*\off+160(%rdi)
vmovdqa		%ymm8,256*\off+192(%rdi)
vmovdqa		%ymm11,256*\off+224(%rdi)
.endm

.macro levels6t7 off
vmovdqa		  0+32*\off(%rdi),%ymm4
vmovdqa		128+32*\off(%rdi),%ymm5
vmovdqa		256+32*\off(%rdi),%ymm6
vmovdqa		384+32*\off(%rdi),%ymm7
vmovdqa		512+32*\off(%rdi),%ymm8
vmovdqa		640+32*\off(%rdi),%ymm9
vmovdqa		768+32*\off(%rdi),%ymm10
vmovdqa		896+32*\off(%rdi),%ymm11

/* level 6 */
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+3)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+3)*4(%rsi),%ymm2
butterfly	4, 6
butterfly	5, 7

vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+2)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+2)*4(%rsi),%ymm2
butterfly	8, 10
butterfly	9, 11

/* Bounds: |ymm{i}| < 128q */

/* level 7 */
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+0)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+0)*4(%rsi),%ymm2

butterfly	4, 8
butterfly	5, 9
butterfly	6, 10
butterfly	7, 11

/*
 * Bounds: |ymm{i}| < 256q          for i in 4...7
 *                  < MONTMUL_BOUND for i in 8...11
 */

vmovdqa         %ymm8,512+32*\off(%rdi)
vmovdqa         %ymm9,640+32*\off(%rdi)
vmovdqa         %ymm10,768+32*\off(%rdi)
vmovdqa         %ymm11,896+32*\off(%rdi)

/*
 * In order to (a) remove the factor of 256 arising from the 256-point INTT
 * butterflies and (b) transform the output into Montgomery domain, we need to
 * multiply all coefficients by 2^32/256.
 *
 * For ymm{8,9,10,11}, the scaling has been merged into the last butterfly, so
 * only ymm{4,5,6,7} need to be scaled explicitly.
 *
 * The scaling is achieved by computing montmul(-, 41978 = 2**(64-8) mod q)).
 *
 * Bounds: |ymm{i}| < 256q for i in 4...7
 */

vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_8XDIV_QINV)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_8XDIV)*4(%rsi),%ymm2
vpmuldq		%ymm1,%ymm4,%ymm12
vpmuldq		%ymm1,%ymm5,%ymm13
vmovshdup	%ymm4,%ymm8
vmovshdup	%ymm5,%ymm9
vpmuldq		%ymm1,%ymm8,%ymm14
vpmuldq		%ymm1,%ymm9,%ymm15
vpmuldq		%ymm2,%ymm4,%ymm4
vpmuldq		%ymm2,%ymm5,%ymm5
vpmuldq		%ymm2,%ymm8,%ymm8
vpmuldq		%ymm2,%ymm9,%ymm9
vpmuldq		%ymm0,%ymm12,%ymm12
vpmuldq		%ymm0,%ymm13,%ymm13
vpmuldq		%ymm0,%ymm14,%ymm14
vpmuldq		%ymm0,%ymm15,%ymm15
vpsubd		%ymm12,%ymm4,%ymm4
vpsubd		%ymm13,%ymm5,%ymm5
vpsubd		%ymm14,%ymm8,%ymm8
vpsubd		%ymm15,%ymm9,%ymm9
vmovshdup	%ymm4,%ymm4
vmovshdup	%ymm5,%ymm5
vpblendd	$0xAA,%ymm8,%ymm4,%ymm4
vpblendd	$0xAA,%ymm9,%ymm5,%ymm5

vpmuldq		%ymm1,%ymm6,%ymm12
vpmuldq		%ymm1,%ymm7,%ymm13
vmovshdup	%ymm6,%ymm8
vmovshdup	%ymm7,%ymm9
vpmuldq		%ymm1,%ymm8,%ymm14
vpmuldq		%ymm1,%ymm9,%ymm15
vpmuldq		%ymm2,%ymm6,%ymm6
vpmuldq		%ymm2,%ymm7,%ymm7
vpmuldq		%ymm2,%ymm8,%ymm8
vpmuldq		%ymm2,%ymm9,%ymm9
vpmuldq		%ymm0,%ymm12,%ymm12
vpmuldq		%ymm0,%ymm13,%ymm13
vpmuldq		%ymm0,%ymm14,%ymm14
vpmuldq		%ymm0,%ymm15,%ymm15
vpsubd		%ymm12,%ymm6,%ymm6
vpsubd		%ymm13,%ymm7,%ymm7
vpsubd		%ymm14,%ymm8,%ymm8
vpsubd		%ymm15,%ymm9,%ymm9
vmovshdup	%ymm6,%ymm6
vmovshdup	%ymm7,%ymm7
vpblendd	$0xAA,%ymm8,%ymm6,%ymm6
vpblendd	$0xAA,%ymm9,%ymm7,%ymm7

/* Bounds: |ymm{i}| < MONTMUL_BOUND for i in 4...7 */

/*
 * In the following we show that |montmul(a, b)| < MONTMUL_BOUND := ceil(3q/4),
 * given that a fits inside int32_t (thus |a| <= R/2) and b is signed canonical
 * (in the range -(Q-1)/2...(Q-1)/2).
 *
 * In Section 2.2 of https://eprint.iacr.org/2023/1962, they showed that b being
 * signed canonical implies:
 *
 *   |montmul(a, b)| <= (|a| (q/2) + (R/2) q) / R = (q/2) (1 + |a|/R).
 *
 * From this, simple computation gives |montmul(a, b)| <= 3q/4 < ceil(3q/4).
 *
 * See test/test_bounds.py for more empirical evidence (and some minor technical
 * details).
 *
 * TODO: Use proper citation. Currently, citations within asm can cause linter
 *       to complain about unused citation, because comments are not preserved
 *       after simpasm.
 */

vmovdqa         %ymm4,  0+32*\off(%rdi)
vmovdqa         %ymm5,128+32*\off(%rdi)
vmovdqa         %ymm6,256+32*\off(%rdi)
vmovdqa         %ymm7,384+32*\off(%rdi)
.endm

.text
.balign 4
.global MLD_ASM_NAMESPACE(invntt_avx2)
MLD_ASM_FN_SYMBOL(invntt_avx2)

vmovdqa MLD_AVX2_BACKEND_DATA_OFFSET_8XQ*4(%rsi),%ymm0

levels0t5	0
levels0t5	1
levels0t5	2
levels0t5	3

levels6t7	0
levels6t7	1
levels6t7	2
levels6t7	3

ret
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
        */
