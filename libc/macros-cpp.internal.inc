/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "ape/relocations.h"
/* clang-format off */

#if __MNO_VZEROUPPER__ + 0
#define vzeroupper
#endif

/	Begins definition of frameless function that calls no functions.
.macro	.leafprologue
#if !(defined(TINY) && !defined(__PG__))
	push	%rbp
	mov	%rsp,%rbp
#endif
.endm

/	Ends definition of frameless function that calls no functions.
.macro	.leafepilogue
#if !(defined(TINY) && !defined(__PG__))
	pop	%rbp
#endif
	ret
.endm

/	Good alignment for functions where alignment actually helps.
/	@note 16-byte
.macro	.alignfunc
#ifndef	__OPTIMIZE_SIZE__
	.p2align 4
#endif
.endm

/	Good alignment for loops where alignment actually helps.
/	@note 16-byte if <10 padding otherwise 8-byte
.macro	.alignloop
#ifndef	__OPTIMIZE_SIZE__
	.p2align 4,,10
	.p2align 4
#endif
.endm

/	Loads Effective Address
/	Supporting security blankets
.macro	plea	symbol:req reg64:req reg32:req
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	lea	\symbol(%rip),\reg64
#else
	mov	$\symbol,\reg32
#endif
.endm

/	Loads Effective Address to Stack
/	Supporting security blankets
.macro	pshaddr	symbol:req
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	push	$IMAGE_BASE_VIRTUAL+RVA(\symbol)(%rip),\reg64
#else
	push	$\symbol
#endif
.endm

/	TODO(jart): delete
/	Loads Effective Address
/	Supporting security blankets
.macro	ezlea	symbol:req reg:req
#if	__pic__ + __pie__ + __code_model_medium__ + __code_model_large__ + 0 > 1
/	lea	\symbol(%rip),%r\reg
	mov	$\symbol,%e\reg
#else
	mov	$\symbol,%e\reg
#endif
.endm

.macro	farcall	symbol:req
 .type	\symbol,@function
#if	__PIC__ + __PIE__ + __code_model_medium__ + __code_model_large__ + 0 > 1
	call	*\symbol\()@gotpcrel(%rip)
#else
	call	\symbol\()@plt
#endif
.endm

/	Creates first stack frame.
.macro	.frame0
	and	$-16,%rsp
	xor	%ebp,%ebp
.endm

.macro	.source	symbol:req
.endm

/	Inserts profiling hook in prologue if cc wants it.
/
/	Cosmopolitan does this in a slightly different way from normal
/	GNU toolchains. We always use the -mnop-mcount behavior, since
/	the runtime is able to morph the binary at runtime. It is good
/	since we can put hooks for profiling and function tracing into
/	most builds, without any impact on performance.
/
/	@cost	≥0.3 cycles, 5 bytes
/	@see	build/compile
.macro	.profilable
#ifdef __PG__
1382:
#if defined(__MFENTRY__)
	call	__fentry__
#elif defined(__PIC__) || defined(__PIE__)
/	nopw 0x00(%rax,%rax,1)
	.byte	0x66,0x0f,0x1f,0x44,0x00,0x00
#else
/	nopl 0x00(%rax,%rax,1)
	.byte	0x0f,0x1f,0x44,0x00,0x00
#endif
#if defined(__MRECORD_MCOUNT__) && !defined(__MFENTRY__)
 .section	__mcount_loc,"a",@progbits
  .quad	1382b
 .previous
#endif
#endif
.endm

/	Pushes RVA on stack of linktime mergeable string literal.
/	@see	popstr
.macro	pushstr	text
	.section .rodata.str1.1,"aSM",@progbits,1
.Lstr\@: .asciz	"\text"
	.endobj	.Lstr\@
	.previous
	push	$.Lstr\@ - IMAGE_BASE_VIRTUAL
.endm

/	Pops off stack string address.
/	@see	pushstr
.macro	popstr	dest:req
	addl	$IMAGE_BASE_VIRTUAL,(%rsp)
	pop	\dest
.endm

/	Loads address of linktime mergeable string literal into register.
.macro	getstr	text:req reg64:req reg32 regsz64 regsz32 bias=0
 .section .rodata.str1.1,"aSM",@progbits,1
  .type	.Lstr\@,@object
.Lstr\@: .asciz	"\text"
.Lstr\@.size = .-.Lstr\@ - 1
  .size	.Lstr\@,.-.Lstr\@
 .previous
	plea	.Lstr\@,\reg64,\reg32
 .ifnb	\regsz64
#ifdef	__OPTIMIZE_SIZE__
  .if	.Lstr\@.size + \bias < 128
	pushpop	.Lstr\@.size,\regsz64
  .else
	mov	$.Lstr\@.size,\regsz32
  .endif
#else
	mov	$.Lstr\@.size,\regsz32
#endif
 .endif
.endm

/	TODO(jart): delete
/	Loads address of linktime mergeable string literal into register.
.macro	loadstr	text:req reg:req regsz bias=0
 .section .rodata.str1.1,"aSM",@progbits,1
  .type	.Lstr\@,@object
.Lstr\@: .asciz	"\text"
.Lstr\@.size = .-.Lstr\@ - 1
  .size	.Lstr\@,.-.Lstr\@
 .previous
	ezlea	.Lstr\@,\reg
 .ifnb	\regsz
#ifdef	__OPTIMIZE_SIZE__
  .if	.Lstr\@.size + \bias < 128
	pushpop	.Lstr\@.size,%r\regsz
  .else
	mov	$.Lstr\@.size,%e\regsz
  .endif
#else
	mov	$.Lstr\@.size,%e\regsz
#endif
 .endif
.endm
