// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include "libc/macros.h"

// aarch64 atomics compiler runtime
//
// armv8.1 introduced atomic instructions that go considerably faster.
// you can pass the -mno-outline-atomics flag to the compiler to avoid
// this runtime, however that'll go slower.

.arch armv8-a+lse

.macro .prvfn name
	.privileged
	.balign	16
\name:
.endm

.macro .begfn name
	.section .text.\name,"ax",%progbits
	.balign	16
	.ftrace1
\name:
	.ftrace2
.endm

.macro	jnatom	label
	adrp	x16,__aarch64_have_lse_atomics
	ldrb	w16,[x16,:lo12:__aarch64_have_lse_atomics]
	cbz	w16,\label
.endm


.begfn __aarch64_swp1_relax
	jnatom	1f
	swpb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	stxrb	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp1_relax,globl

.begfn __aarch64_swp1_acq
	jnatom	1f
	swpab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	stxrb	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp1_acq,globl

.begfn __aarch64_swp1_rel
	jnatom	1f
	swplb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	stlxrb	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp1_rel,globl

.begfn __aarch64_swp1_acq_rel
	jnatom	1f
	swpalb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	stlxrb	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp1_acq_rel,globl

.begfn __aarch64_swp1_sync
	jnatom	1f
	swpab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	stxrb	w17,w16,[x1]
	cbnz	w17,0b
	dmb	ish
	ret
.endfn __aarch64_swp1_sync,globl


.begfn __aarch64_cas1_relax
	jnatom	1f
	casb	w0,w1,[x2]
	ret
1:	uxtb	w16,w0
0:	ldxrb	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxrb	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas1_relax,globl

.begfn __aarch64_cas1_acq
	jnatom	1f
	casab	w0,w1,[x2]
	ret
1:	uxtb	w16,w0
0:	ldaxrb	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxrb	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas1_acq,globl

.begfn __aarch64_cas1_rel
	jnatom	1f
	caslb	w0,w1,[x2]
	ret
1:	uxtb	w16,w0
0:	ldxrb	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrb	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas1_rel,globl

.begfn __aarch64_cas1_acq_rel
	jnatom	1f
	casalb	w0,w1,[x2]
	ret
1:	uxtb	w16,w0
0:	ldaxrb	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrb	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas1_acq_rel,globl

.begfn __aarch64_cas1_sync
	jnatom	1f
	casalb	w0,w1,[x2]
	ret
1:	uxtb	w16,w0
0:	ldxrb	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrb	w17,w1,[x2]
	cbnz	w17,0b
1:	dmb	ish
	ret
.endfn __aarch64_cas1_sync,globl


.begfn __aarch64_ldadd1_relax
	jnatom	1f
	ldaddb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	add	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd1_relax,globl

.begfn __aarch64_ldadd1_acq
	jnatom	1f
	ldaddab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	add	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd1_acq,globl

.begfn __aarch64_ldadd1_rel
	jnatom	1f
	ldaddlb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	add	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd1_rel,globl

.begfn __aarch64_ldadd1_acq_rel
	jnatom	1f
	ldaddalb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	add	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd1_acq_rel,globl

.begfn __aarch64_ldadd1_sync
	jnatom	1f
	ldaddalb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	add	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldadd1_sync,globl


.begfn __aarch64_ldset1_relax
	jnatom	1f
	ldsetb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	orr	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset1_relax,globl

.begfn __aarch64_ldset1_acq
	jnatom	1f
	ldsetab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	orr	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset1_acq,globl

.begfn __aarch64_ldset1_rel
	jnatom	1f
	ldsetlb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	orr	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset1_rel,globl

.begfn __aarch64_ldset1_acq_rel
	jnatom	1f
	ldsetalb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	orr	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset1_acq_rel,globl

.begfn __aarch64_ldset1_sync
	jnatom	1f
	ldsetalb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	orr	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldset1_sync,globl


.begfn __aarch64_ldclr1_relax
	jnatom	1f
	ldclrb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	bic	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr1_relax,globl

.begfn __aarch64_ldclr1_acq
	jnatom	1f
	ldclrab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	bic	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr1_acq,globl

