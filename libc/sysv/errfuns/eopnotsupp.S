#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eopnotsupp:
	.leafprologue
	mov	EOPNOTSUPP(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eopnotsupp,globl,hidden
