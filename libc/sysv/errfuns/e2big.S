#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

e2big:	.leafprologue
	mov	E2BIG(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	e2big,globl,hidden
