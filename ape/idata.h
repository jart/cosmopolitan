/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#ifndef APE_IDATA_H_
#define APE_IDATA_H_
#ifdef __ASSEMBLER__
#include "ape/relocations.h"
/* clang-format off */

/	Links function from external DLL.
/
/	This embeds a function pointer in the binary. The NT Executive
/	fills its value before control is handed off to the program.
/
/	@note	only ELF toolchains are powerful enough to use this
/	@see	libc/nt/master.sh
/	@see	ape/ape.lds
/	@see	winimp
.macro	.imp	dll:req fn:req actual hint
	.dll	\dll
	.section .piro.data.sort.iat.2.\dll\().2.\fn,"aw",@progbits
	.type	\fn,@object
	.align	__SIZEOF_POINTER__
\fn:	.quad	RVA((.L\dll\().\fn))
	.size	\fn,.-\fn
	.globl	\fn
	.hidden	\fn
	.previous
	.section .idata.ro.ilt.\dll\().2.\fn,"a",@progbits
.Lidata.ilt.\dll\().\fn:
	.quad	RVA((.L\dll\().\fn))
	.type	.Lidata.ilt..L\dll\().\fn,@object
	.size	.Lidata.ilt..L\dll\().\fn,.-.Lidata.ilt.\dll\().\fn
	.previous
	.section .idata.ro.hnt.\dll\().2.\fn,"a",@progbits
.L\dll\().\fn:
	.ifnb	\hint			# hint i.e. guess function ordinal
	.short	\hint
	.else
	.short	0
	.endif
	.ifnb	\actual			# name
	.asciz	"\actual"
	.else
	.asciz	"\fn"
	.endif
	.align	2			# documented requirement
/	.globl	.L\dll\().\fn
/	.hidden	.L\dll\().\fn
	.type	.L\dll\().\fn,@object
	.size	.L\dll\().\fn,.-.L\dll\().\fn
	.previous
.endm

/	Defines DLL import.
/	@note	this is an implementation detail of .imp
.macro	.dll	name:req
  .section .idata.ro.idt.2.\name,"aG",\name,comdat
	.equ	.Lidata.idt.\name,.
	.long	RVA(idata.ilt.\name)		# ImportLookupTable
	.long	0				# TimeDateStamp
	.long	0				# ForwarderChain
	.long	RVA(.Lidata.str.\name)		# DllNameRva
	.long	RVA(idata.iat.\name)		# ImportAddressTable
	.type	.Lidata.idt.\name,@object
	.size	.Lidata.idt.\name,.-.Lidata.idt.\name
  .previous
  .section .idata.ro.ilt.\name\().1,"aG",\name,comdat
	.align	__SIZEOF_POINTER__
	.type	idata.ilt.\name,@object
idata.ilt.\name:
	.previous/*
	...
	decentralized content
	...
	*/.section .idata.ro.ilt.\name\().3,"aG",\name,comdat
	.quad	0
  .previous
  .section .idata.ro.hnt.\name\().1,"aG",\name,comdat
	.align	__SIZEOF_POINTER__
	.type	idata.hnt.\name,@object
	.equ	idata.hnt.\name,.
  .previous
  .section .piro.data.sort.iat.2.\name\().1,"awG",\name,comdat
	.align	__SIZEOF_POINTER__
	.type	idata.iat.\name,@object
idata.iat.\name:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.data.sort.iat.2.\name\().3,"awG",\name,comdat
	.quad	0
  .previous
  .pushsection .rodata.str1.1,"aSM",@progbits,1
.Lidata.str.\name:
	.asciz	"\name\().dll"
  .popsection
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* APE_IDATA_H_ */
