#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

ecanceled:
	.leafprologue
	mov	ECANCELED(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	ecanceled,globl,hidden