.begfn __aarch64_ldclr1_rel
	jnatom	1f
	ldclrlb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	bic	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr1_rel,globl

.begfn __aarch64_ldclr1_acq_rel
	jnatom	1f
	ldclralb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	bic	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr1_acq_rel,globl

.begfn __aarch64_ldclr1_sync
	jnatom	1f
	ldclralb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	bic	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldclr1_sync,globl


.begfn __aarch64_ldeor1_relax
	jnatom	1f
	ldeorb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	eor	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor1_relax,globl

.begfn __aarch64_ldeor1_acq
	jnatom	1f
	ldeorab	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	eor	w17,w0,w16
	stxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor1_acq,globl

.begfn __aarch64_ldeor1_rel
	jnatom	1f
	ldeorlb	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	eor	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor1_rel,globl

.begfn __aarch64_ldeor1_acq_rel
	jnatom	1f
	ldeoralb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrb	w0,[x1]
	eor	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor1_acq_rel,globl

.begfn __aarch64_ldeor1_sync
	jnatom	1f
	ldeoralb w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrb	w0,[x1]
	eor	w17,w0,w16
	stlxrb	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldeor1_sync,globl


.begfn __aarch64_swp2_relax
	jnatom	1f
	swph	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	stxrh	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp2_relax,globl

.begfn __aarch64_swp2_acq
	jnatom	1f
	swpah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	stxrh	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp2_acq,globl

.begfn __aarch64_swp2_rel
	jnatom	1f
	swplh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	stlxrh	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp2_rel,globl

.begfn __aarch64_swp2_acq_rel
	jnatom	1f
	swpalh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	stlxrh	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp2_acq_rel,globl

.begfn __aarch64_swp2_sync
	jnatom	1f
	swpah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	stxrh	w17,w16,[x1]
	cbnz	w17,0b
	dmb	ish
	ret
.endfn __aarch64_swp2_sync,globl


.begfn __aarch64_cas2_relax
	jnatom	1f
	cash	w0,w1,[x2]
	ret
1:	uxth	w16,w0
0:	ldxrh	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxrh	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas2_relax,globl

.begfn __aarch64_cas2_acq
	jnatom	1f
	casah	w0,w1,[x2]
	ret
1:	uxth	w16,w0
0:	ldaxrh	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxrh	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas2_acq,globl

.begfn __aarch64_cas2_rel
	jnatom	1f
	caslh	w0,w1,[x2]
	ret
1:	uxth	w16,w0
0:	ldxrh	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrh	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas2_rel,globl

.begfn __aarch64_cas2_acq_rel
	jnatom	1f
	casalh	w0,w1,[x2]
	ret
1:	uxth	w16,w0
0:	ldaxrh	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrh	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas2_acq_rel,globl

.begfn __aarch64_cas2_sync
	jnatom	1f
	casalh	w0,w1,[x2]
	ret
1:	uxth	w16,w0
0:	ldxrh	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxrh	w17,w1,[x2]
	cbnz	w17,0b
1:	dmb	ish
	ret
.endfn __aarch64_cas2_sync,globl


.begfn __aarch64_ldadd2_relax
	jnatom	1f
	ldaddh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	add	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd2_relax,globl

.begfn __aarch64_ldadd2_acq
	jnatom	1f
	ldaddah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	add	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd2_acq,globl

.begfn __aarch64_ldadd2_rel
	jnatom	1f
	ldaddlh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	add	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd2_rel,globl

.begfn __aarch64_ldadd2_acq_rel
	jnatom	1f
	ldaddalh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	add	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd2_acq_rel,globl

.begfn __aarch64_ldadd2_sync
	jnatom	1f
	ldaddalh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	add	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldadd2_sync,globl


.begfn __aarch64_ldset2_relax
	jnatom	1f
	ldseth	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	orr	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset2_relax,globl

.begfn __aarch64_ldset2_acq
	jnatom	1f
	ldsetah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	orr	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset2_acq,globl

.begfn __aarch64_ldset2_rel
	jnatom	1f
	ldsetlh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	orr	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset2_rel,globl

