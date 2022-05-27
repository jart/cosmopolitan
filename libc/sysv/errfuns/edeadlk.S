#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

edeadlk:
	.leafprologue
	mov	EDEADLK(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edeadlk,globl,hidden
