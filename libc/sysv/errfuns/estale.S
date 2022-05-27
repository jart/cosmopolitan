#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

estale:	.leafprologue
	mov	ESTALE(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	estale,globl,hidden