.begfn __aarch64_ldset2_acq_rel
	jnatom	1f
	ldsetalh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	orr	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset2_acq_rel,globl

.begfn __aarch64_ldset2_sync
	jnatom	1f
	ldsetalh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	orr	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldset2_sync,globl


.begfn __aarch64_ldclr2_relax
	jnatom	1f
	ldclrh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	bic	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr2_relax,globl

.begfn __aarch64_ldclr2_acq
	jnatom	1f
	ldclrah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	bic	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr2_acq,globl

.begfn __aarch64_ldclr2_rel
	jnatom	1f
	ldclrlh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	bic	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr2_rel,globl

.begfn __aarch64_ldclr2_acq_rel
	jnatom	1f
	ldclralh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	bic	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr2_acq_rel,globl

.begfn __aarch64_ldclr2_sync
	jnatom	1f
	ldclralh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	bic	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldclr2_sync,globl


.begfn __aarch64_ldeor2_relax
	jnatom	1f
	ldeorh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	eor	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor2_relax,globl

.begfn __aarch64_ldeor2_acq
	jnatom	1f
	ldeorah	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	eor	w17,w0,w16
	stxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor2_acq,globl

.begfn __aarch64_ldeor2_rel
	jnatom	1f
	ldeorlh	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	eor	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor2_rel,globl

.begfn __aarch64_ldeor2_acq_rel
	jnatom	1f
	ldeoralh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxrh	w0,[x1]
	eor	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor2_acq_rel,globl

.begfn __aarch64_ldeor2_sync
	jnatom	1f
	ldeoralh w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxrh	w0,[x1]
	eor	w17,w0,w16
	stlxrh	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldeor2_sync,globl


.begfn __aarch64_swp4_relax
	jnatom	1f
	swp	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	stxr	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp4_relax,globl

.begfn __aarch64_swp4_acq
	jnatom	1f
	swpa	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	stxr	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp4_acq,globl

.begfn __aarch64_swp4_rel
	jnatom	1f
	swpl	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	stlxr	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp4_rel,globl

.begfn __aarch64_swp4_acq_rel
	jnatom	1f
	swpal	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	stlxr	w17,w16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp4_acq_rel,globl

.begfn __aarch64_swp4_sync
	jnatom	1f
	swpa	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	stxr	w17,w16,[x1]
	cbnz	w17,0b
	dmb	ish
	ret
.endfn __aarch64_swp4_sync,globl


.begfn __aarch64_cas4_relax
	jnatom	1f
	cas	w0,w1,[x2]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxr	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas4_relax,globl

.begfn __aarch64_cas4_acq
	jnatom	1f
	casa	w0,w1,[x2]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x2]
	cmp	w0,w16
	bne	1f
	stxr	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas4_acq,globl

.begfn __aarch64_cas4_rel
	jnatom	1f
	casl	w0,w1,[x2]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxr	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas4_rel,globl

.begfn __aarch64_cas4_acq_rel
	jnatom	1f
	casal	w0,w1,[x2]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxr	w17,w1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas4_acq_rel,globl

.begfn __aarch64_cas4_sync
	jnatom	1f
	casal	w0,w1,[x2]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x2]
	cmp	w0,w16
	bne	1f
	stlxr	w17,w1,[x2]
	cbnz	w17,0b
1:	dmb	ish
	ret
.endfn __aarch64_cas4_sync,globl


.begfn __aarch64_ldadd4_relax
	jnatom	1f
	ldadd	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	add	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd4_relax,globl

.begfn __aarch64_ldadd4_acq
	jnatom	1f
	ldadda	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	add	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd4_acq,globl

.begfn __aarch64_ldadd4_rel
	jnatom	1f
	ldaddl	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	add	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd4_rel,globl

.begfn __aarch64_ldadd4_acq_rel
	jnatom	1f
	ldaddal	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	add	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd4_acq_rel,globl

.begfn __aarch64_ldadd4_sync
	jnatom	1f
	ldaddal	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	add	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldadd4_sync,globl


.begfn __aarch64_ldset4_relax
	jnatom	1f
	ldset	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	orr	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset4_relax,globl

