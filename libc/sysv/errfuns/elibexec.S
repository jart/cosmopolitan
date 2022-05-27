#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

elibexec:
	.leafprologue
	mov	ELIBEXEC(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	elibexec,globl,hidden
