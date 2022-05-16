#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

el2hlt:	.leafprologue
	mov	EL2HLT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el2hlt,globl,hidden
