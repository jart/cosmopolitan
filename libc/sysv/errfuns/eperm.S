#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

eperm:	.leafprologue
	mov	EPERM(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	eperm,globl,hidden
