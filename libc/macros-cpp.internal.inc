/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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

/	Pulls source code file into ZIP portion of binary.
/
/	@param	symbol is quoted path relative to root e.g. __FILE__
/	@see	see libc/zipos/zipcentraldir.S
/	@see	see libc/integral/c.inc
.macro	.source	symbol:req
#ifndef	IM_FEELING_NAUGHTY
	.yoink	"\symbol"
#endif
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
 .pushsection	__mcount_loc,"a",@progbits
  .quad	1382b
 .popsection
#endif
#endif
.endm

/	Pushes RVA on stack of linktime mergeable string literal.
/	@see	popstr
.macro	pushstr	text
	.pushsection .rodata.str1.1,"aSM",@progbits,1
.Lstr\@: .asciz	"\text"
	.endobj	.Lstr\@
	.popsection
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
 .pushsection .rodata.str1.1,"aSM",@progbits,1
  .type	.Lstr\@,@object
.Lstr\@: .asciz	"\text"
.Lstr\@.size = .-.Lstr\@ - 1
  .size	.Lstr\@,.-.Lstr\@
 .popsection
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
 .pushsection .rodata.str1.1,"aSM",@progbits,1
  .type	.Lstr\@,@object
.Lstr\@: .asciz	"\text"
.Lstr\@.size = .-.Lstr\@ - 1
  .size	.Lstr\@,.-.Lstr\@
 .popsection
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
