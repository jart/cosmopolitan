/*
 * Copyright (c) The mlkem-native project authors
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
 * Compute l' = l + montmul(h, zh), h' = l - montmul(h, zh).
 *
 * Bounds: |l'|, |h'| < |l| + MONTMUL_BOUND < |l| + q
 *         (See the end of dev/x86_64/src/intt.S for the exact value of
 *         MONTMUL_BOUND)
 *
 * In conclusion, the magnitudes of all coefficients grow by at most q after
 * each layer.
 */
.macro butterfly l, h, zl0=1, zl1=1, zh0=2, zh1=2
vpmuldq		%ymm\zl0,%ymm\h,%ymm13
vmovshdup	%ymm\h,%ymm12
vpmuldq		%ymm\zl1,%ymm12,%ymm14

vpmuldq		%ymm\zh0,%ymm\h,%ymm\h
vpmuldq		%ymm\zh1,%ymm12,%ymm12

vpmuldq		%ymm0,%ymm13,%ymm13
vpmuldq		%ymm0,%ymm14,%ymm14

vmovshdup	%ymm\h,%ymm\h
vpblendd	$0xAA,%ymm12,%ymm\h,%ymm\h /* mulhi(h, zh) */

/*
 * Originally, mulhi(h, zh) should be subtracted by mulhi(q, mullo(h, zl)) in
 * order to complete computing
 *
 *   montmul(h, zh) = mulhi(h, zh) - mulhi(q, mullo(h, zl)).
 *
 * Here, since mulhi(q, mullo(h, zl)) has not been computed yet, this task is
 * delayed until the end of the butterfly. Note that whether any of the
 * remaining add/subs overflow or not doesn't affect the final value of h' or l'
 * at all, because associativity holds unconditionally.
 */
vpsubd      %ymm\h,%ymm\l,%ymm12 /* l - mulhi(h, zh)
                                  * = h' - mulhi(q, mullo(h, zl)) */
/*
 * VEX Encoding Optimization for Platform-Independent Code
 *
 * Some assemblers (notably clang) will automatically swap operands of
 * commutative instructions like VPADDD to use shorter encodings, while others
 * (like gcc) may not. This causes different machine code across platforms.
 *
 * VEX prefixes come in two forms:
 * - 2-byte VEX (0xC5): Can only be used when the ModR/M.rm operand is ymm0-7
 * - 3-byte VEX (0xC4): Required when ModR/M.rm operand is ymm8-15
 *
 * When one operand is in ymm0-7 and another is in ymm8-15, we explicitly
 * place the lower-numbered register (ymm0-7) as the second source operand
 * to enable the 2-byte VEX encoding. Since VPADDD is commutative, this
 * produces identical results while ensuring consistent machine code across
 * different assemblers.
 *
 * Example:
 *   VPADDD ymm4, ymm4, ymm8  -> 3-byte VEX (0xC4 0xC1 0x5D 0xFE 0xE0)
 *   VPADDD ymm4, ymm8, ymm4  -> 2-byte VEX (0xC5 0xBD 0xFE 0xE4) ✓ preferred
 */
.if (\l < 8) && (\h >= 8)
vpaddd      %ymm\l,%ymm\h,%ymm\l /* l + mulhi(h, zh)
                                  * = l' + mulhi(q, mullo(h, zl)) */
.else                            
vpaddd      %ymm\h,%ymm\l,%ymm\l
.endif

vmovshdup	%ymm13,%ymm13
vpblendd	$0xAA,%ymm14,%ymm13,%ymm13 /* mulhi(q, mullo(h, zl)) */

/* Finish the delayed task mentioned above */
vpaddd		%ymm13,%ymm12,%ymm\h /* h' */
vpsubd		%ymm13,%ymm\l,%ymm\l /* l' */
.endm

.macro levels0t1 off
/* level 0 */
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+1)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+1)*4(%rsi),%ymm2

vmovdqa		  0+32*\off(%rdi),%ymm4
vmovdqa		128+32*\off(%rdi),%ymm5
vmovdqa		256+32*\off(%rdi),%ymm6
vmovdqa	 	384+32*\off(%rdi),%ymm7
vmovdqa		512+32*\off(%rdi),%ymm8
vmovdqa		640+32*\off(%rdi),%ymm9
vmovdqa		768+32*\off(%rdi),%ymm10
vmovdqa	 	896+32*\off(%rdi),%ymm11

/* Bounds: |ymm{i}| < q */

butterfly	4, 8
butterfly	5, 9
butterfly	6, 10
butterfly	7, 11

/* Bounds: |ymm{i}| < 2q */

/* level 1 */
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+2)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+2)*4(%rsi),%ymm2
butterfly	4, 6
butterfly	5, 7

vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+3)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+3)*4(%rsi),%ymm2
butterfly	8, 10
butterfly	9, 11

/* Bounds: |ymm{i}| < 3q */