.begfn __aarch64_ldset4_acq
	jnatom	1f
	ldseta	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	orr	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset4_acq,globl

.begfn __aarch64_ldset4_rel
	jnatom	1f
	ldsetl	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	orr	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset4_rel,globl

.begfn __aarch64_ldset4_acq_rel
	jnatom	1f
	ldsetal	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	orr	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset4_acq_rel,globl

.begfn __aarch64_ldset4_sync
	jnatom	1f
	ldsetal	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	orr	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldset4_sync,globl


.begfn __aarch64_ldclr4_relax
	jnatom	1f
	ldclr	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	bic	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr4_relax,globl

.begfn __aarch64_ldclr4_acq
	jnatom	1f
	ldclra	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	bic	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr4_acq,globl

.begfn __aarch64_ldclr4_rel
	jnatom	1f
	ldclrl	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	bic	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr4_rel,globl

.begfn __aarch64_ldclr4_acq_rel
	jnatom	1f
	ldclral	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	bic	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr4_acq_rel,globl

.begfn __aarch64_ldclr4_sync
	jnatom	1f
	ldclral	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	bic	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldclr4_sync,globl


.begfn __aarch64_ldeor4_relax
	jnatom	1f
	ldeor	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	eor	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor4_relax,globl

.begfn __aarch64_ldeor4_acq
	jnatom	1f
	ldeora	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	eor	w17,w0,w16
	stxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor4_acq,globl

.begfn __aarch64_ldeor4_rel
	jnatom	1f
	ldeorl	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	eor	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor4_rel,globl

.begfn __aarch64_ldeor4_acq_rel
	jnatom	1f
	ldeoral	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldaxr	w0,[x1]
	eor	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor4_acq_rel,globl

.begfn __aarch64_ldeor4_sync
	jnatom	1f
	ldeoral	w0,w0,[x1]
	ret
1:	mov	w16,w0
0:	ldxr	w0,[x1]
	eor	w17,w0,w16
	stlxr	w15,w17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldeor4_sync,globl


.begfn __aarch64_swp8_relax
	jnatom	1f
	swp	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp8_relax,globl

.begfn __aarch64_swp8_acq
	jnatom	1f
	swpa	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp8_acq,globl

.begfn __aarch64_swp8_rel
	jnatom	1f
	swpl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stlxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp8_rel,globl

.begfn __aarch64_swp8_acq_rel
	jnatom	1f
	swpal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	stlxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp8_acq_rel,globl

.begfn __aarch64_swp8_sync
	jnatom	1f
	swpa	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	dmb	ish
	ret
.endfn __aarch64_swp8_sync,globl


.prvfn __aarch64_cas8_relax
	jnatom	1f
	cas	x0,x1,[x2]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x2]
	cmp	x0,x16
	bne	1f
	stxr	w17,x1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas8_relax,globl

.prvfn __aarch64_cas8_acq
	jnatom	1f
	casa	x0,x1,[x2]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x2]
	cmp	x0,x16
	bne	1f
	stxr	w17,x1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas8_acq,globl

.prvfn __aarch64_cas8_rel
	jnatom	1f
	casl	x0,x1,[x2]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x2]
	cmp	x0,x16
	bne	1f
	stlxr	w17,x1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas8_rel,globl

.begfn __aarch64_cas8_acq_rel
	jnatom	1f
	casal	x0,x1,[x2]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x2]
	cmp	x0,x16
	bne	1f
	stlxr	w17,x1,[x2]
	cbnz	w17,0b
1:	ret
.endfn __aarch64_cas8_acq_rel,globl

.begfn __aarch64_cas8_sync
	jnatom	1f
	casal	x0,x1,[x2]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x2]
	cmp	x0,x16
	bne	1f
	stlxr	w17,x1,[x2]
	cbnz	w17,0b
1:	dmb	ish
	ret
.endfn __aarch64_cas8_sync,globl


.begfn __aarch64_ldadd8_relax
	jnatom	1f
	ldadd	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd8_relax,globl

.begfn __aarch64_ldadd8_acq
	jnatom	1f
	ldadda	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	add	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd8_acq,globl

