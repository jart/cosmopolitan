/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#ifndef APE_IDATA_H_
#define APE_IDATA_H_
#ifdef __ASSEMBLER__
#include "ape/relocations.h"
/* clang-format off */

//	Links function from external DLL.
//
//	This embeds a function pointer in the binary. The NT Executive
//	fills its value before control is handed off to the program.
//
//	@note	only ELF toolchains are powerful enough to use this
//	@see	libc/nt/master.sh
//	@see	ape/ape.lds
//	@see	winimp
.macro	.imp	dll:req fn:req actual:req
#ifdef __x86_64__
	.dll	"\dll"
	.section ".piro.data.sort.iat.2.\dll\().2.\actual","aw",@progbits
	.type	\fn,@object
	.align	__SIZEOF_POINTER__
\fn:	.quad	RVA(("\dll\().\actual"))
	.size	\fn,.-\fn
	.globl	\fn
	.hidden	\fn
	.previous
	.section ".idata.ro.ilt.\dll\().2.\actual","a",@progbits
"idata.ilt.\dll\().\actual":
	.quad	RVA("\dll\().\actual")
	.type	"idata.ilt.\dll\().\actual",@object
	.size	"idata.ilt.\dll\().\actual",.-"idata.ilt.\dll\().\actual"
	.previous
	.section ".idata.ro.hnt.\dll\().2.\actual","a",@progbits
"\dll\().\actual":
	.short	0			// hint
	.asciz	"\actual"
	.align	2			// documented requirement
	.globl	"\dll\().\actual"
	.hidden	"\dll\().\actual"
	.type	"\dll\().\actual",@object
	.size	"\dll\().\actual",.-"\dll\().\actual"
	.previous
#else
	.section ".data.nt.\actual","aw",@progbits
	.globl	"\fn"
	.balign	8
	.weak	"\actual"
"\fn":	.quad	"\actual"
#endif
.endm

//	Defines DLL import.
//	@note	this is an implementation detail of .imp
.macro	.dll	name:req
  .section ".idata.ro.idt.2.\name","aG",@progbits,"\name",comdat
	.equ	".Lidata.idt.\name",.
	.long	RVA("idata.ilt.\name")		// ImportLookupTable
	.long	0				// TimeDateStamp
	.long	0				// ForwarderChain
	.long	RVA(".Lidata.str.\name")	// DllNameRva
	.long	RVA("idata.iat.\name")		// ImportAddressTable
	.type	".Lidata.idt.\name",@object
	.size	".Lidata.idt.\name",.-".Lidata.idt.\name"
  .previous
  .section ".idata.ro.ilt.\name\().1","aG",@progbits,"\name",comdat
	.balign	__SIZEOF_POINTER__
	.type	"idata.ilt.\name",@object
"idata.ilt.\name":
	.previous/*
	...
	decentralized content
	...
	*/.section ".idata.ro.ilt.\name\().3","aG",@progbits,"\name",comdat
	.quad	0
  .previous
  .section ".idata.ro.hnt.\name\().1","aG",@progbits,"\name",comdat
	.balign	__SIZEOF_POINTER__
	.type	"idata.hnt.\name",@object
	.equ	"idata.hnt.\name",.
  .previous
  .section ".piro.data.sort.iat.2.\name\().1","awG",@progbits,"\name",comdat
	.balign	__SIZEOF_POINTER__
	.type	"idata.iat.\name",@object
"idata.iat.\name":
	.previous/*
	...
	decentralized content
	...
	*/.section ".piro.data.sort.iat.2.\name\().3","awG",@progbits,"\name",comdat
	.quad	0
  .previous
  .section .rodata.str1.1,"aSM",@progbits,1
".Lidata.str.\name":
	.asciz	"\name\().dll"
  .previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* APE_IDATA_H_ */
