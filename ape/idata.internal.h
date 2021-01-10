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
.macro	.imp	dll:req fn:req actual:req hint
	.dll	\dll
	.section .piro.data.sort.iat.2.\dll\().2.\actual,"aw",@progbits
	.type	\fn,@object
	.align	__SIZEOF_POINTER__
\fn:	.quad	RVA((\dll\().\actual))
	.size	\fn,.-\fn
	.globl	\fn
	.hidden	\fn
	.previous
	.section .idata.ro.ilt.\dll\().2.\actual,"a",@progbits
.Lidata.ilt.\dll\().\actual:
	.quad	RVA((\dll\().\actual))
	.type	.Lidata.ilt.\dll\().\actual,@object
	.size	.Lidata.ilt.\dll\().\actual,.-.Lidata.ilt.\dll\().\actual
	.previous
	.section .idata.ro.hnt.\dll\().2.\actual,"a",@progbits
\dll\().\actual:
	.ifnb	\hint			# hint i.e. guess function ordinal
	.short	\hint
	.else
	.short	0
	.endif
	.asciz	"\actual"
	.align	2			# documented requirement
	.globl	\dll\().\actual
	.hidden	\dll\().\actual
	.type	\dll\().\actual,@object
	.size	\dll\().\actual,.-\dll\().\actual
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
  .section .rodata.str1.1,"aSM",@progbits,1
.Lidata.str.\name:
	.asciz	"\name\().dll"
  .previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* APE_IDATA_H_ */
