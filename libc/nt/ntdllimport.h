#ifndef COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#define COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#include "ape/relocations.h"
#include "libc/macros.h"
#ifdef __ASSEMBLER__
/* clang-format off */

.macro	.ntimp	fn:req
	yoink	_init_ntdll
	.initbss 202,_init_ntdll.\fn
__imp_\fn:
	.quad	0
	.endobj	__imp_\fn,globl,hidden
	.previous
	.initro 202,_init_ntdll.2.\fn
	.quad	RVA(.L\fn)
	.previous
	.section .rodata.str1.1,"aSM",@progbits,1
.L\fn:
	.asciz	"\fn"
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_ */
