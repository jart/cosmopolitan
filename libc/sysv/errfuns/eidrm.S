#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eidrm:	.leafprologue
	mov	EIDRM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eidrm,globl,hidden