.begfn __aarch64_ldadd8_rel
	jnatom	1f
	ldaddl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd8_rel,globl

.begfn __aarch64_ldadd8_acq_rel
	jnatom	1f
	ldaddal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd8_acq_rel,globl

.begfn __aarch64_ldadd8_sync
	jnatom	1f
	ldaddal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldadd8_sync,globl


.begfn __aarch64_ldset8_relax
	jnatom	1f
	ldset	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset8_relax,globl

.begfn __aarch64_ldset8_acq
	jnatom	1f
	ldseta	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	orr	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset8_acq,globl

.begfn __aarch64_ldset8_rel
	jnatom	1f
	ldsetl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset8_rel,globl

.begfn __aarch64_ldset8_acq_rel
	jnatom	1f
	ldsetal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset8_acq_rel,globl

.begfn __aarch64_ldset8_sync
	jnatom	1f
	ldsetal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldset8_sync,globl


.begfn __aarch64_ldclr8_relax
	jnatom	1f
	ldclr	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr8_relax,globl

.begfn __aarch64_ldclr8_acq
	jnatom	1f
	ldclra	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	bic	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr8_acq,globl

.begfn __aarch64_ldclr8_rel
	jnatom	1f
	ldclrl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr8_rel,globl

.begfn __aarch64_ldclr8_acq_rel
	jnatom	1f
	ldclral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr8_acq_rel,globl

.begfn __aarch64_ldclr8_sync
	jnatom	1f
	ldclral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldclr8_sync,globl


.begfn __aarch64_ldeor8_relax
	jnatom	1f
	ldeor	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor8_relax,globl

.begfn __aarch64_ldeor8_acq
	jnatom	1f
	ldeora	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	eor	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor8_acq,globl

.begfn __aarch64_ldeor8_rel
	jnatom	1f
	ldeorl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor8_rel,globl

.begfn __aarch64_ldeor8_acq_rel
	jnatom	1f
	ldeoral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor8_acq_rel,globl

.begfn __aarch64_ldeor8_sync
	jnatom	1f
	ldeoral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldeor8_sync,globl


.begfn __aarch64_swp16_relax
	jnatom	1f
	swp	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp16_relax,globl

.begfn __aarch64_swp16_acq
	jnatom	1f
	swpa	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp16_acq,globl

.begfn __aarch64_swp16_rel
	jnatom	1f
	swpl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stlxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp16_rel,globl

.begfn __aarch64_swp16_acq_rel
	jnatom	1f
	swpal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	stlxr	w17,x16,[x1]
	cbnz	w17,0b
	ret
.endfn __aarch64_swp16_acq_rel,globl

.begfn __aarch64_swp16_sync
	jnatom	1f
	swpa	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	stxr	w17,x16,[x1]
	cbnz	w17,0b
	dmb	ish
	ret
.endfn __aarch64_swp16_sync,globl


.begfn __aarch64_cas16_relax
	jnatom	1f
	casp	x0,x1,x2,x3,[x4]
	ret
1:	mov	x16,x0
	mov	x17,x1
0:	ldxp	x0,x1,[x4]
	cmp	x0,x16
	ccmp	x1,x17,#0,eq
	csel	x15,x2,x0,eq
	csel	x14,x3,x1,eq
	stxp	w13,x15,x14,[x4]
	cbnz	w13,0b
	ret
.endfn __aarch64_cas16_relax,globl

.begfn __aarch64_cas16_acq
	jnatom	1f
	caspa	x0,x1,x2,x3,[x4]
	ret
1:	mov	x16,x0
	mov	x17,x1
0:	ldaxp	x0,x1,[x4]
	cmp	x0,x16
	ccmp	x1,x17,#0,eq
	csel	x15,x2,x0,eq
	csel	x14,x3,x1,eq
	stxp	w13,x15,x14,[x4]
	cbnz	w13,0b
	ret
.endfn __aarch64_cas16_acq,globl

.begfn __aarch64_cas16_rel
	jnatom	1f
	caspl	x0,x1,x2,x3,[x4]
	ret
1:	mov	x16,x0
	mov	x17,x1