vmovdqa		%ymm4,  0+32*\off(%rdi)
vmovdqa		%ymm5,128+32*\off(%rdi)
vmovdqa		%ymm6,256+32*\off(%rdi)
vmovdqa		%ymm7,384+32*\off(%rdi)
vmovdqa		%ymm8,512+32*\off(%rdi)
vmovdqa		%ymm9,640+32*\off(%rdi)
vmovdqa		%ymm10,768+32*\off(%rdi)
vmovdqa		%ymm11,896+32*\off(%rdi)
.endm

.macro levels2t7 off
/* level 2 */
vmovdqa		256*\off+  0(%rdi),%ymm4
vmovdqa		256*\off+ 32(%rdi),%ymm5
vmovdqa		256*\off+ 64(%rdi),%ymm6
vmovdqa	 	256*\off+ 96(%rdi),%ymm7
vmovdqa		256*\off+128(%rdi),%ymm8
vmovdqa		256*\off+160(%rdi),%ymm9
vmovdqa		256*\off+192(%rdi),%ymm10
vmovdqa	 	256*\off+224(%rdi),%ymm11

vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+4+\off)*4(%rsi),%ymm1
vpbroadcastd	(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+4+\off)*4(%rsi),%ymm2

butterfly	4, 8
butterfly	5, 9
butterfly	6, 10
butterfly	7, 11

shuffle8	4, 8, 3, 8
shuffle8	5, 9, 4, 9
shuffle8	6, 10, 5, 10
shuffle8	7, 11, 6, 11

/* Bounds: |ymm{i}| < 4q */

/* level 3 */
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+8+8*\off)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+8+8*\off)*4(%rsi),%ymm2

butterfly	3, 5
butterfly	8, 10
butterfly	4, 6
butterfly	9, 11

shuffle4	3, 5, 7, 5
shuffle4	8, 10, 3, 10
shuffle4	4, 6, 8, 6
shuffle4	9, 11, 4, 11

/* Bounds: |ymm{i}| < 5q */

/* level 4 */
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+40+8*\off)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+40+8*\off)*4(%rsi),%ymm2

butterfly	7, 8
butterfly	5, 6
butterfly	3, 4
butterfly	10, 11

shuffle2	7, 8, 9, 8
shuffle2	5, 6, 7, 6
shuffle2	3, 4, 5, 4
shuffle2	10, 11, 3, 11

/* Bounds: |ymm{i}| < 6q */

/* level 5 */
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+72+8*\off)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+72+8*\off)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15

butterfly	9, 5, 1, 10, 2, 15
butterfly	8, 4, 1, 10, 2, 15
butterfly	7, 3, 1, 10, 2, 15
butterfly	6, 11, 1, 10, 2, 15

/* Bounds: |ymm{i}| < 7q */

/* level 6 */
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+104+8*\off)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+104+8*\off)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	9, 7, 1, 10, 2, 15
butterfly	8, 6, 1, 10, 2, 15

vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+104+8*\off+32)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+104+8*\off+32)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	5, 3, 1, 10, 2, 15
butterfly	4, 11, 1, 10, 2, 15

/* Bounds: |ymm{i}| < 8q */

/* level 7 */
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168+8*\off)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168+8*\off)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	9, 8, 1, 10, 2, 15

vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168+8*\off+32)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168+8*\off+32)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	7, 6, 1, 10, 2, 15

vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168+8*\off+64)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168+8*\off+64)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	5, 4, 1, 10, 2, 15

vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS_QINV+168+8*\off+96)*4(%rsi),%ymm1
vmovdqa		(MLD_AVX2_BACKEND_DATA_OFFSET_ZETAS+168+8*\off+96)*4(%rsi),%ymm2
vpsrlq		$32,%ymm1,%ymm10
vmovshdup	%ymm2,%ymm15
butterfly	3, 11, 1, 10, 2, 15

/* Bounds: |ymm{i}| < 9q */

vmovdqa		%ymm9,256*\off+  0(%rdi)
vmovdqa		%ymm8,256*\off+ 32(%rdi)
vmovdqa		%ymm7,256*\off+ 64(%rdi)
vmovdqa		%ymm6,256*\off+ 96(%rdi)
vmovdqa		%ymm5,256*\off+128(%rdi)
vmovdqa		%ymm4,256*\off+160(%rdi)
vmovdqa		%ymm3,256*\off+192(%rdi)
vmovdqa		%ymm11,256*\off+224(%rdi)
.endm

.text
.balign 4
.global MLD_ASM_NAMESPACE(ntt_avx2)
MLD_ASM_FN_SYMBOL(ntt_avx2)
vmovdqa		MLD_AVX2_BACKEND_DATA_OFFSET_8XQ*4(%rsi),%ymm0

levels0t1	0
levels0t1	1
levels0t1	2
levels0t1	3

levels2t7	0
levels2t7	1
levels2t7	2
levels2t7	3

ret
/* simpasm: footer-start */
#endif /* MLD_ARITH_BACKEND_X86_64_DEFAULT && !MLD_CONFIG_MULTILEVEL_NO_SHARED \
        */
