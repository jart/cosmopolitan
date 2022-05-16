#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

edotdot:
	.leafprologue
	mov	EDOTDOT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	edotdot,globl,hidden