0:	ldxp	x0,x1,[x4]
	cmp	x0,x16
	ccmp	x1,x17,#0,eq
	csel	x15,x2,x0,eq
	csel	x14,x3,x1,eq
	stlxp	w13,x15,x14,[x4]
	cbnz	w13,0b
	ret
.endfn __aarch64_cas16_rel,globl

.begfn __aarch64_cas16_acq_rel
	jnatom	1f
	caspal	x0,x1,x2,x3,[x4]
	ret
1:	mov	x16,x0
	mov	x17,x1
0:	ldaxp	x0,x1,[x4]
	cmp	x0,x16
	ccmp	x1,x17,#0,eq
	csel	x15,x2,x0,eq
	csel	x14,x3,x1,eq
	stlxp	w13,x15,x14,[x4]
	cbnz	w13,0b
	ret
.endfn __aarch64_cas16_acq_rel,globl

.begfn __aarch64_cas16_sync
	jnatom	1f
	caspal	x0,x1,x2,x3,[x4]
	ret
1:	mov	x16,x0
	mov	x17,x1
0:	ldxp	x0,x1,[x4]
	cmp	x0,x16
	ccmp	x1,x17,#0,eq
	csel	x15,x2,x0,eq
	csel	x14,x3,x1,eq
	stlxp	w13,x15,x14,[x4]
	cbnz	w13,0b
	dmb	ish
	ret
.endfn __aarch64_cas16_sync,globl


.begfn __aarch64_ldadd16_relax
	jnatom	1f
	ldadd	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd16_relax,globl

.begfn __aarch64_ldadd16_acq
	jnatom	1f
	ldadda	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	add	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd16_acq,globl

.begfn __aarch64_ldadd16_rel
	jnatom	1f
	ldaddl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd16_rel,globl

.begfn __aarch64_ldadd16_acq_rel
	jnatom	1f
	ldaddal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldadd16_acq_rel,globl

.begfn __aarch64_ldadd16_sync
	jnatom	1f
	ldaddal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	add	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldadd16_sync,globl


.begfn __aarch64_ldset16_relax
	jnatom	1f
	ldset	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset16_relax,globl

.begfn __aarch64_ldset16_acq
	jnatom	1f
	ldseta	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	orr	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset16_acq,globl

.begfn __aarch64_ldset16_rel
	jnatom	1f
	ldsetl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset16_rel,globl

.begfn __aarch64_ldset16_acq_rel
	jnatom	1f
	ldsetal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldset16_acq_rel,globl

.begfn __aarch64_ldset16_sync
	jnatom	1f
	ldsetal	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	orr	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldset16_sync,globl


.begfn __aarch64_ldclr16_relax
	jnatom	1f
	ldclr	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr16_relax,globl

.begfn __aarch64_ldclr16_acq
	jnatom	1f
	ldclra	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	bic	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr16_acq,globl

.begfn __aarch64_ldclr16_rel
	jnatom	1f
	ldclrl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr16_rel,globl

.begfn __aarch64_ldclr16_acq_rel
	jnatom	1f
	ldclral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldclr16_acq_rel,globl

.begfn __aarch64_ldclr16_sync
	jnatom	1f
	ldclral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	bic	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldclr16_sync,globl


.begfn __aarch64_ldeor16_relax
	jnatom	1f
	ldeor	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor16_relax,globl

.begfn __aarch64_ldeor16_acq
	jnatom	1f
	ldeora	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	eor	x17,x0,x16
	stxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor16_acq,globl

.begfn __aarch64_ldeor16_rel
	jnatom	1f
	ldeorl	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor16_rel,globl

.begfn __aarch64_ldeor16_acq_rel
	jnatom	1f
	ldeoral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldaxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	ret
.endfn __aarch64_ldeor16_acq_rel,globl

.begfn __aarch64_ldeor16_sync
	jnatom	1f
	ldeoral	x0,x0,[x1]
	ret
1:	mov	x16,x0
0:	ldxr	x0,[x1]
	eor	x17,x0,x16
	stlxr	w15,x17,[x1]
	cbnz	w15,0b
	dmb	ish
	ret
.endfn __aarch64_ldeor16_sync,globl
