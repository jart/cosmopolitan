#include "libc/macros.internal.h"
.text.unlikely

.section .privileged,"ax",@progbits

el3hlt:	.leafprologue
	mov	EL3HLT(%rip),%ecx
	.errno
	mov	%ecx,(%rax)
	push	$-1
	pop	%rax
	.leafepilogue
	.endfn	el3hlt,globl,hidden
