#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ecomm:	.leafprologue
	mov	ECOMM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ecomm,globl,hidden
