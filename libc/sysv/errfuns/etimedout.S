#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

etimedout:
	.leafprologue
	mov	ETIMEDOUT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	etimedout,globl,hidden
