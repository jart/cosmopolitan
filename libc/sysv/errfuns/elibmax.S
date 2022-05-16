#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elibmax:
	.leafprologue
	mov	ELIBMAX(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibmax,globl,hidden
