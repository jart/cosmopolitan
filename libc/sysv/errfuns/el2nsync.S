#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

el2nsync:
	.leafprologue
	mov	EL2NSYNC(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el2nsync,globl,hidden
