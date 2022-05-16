#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

erestart:
	.leafprologue
	mov	ERESTART(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	erestart,globl,hidden
